#include "hierarchy.h"

#include <ranges>

#include "ecs.h"
#include "locator.h"
#include "prefab_loader.h"
#include "hierarchy_component.h"
#include "scene.h"
#include "scene_manager.h"
#include "scene_loader.h"
#include "filename_helper.h"

namespace
{
	const std::string context_id{ "context_menu" };	// ImGui::PopupのコンテキストメニューID
	const std::string dragged_id{ "dragged_entity" };
}

namespace cumulonimbus::editor
{
	void Hierarchy::Render(
		mapping::rename_type::UUID& selected_scene_id,
		const std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes,
		const ProjectView& project_view)
	{
		scene::Scene& selected_scene = *active_scenes.at(selected_scene_id).get();

		if (ImGui::Begin(ICON_FA_ALIGN_RIGHT" Hierarchy"))
		{
			//-- ウィンドウパラメータの設定 --//
			window_pos = DirectX::SimpleMath::Vector2{ ImGui::GetCurrentWindow()->Pos.x,ImGui::GetCurrentWindow()->Pos.y + title_bar_height };
			window_size = DirectX::SimpleMath::Vector2{ ImGui::GetContentRegionAvail().x ,ImGui::GetContentRegionAvail().y };
			title_bar_height = ImGui::GetCurrentWindow()->TitleBarHeight();

			//-- エンティティ追加処理 --//
			if (ImGui::Button(ICON_FA_PLUS, { 30,30 }))
				ImGui::OpenPopup("my_file_popup");
			if (ImGui::BeginPopup("my_file_popup"))
			{
				if (ecs::Registry& registry = *selected_scene.GetRegistry();
					ImGui::MenuItem("Create Empty"))
				{
					// 現在選択されているシーンへのエンティティ追加
					registry.CreateEntity();
				}
				ImGui::EndPopup();
			}
		}

		//-- 開かれているシーン分のエンティティ表示 & 編集 --//
		for (const auto& [scene_id, scene] : active_scenes)
		{
			auto& registry = *scene->GetRegistry();
			std::string scene_name = scene->GetSceneName();
			if (scene->GetSceneName().empty())
			{
				scene_name = filename_helper::GetNoTitled();
			}

			ImGui::PushID(scene_id.c_str());
			if (const std::filesystem::path current_scene_path = locator::Locator::GetSystem()->GetCurrentScenePath();
				ImGui::TreeNodeEx(std::string{ ICON_FA_CLOUD_SUN + scene_name }.c_str(),
					ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
			{
				if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{// 選択されているシーンの変更
					selected_scene_id = scene_id;
				}

				//-- シーン階層へのドラッグ & ドロップ処理 --//
				if (ImGui::BeginDragDropTarget())
				{// シーン階層に対してのエンティティドロップ処理
					if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
					{
						// 一番上の親に設定
						registry.GetComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(&registry, {});
					}

					ImGui::EndDragDropTarget();
				}

				{//-- 現在選択されているエンティティの小階層エンティティを取得 --//
					sub_hierarchical_entities.clear();
					for (const auto& hierarchy_comp : registry.GetArray<component::HierarchyComponent>().GetComponents())
					{
						if (HasParentEntity(&registry, selected_entity, hierarchy_comp.GetEntity()))
						{
							sub_hierarchical_entities.emplace_back(hierarchy_comp.GetEntity());
						}
					}
				}

				for (const auto& key : registry.GetEntities() | std::views::keys)
				{
					// エンティティの階層表示
					// 初めは一番上の親階層から始める
					if (!registry.GetComponent<component::HierarchyComponent>(key).GetParentEntity().empty())
						continue;;

					//ImGui::Separator();
					is_dragged_entity = false;
					EntityTree(&registry, selected_scene_id, scene_id, key, registry.GetName(key));
				}
				DeleteEntity(&registry);
				ImGui::TreePop();
			}

			//-- ドラッグ & ドロップでのシーン追加読み込み --//
			std::filesystem::path selected_asset = ""; // アセットのリセット
			if (IsWindowHovered())
			{
				if (project_view.DraggingAsset(selected_asset))
				{
					if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
					   (selected_asset.extension().string() == file_path_helper::GetSceneExtension()))
					{
						scene::SceneManager& scene_manager = *active_scenes.begin()->second->GetSceneManager();
						locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->AddScene(scene_manager, selected_asset);
					}
				}
			}
			ImGui::PopID();
		}

		ImGui::End();
	}

	void Hierarchy::Render(ecs::Registry* const registry)
	{
		//if (ImGui::Begin(ICON_FA_ALIGN_RIGHT" Hierarchy"))
		//{
		//	if (ImGui::Button(ICON_FA_PLUS, { 30,30 }))
		//		ImGui::OpenPopup("my_file_popup");
		//	if (ImGui::BeginPopup("my_file_popup"))
		//	{
		//		if (ImGui::MenuItem("Create Empty"))
		//		{
		//			registry->CreateEntity();
		//		}
		//		ImGui::EndPopup();
		//	}

		//	if (const std::filesystem::path current_scene_path = locator::Locator::GetSystem()->GetCurrentScenePath();
		//		ImGui::TreeNodeEx(std::string{ ICON_FA_CLOUD_SUN + current_scene_path.filename().replace_extension().string() }.c_str(),
		//		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
		//	{
		//		//-- シーン階層へのドラッグ & ドロップ処理 --//
		//		if (ImGui::BeginDragDropTarget())
		//		{// シーン階層に対してのエンティティドロップ処理
		//			if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
		//			{
		//				// 一番上の親に設定
		//				registry->GetComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(registry, {});
		//			}

		//			ImGui::EndDragDropTarget();
		//		}

		//		{//-- 現在選択されているエンティティの小階層エンティティを取得 --//
		//			sub_hierarchical_entities.clear();
		//			for (const auto& hierarchy_comp : registry->GetArray<component::HierarchyComponent>().GetComponents())
		//			{
		//				if (HasParentEntity(registry, selected_entity, hierarchy_comp.GetEntity()))
		//				{
		//					sub_hierarchical_entities.emplace_back(hierarchy_comp.GetEntity());
		//				}
		//			}
		//		}

		//		for (auto& [key, value] : registry->GetEntities())
		//		{
		//			// エンティティの階層表示
		//			// 初めは一番上の親階層から始める
		//			if (!registry->GetComponent<component::HierarchyComponent>(key).GetParentEntity().empty())
		//				continue;;

		//			//ImGui::Separator();
		//			is_dragged_entity = false;
		//			EntityTree(registry, key, registry->GetName(key));
		//		}
		//		DeleteEntity(registry);
		//		ImGui::TreePop();
		//	}

		//}
		//ImGui::End();

		if(ImGui::Begin("Hierarchy Data"))
		{
			if(registry->HasComponent<component::HierarchyComponent>(selected_entity))
			{
				const auto& hierarchy_comp = registry->GetComponent<component::HierarchyComponent>(selected_entity);
				ImGui::Text("Parent Ent : %s", registry->GetName(hierarchy_comp.GetParentEntity()).c_str());
				ImGui::Text("First Child : %s", registry->GetName(hierarchy_comp.GetFirstChild()).c_str());
				ImGui::Text("Back Sibling : %s", registry->GetName(hierarchy_comp.GetBackSibling()).c_str());
				ImGui::Text("Next Sibling : %s", registry->GetName(hierarchy_comp.GetNextSibling()).c_str());
				if(!hierarchy_comp.GetParentEntity().empty())
				{
					ImGui::Text("Parent Ent Exist");
				}
				if (!hierarchy_comp.GetFirstChild().empty())
				{
					ImGui::Text("First Child Ent Exist");
				}
				if (!hierarchy_comp.GetNextSibling().empty())
				{
					ImGui::Text("Next Sibling Ent Exist");
				}
				if (!hierarchy_comp.GetBackSibling().empty())
				{
					ImGui::Text("Back Sibling Ent Exist");
				}
			}
			ImGui::Separator();
			ImGui::Text("--Selected Entity--");
			ImGui::Text(registry->GetName(selected_entity).c_str());
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Text("--Sub Entities--");
			for(const auto& ent : sub_hierarchical_entities)
			{
				ImGui::Text(registry->GetName(ent).c_str());
			}
		}
		ImGui::End();
	}

	void Hierarchy::ContextMenu(ecs::Registry* const registry)
	{
		if(ImGui::BeginPopup(context_id.c_str()))
		{
			if(registry->HasComponent<component::HierarchyComponent>(selected_entity))
			{// Hierarchy Componentを所持していない場合 -> 一番上の親階層の場合Prefab作成を可能に
				if (ImGui::MenuItem("Create Prefab"))
				{
					std::vector<std::string> entities = sub_hierarchical_entities;
					entities.emplace_back(selected_entity);

					if(auto& selected_hierarchy_comp = registry->GetComponent<component::HierarchyComponent>(selected_entity);
					   !selected_hierarchy_comp.GetParentEntity().empty())
					{// 選択されたエンティティが親のエンティティを保持していれば
						// 一番上の階層の親にするために親階層をなくす
						selected_hierarchy_comp.SetParentEntity({});
					}

					CreatePrefab(registry, entities);
				}
			}

			if(ImGui::MenuItem("Delete"))
			{// エンティティの削除
				destroyed_entity = selected_entity;
			}

			ImGui::EndPopup();
		}
	}

	bool Hierarchy::IsWindowHovered() const
	{
		if (const ImVec2 cursor_pos = ImGui::GetMousePos();
			cursor_pos.x < window_pos.x ||
			cursor_pos.x > window_pos.x + window_size.x ||
			cursor_pos.y < window_pos.y ||
			cursor_pos.y > window_pos.y + window_size.y)
		{
			return false;
		}

		return true;
	}


	void Hierarchy::CreatePrefab(ecs::Registry* const registry, const mapping::rename_type::Entity& ent)
	{
		asset::AssetManager* asset_manager	= locator::Locator::GetAssetManager();
		const std::string&	 ent_name		= registry->GetName(selected_entity);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager, registry, selected_entity, false, ent_name);
	}

