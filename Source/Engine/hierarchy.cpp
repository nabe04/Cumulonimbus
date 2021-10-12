#include "hierarchy.h"

#include "ecs.h"
#include "locator.h"
#include "prefab_loader.h"
#include "hierarchy_component.h"

namespace
{
	const std::string context_id{ "Context_Menu" };	// ImGui::PopupのコンテキストメニューID
}

namespace cumulonimbus::editor
{
	void Hierarchy::Render(ecs::Registry* registry)
	{
		if (ImGui::Begin(ICON_FA_ALIGN_RIGHT" Hierarchy"))
		{
			if (ImGui::Button(ICON_FA_PLUS, { 30,30 }))
				ImGui::OpenPopup("my_file_popup");
			if (ImGui::BeginPopup("my_file_popup"))
			{
				if (ImGui::MenuItem("Create Empty"))
				{
					registry->CreateEntity();
				}
				ImGui::EndPopup();
			}

			for (auto& [key, value] : registry->GetEntities())
			{
				//value.second
				//if (ImGui::Selectable(value.second.c_str(), selected_entity == key,ImGuiSelectableFlags_AllowItemOverlap))
				//{
				//	selected_entity = key;

				//}

				ImGui::Selectable(value.second.c_str(), selected_entity == key);
				if(ImGui::IsItemHovered())
				{// ヒエラルキービュー上でのアイテム選択
					if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{// 左クリック時
						selected_entity = key;
					}
					if(ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{// 右クリック時
						selected_entity = key;
						ImGui::OpenPopup(context_id.c_str());
					}
				}
			}

			//
			ContextMenu(registry);

			{// 新規Hierarchy
				std::filesystem::path current_scene_path = locator::Locator::GetSystem()->GetCurrentScenePath();
				if (ImGui::TreeNode(current_scene_path.filename().replace_extension().string().c_str()))
				{
					// シーン階層へのドラッグ & ドロップ処理
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						ImGui::SetDragDropPayload("cell", &selected_entity, sizeof(mapping::rename_type::Entity));
						ImGui::Text(registry->GetName(selected_entity).c_str());
						ImGui::EndDragDropSource();
					}
					if (ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("cell");
						if (payload)
						{

							const mapping::rename_type::Entity drag_ent = *static_cast<mapping::rename_type::Entity*>(payload->Data);
							auto drag_name = registry->GetName(drag_ent);
							auto name = registry->GetName(selected_entity);
							int a;
							a = 0;
						}

						ImGui::EndDragDropTarget();
					}
					for (auto& [key, value] : registry->GetEntities())
					{
						if (registry->HasComponent<component::HierarchyComponent>(key))
							continue;

						EntityTree(registry, key, registry->GetName(key));
					}


					ImGui::TreePop();
				}

			}
		}
		ImGui::End();

		if(ImGui::Begin("Hierarchy Data"))
		{
			ImGui::Text(registry->GetName(selected_entity).c_str());
		}
		ImGui::End();
	}

	void Hierarchy::ContextMenu(ecs::Registry* registry)
	{
		if(ImGui::BeginPopup(context_id.c_str()))
		{
			if (ImGui::MenuItem("Create Prefab"))
			{
				CreatePrefab(registry, selected_entity);
			}
			ImGui::MenuItem("Delete");


			ImGui::EndPopup();
		}
	}

	void Hierarchy::CreatePrefab(ecs::Registry* registry, const mapping::rename_type::Entity& ent)
	{
		asset::AssetManager* asset_manager = locator::Locator::GetAssetManager();
		const std::string& ent_name = registry->GetName(selected_entity);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager, registry, selected_entity, false, ent_name);
	}

	void Hierarchy::EntityTree(
		ecs::Registry* registry,
		const mapping::rename_type::Entity& ent,
		const std::string& entity_name)
	{
		bool is_last = true;

		ImGuiTreeNodeFlags flg{};
		ecs::ComponentArray<component::HierarchyComponent>& hierarchy_comp_array = registry->GetArray<component::HierarchyComponent>();
		for (const auto& hierarchy_comp : hierarchy_comp_array.GetComponents())
		{
			if (hierarchy_comp.GetParentEntity() != ent)
				continue;

			is_last = false;
		}
		if(HasParentEntity(registry, selected_entity, ent))
		{
			int a;
			a = 0;
		}
		// 現エンティティの小階層が存在しないときTreeNodeの矢印を表示しない
		if (is_last)
			flg |= ImGuiTreeNodeFlags_Leaf;
		// 現エンティティが選択されている場合TreeNodeを選択状態にする
		if (ent == selected_entity)
			flg |= ImGuiTreeNodeFlags_Selected;
		// ノード作成
		const bool node_opened = ImGui::TreeNodeEx(entity_name.c_str(), flg);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			selected_entity = ent;
		}
		// ドラック & ドロップ操作
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("cell", &ent, sizeof(mapping::rename_type::Entity));
			ImGui::Text(registry->GetName(selected_entity).c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (ImGui::AcceptDragDropPayload("cell"))
			{
				auto drag_name	= registry->GetName(selected_entity);
				auto name		= registry->GetName(ent);
			}

			ImGui::EndDragDropTarget();
		}

		if (node_opened)
		{
			for (const auto& hierarchy_comp : hierarchy_comp_array.GetComponents())
			{
				if (hierarchy_comp.GetParentEntity() != ent)
					continue;

				is_last = false;
				EntityTree(registry, hierarchy_comp.GetEntity(), registry->GetName(hierarchy_comp.GetEntity()));
			}
			ImGui::TreePop();
		}

		// is_last ?  ImGuiTreeNodeFlags_Leaf | flg : ImGuiTreeNodeFlags_None | flg
	}

	bool Hierarchy::HasParentEntity(
		ecs::Registry* registry,
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
