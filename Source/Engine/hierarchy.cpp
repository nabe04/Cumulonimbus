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
	// ImGui::PopupのコンテキストメニューID
	const std::string context_id{ "context_menu" };
	// ImGui::Popupのドラッグ中のエンティティID
	const std::string dragged_id{ "dragged_entity" };
	// シーン間のエンティティ移動時の一時保存ファイルパス
	const std::string transfer_scene_path{ "./Data/TransferScene" };
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
			window_pos		 = DirectX::SimpleMath::Vector2{ ImGui::GetCurrentWindow()->Pos.x,ImGui::GetCurrentWindow()->Pos.y + title_bar_height };
			window_size		 = DirectX::SimpleMath::Vector2{ ImGui::GetContentRegionAvail().x ,ImGui::GetContentRegionAvail().y };
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

			ImGui::PushID(scene_id.c_str());
			if (scene->GetSceneName().empty())
			{
				scene_name = filename_helper::GetNoTitled();
			}

			//-- 「表示」、「非表示」ボタン --//
			std::string eye_button_name{};
			if(scene->GetIsVisible())
			{
				eye_button_name = ICON_FA_EYE;
			}
			else
			{
				eye_button_name = ICON_FA_EYE_SLASH;
			}
			if(ImGui::Button(eye_button_name.c_str()))
			{// シーン表示内容の反転
				scene->SetIsVisible(!scene->GetIsVisible());
			}
			ImGui::SameLine();

			ImGuiTreeNodeFlags tree_flg = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
			if(selected_scene_id == scene_id)
			{
				tree_flg |= ImGuiTreeNodeFlags_Selected;
				ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4{ 1,0,1,1 });
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 100.f / 255.f,100.f / 255.f,100.f / 255.f,1 });
			}

			//-- シーン単位でのヒエラルキー表示 --//
			if (const std::filesystem::path current_scene_path = locator::Locator::GetSystem()->GetCurrentScenePath();
				ImGui::TreeNodeEx(std::string{ ICON_FA_CLOUD_SUN + scene_name }.c_str(),tree_flg))
			{

				if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					// 選択されているシーンの変更
					selected_scene_id		= scene_id;
					// 遷移元のシーン変更(ヒエラルキーの変更に使用)
					transition_source_scene = scene.get();
				}

				//-- シーン階層へのドラッグ & ドロップ処理 --//
				if (ImGui::BeginDragDropTarget())
				{
					// 遷移先のシーン更新
					destination_scene = scene.get();
					// シーン階層に対してのエンティティドロップ処理
					if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
					{
						if(transition_source_scene == destination_scene)
						{	// 一番上の親に設定
							registry.GetComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(&registry, {});
						}
						else
						{
							// シーン間の移動エンティティ(親のみ)のセット
							scene_transfer_entity	  = selected_entity;
							// 遷移先の親エンティティのセット
							// -> 一番上の親階層になるため空で良い
							destination_parent_entity = {};
						}
					}

					ImGui::EndDragDropTarget();
				}

				{//-- 現在選択されているエンティティの小階層エンティティを取得 --//
					if(selected_scene_id == scene_id)
					{
						sub_hierarchical_entities.clear();
						for (const auto& hierarchy_comp : registry.GetArray<component::HierarchyComponent>().GetComponents())
						{
							if (HasParentEntity(&registry, selected_entity, hierarchy_comp.GetEntity()))
							{
								sub_hierarchical_entities.emplace_back(hierarchy_comp.GetEntity());
							}
						}
					}
				}

				for (const auto& key : registry.GetEntities() | std::views::keys)
				{
					// エンティティの階層表示
					// 初めは一番上の親階層から始める
					if (!registry.GetComponent<component::HierarchyComponent>(key).GetParentEntity().empty())
						continue;;

					is_dragged_entity = false;
					EntityTree(&registry, selected_scene_id, scene_id, key, registry.GetName(key));
				}

				if (!scene_transfer_entity.empty())
				{// シーン間の遷移を行いたいエンティティ(scene_transfer_entity)
				 // が存在した場合のエンティティ削除処理
					SceneTransferEntity();
				}

				if(!destroyed_entity.empty())
				{// 削除したいエンティティ(destroyed_entity)
				 // が存在した場合のエンティティ削除処理
					DeleteEntity(&registry);
				}

				ImGui::TreePop();
			}

			ImGui::PopStyleColor();

			//-- ドラッグ & ドロップでのシーン追加読み込み --//
			if (std::filesystem::path selected_asset = ""; // アセットのリセット
				IsWindowHovered())
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

		if (ImGui::Begin("Hierarchy Data"))
		{
			ecs::Registry* registry = selected_scene.GetRegistry();
			if (registry->HasComponent<component::HierarchyComponent>(selected_entity))
			{
				const auto& hierarchy_comp = registry->GetComponent<component::HierarchyComponent>(selected_entity);
				ImGui::Text("Parent Ent : %s", registry->GetName(hierarchy_comp.GetParentEntity()).c_str());
				ImGui::Text("First Child : %s", registry->GetName(hierarchy_comp.GetFirstChild()).c_str());
				ImGui::Text("Back Sibling : %s", registry->GetName(hierarchy_comp.GetBackSibling()).c_str());
				ImGui::Text("Next Sibling : %s", registry->GetName(hierarchy_comp.GetNextSibling()).c_str());
				if (!hierarchy_comp.GetParentEntity().empty())
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
			for (const auto& ent : sub_hierarchical_entities)
			{
				ImGui::Text(registry->GetName(ent).c_str());
			}
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


	void Hierarchy::CreatePrefab(
		ecs::Registry* const registry,
		const mapping::rename_type::Entity& ent) const
	{
		asset::AssetManager* asset_manager	= locator::Locator::GetAssetManager();
		const std::string&	 ent_name		= registry->GetName(selected_entity);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager, registry, selected_entity, false, ent_name);
	}

	void Hierarchy::CreatePrefab(
		ecs::Registry* registry,
		const std::vector<mapping::rename_type::Entity>& entities) const
	{
		asset::AssetManager* asset_manager = locator::Locator::GetAssetManager();
		const std::string& ent_name = registry->GetName(selected_entity);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager, registry, entities, false, ent_name);
	}

	void Hierarchy::SceneTransferEntity()
	{
		// 遷移元のレジストリ
		ecs::Registry& transition_source_registry = *transition_source_scene->GetRegistry();
		// 遷移先のレジストリ
		ecs::Registry& destination_registry		  = *destination_scene->GetRegistry();

		// 遷移したいエンティティ(scene_transfer_entity)を一番上の親階層に持ってくる
		// -> Hierarchy Componentのparent、first_child、next_sibling、back_siblingの再設定が必要なため
		transition_source_registry.GetComponent<component::HierarchyComponent>(scene_transfer_entity).SetParentEntity(&transition_source_registry, {});

		// 移動したいエンティティ郡
		std::vector<std::string> entities = sub_hierarchical_entities;
		entities.emplace_back(scene_transfer_entity);

		// プレハブとして一時保存するファイルパス
		const std::string			ent_name  = transition_source_registry.GetName(scene_transfer_entity);
		const std::filesystem::path save_path = transfer_scene_path + "/" +
												ent_name + "/" +
												ent_name + file_path_helper::GetPrefabExtension();

		//-- 遷移先で遷移させたいエンティティ分のエンティティ作成 --//
		// プレハブとして一時保存
		asset::Prefab transition_ent_prefab{};
		// 遷移元のレジストリとして保存
		transition_ent_prefab.CreatePrefab(&transition_source_registry, entities, save_path);

		// 遷移元の移動対象エンティティの削除
		for(const auto& ent : entities)
		{
			transition_source_registry.Destroy(ent);
		}

		// 遷移先のレジストリにインスタンスを作成
		// ent : 遷移先でプレハブから作成された親エンティティ
		const mapping::rename_type::Entity parent_ent = transition_ent_prefab.Instantiate(&destination_registry);

		// 遷移対象のエンティティが遷移先で子エンティティになる時
		if(!destination_parent_entity.empty())
		{
			destination_registry.GetComponent<component::HierarchyComponent>(parent_ent).SetParentEntity(&destination_registry, destination_parent_entity);
			destination_parent_entity.clear();
		}

		// 遷移対象エンティティのリセット
		scene_transfer_entity.clear();
		// 一時保存したファイルの削除
		std::filesystem::remove_all(save_path.parent_path());
	}

	void Hierarchy::DeleteEntity(ecs::Registry* registry)
	{
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
				// 選択されているエンティティの更新
				selected_entity = ent;
				// 選択されているシーンの更新
				selected_scene_id = current_scene_id;
				// 遷移元のシーン更新
				transition_source_scene = registry->GetScene();
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{// 右クリック時
				// 選択されているエンティティの更新
				selected_entity = ent;
				// ポップアップルメニュー(ContextMenu)を開く前処理
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
			// 遷移先のシーン更新
			destination_scene = registry->GetScene();
			// アセットがドロップされた時
			if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
			{
				// 遷移元のシーンと遷移先のシーンが同じ時
				if(transition_source_scene == destination_scene)
				{// Hierarchy Componentの親エンティティを変更
					registry->GetOrEmplaceComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(registry, ent);
				}
				else
				{
					// シーン間の移動エンティティ(親のみ)のセット
					scene_transfer_entity	   = selected_entity;
					// 遷移先の親エンティティのセット
					destination_parent_entity  = ent;
				}
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