	void Hierarchy::CreatePrefab(
		ecs::Registry* registry,
		const std::vector<mapping::rename_type::Entity>& entities)
	{
		asset::AssetManager* asset_manager = locator::Locator::GetAssetManager();
		const std::string& ent_name = registry->GetName(selected_entity);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager, registry, entities, false, ent_name);
	}

	void Hierarchy::DeleteEntity(ecs::Registry* registry)
	{
		if (destroyed_entity.empty())
			return;
		registry->GetComponent<component::HierarchyComponent>(destroyed_entity).OnDestroyed(registry);
		for(const auto& sub_ent : sub_hierarchical_entities)
		{
			registry->GetComponent<component::HierarchyComponent>(sub_ent).OnDestroyed(registry);
		}

		registry->Destroy(destroyed_entity);
		for (const auto& sub_ent : sub_hierarchical_entities)
		{
			registry->Destroy(sub_ent);
		}
		destroyed_entity = "";
		selected_entity  = "";
	}


	void Hierarchy::EntityTree(
		ecs::Registry* const registry,
		mapping::rename_type::UUID& selected_scene_id,
		const mapping::rename_type::UUID& current_scene_id,
		const mapping::rename_type::Entity& ent,
		const std::string& entity_name)
	{
		bool is_last		{ true };	// 現エンティティに子供の階層が存在しないか
		bool is_selectable	{ true };	// ドラッグ & ドロップでの親子付け時に選択可能か

		ImGuiTreeNodeFlags node_flg{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
		ecs::ComponentArray<component::HierarchyComponent>& hierarchy_comp_array = registry->GetArray<component::HierarchyComponent>();
		for (const auto& hierarchy_comp : hierarchy_comp_array.GetComponents())
		{
			// 現在のエンティティに子供がいないか判定
			// もし子供がいなければImGuiTreeにImGuiTreeNodeFlags_Leafを追加する
			if (hierarchy_comp.GetParentEntity() != ent)
				continue;

			is_last = false;
		}
		if(HasParentEntity(registry, selected_entity, ent))
		{
			is_selectable = false;
		}

		{// ImGui::TreeNodeExでのフラグ、テキスト色の設定
			// 現エンティティの小階層が存在しないときTreeNodeの矢印を表示しない
			if (is_last)
				node_flg |= ImGuiTreeNodeFlags_Leaf;
			// 現エンティティが選択されている場合TreeNodeを選択状態にする
			if (ent == selected_entity)
				node_flg |= ImGuiTreeNodeFlags_Selected;
			if (!is_selectable && is_dragged_entity)
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
		}
		ImGui::PushID(ent.c_str());
		// ノード作成
		const bool is_opened = ImGui::TreeNodeEx(std::string{ ICON_FA_CUBE + entity_name }.c_str(),
												 node_flg);

		if (!is_selectable && is_dragged_entity)
			ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{// ヒエラルキービュー上でのアイテム選択
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{// 左クリック時
				selected_entity = ent;
				selected_scene_id = current_scene_id;
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{// 右クリック時
				selected_entity = ent;
				ImGui::OpenPopup(context_id.c_str());
			}
		}

		// ドラック & ドロップ操作
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			is_dragged_entity = true;
			ImGui::SetDragDropPayload(dragged_id.c_str(), &ent, sizeof(mapping::rename_type::Entity));
			ImGui::Text(registry->GetName(selected_entity).c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget() && is_selectable)
		{
			if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
			{// Hierarchy Componentの親エンティティを変更
				registry->GetOrEmplaceComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(registry, ent);
			}

			ImGui::EndDragDropTarget();
		}

		if(ent == selected_entity)
		{
			ContextMenu(registry);
		}

		if (is_opened)
		{
			for (const auto& hierarchy_comp : hierarchy_comp_array.GetComponents())
			{
				if (hierarchy_comp.GetParentEntity() != ent)
					continue;

				is_last = false;
				EntityTree(registry, selected_scene_id, current_scene_id , hierarchy_comp.GetEntity(), registry->GetName(hierarchy_comp.GetEntity()));
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	bool Hierarchy::HasParentEntity(
		ecs::Registry* const registry,
		const mapping::rename_type::Entity& selected_ent,
		const mapping::rename_type::Entity& ent) const
	{
		if (component::HierarchyComponent* hierarchy_comp = registry->TryGetComponent<component::HierarchyComponent>(ent);
			hierarchy_comp)
		{
			const auto& parent_ent = hierarchy_comp->GetParentEntity();
			if (selected_ent == parent_ent)
				return true;
			return HasParentEntity(registry, selected_ent, parent_ent);
		}

		return false;
	}


} // cumulonimbus::editor
