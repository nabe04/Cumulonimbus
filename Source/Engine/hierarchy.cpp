#include "hierarchy.h"

#include "ecs.h"
#include "locator.h"
#include "prefab_loader.h"
#include "hierarchy_component.h"

namespace
{
	const std::string context_id{ "context_menu" };	// ImGui::Popup�̃R���e�L�X�g���j���[ID
	const std::string dragged_id{ "dragged_entity" };
}

namespace cumulonimbus::editor
{
	void Hierarchy::Render(ecs::Registry* const registry)
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

			//for (auto& [key, value] : registry->GetEntities())
			//{
			//	ImGui::Selectable(value.second.c_str(), selected_entity == key);
			//	//if(ImGui::IsItemHovered())
			//	//{// �q�G�����L�[�r���[��ł̃A�C�e���I��
			//	//	if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			//	//	{// ���N���b�N��
			//	//		selected_entity = key;
			//	//	}
			//	//	if(ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			//	//	{// �E�N���b�N��
			//	//		selected_entity = key;
			//	//		ImGui::OpenPopup(context_id.c_str());
			//	//	}
			//	//}
			//}

			{// �V�KHierarchy
				std::filesystem::path current_scene_path = locator::Locator::GetSystem()->GetCurrentScenePath();
				if (ImGui::TreeNodeEx(std::string{ ICON_FA_CLOUD_SUN + current_scene_path.filename().replace_extension().string() }.c_str(),
									  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
				{
					//-- �V�[���K�w�ւ̃h���b�O & �h���b�v���� --//
					if (ImGui::BeginDragDropTarget())
					{// �V�[���K�w�ɑ΂��ẴG���e�B�e�B�h���b�v����
						if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
						{
							//if(registry->HasComponent<component::HierarchyComponent>(selected_entity))
							//{
							//	registry->RemoveComponent<component::HierarchyComponent>(selected_entity);
							//}
							// ��ԏ�̐e�ɐݒ�
							registry->GetComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(registry, {});
						}

						ImGui::EndDragDropTarget();
					}

					{//-- ���ݑI������Ă���G���e�B�e�B�̏��K�w�G���e�B�e�B���擾 --//
						sub_hierarchical_entities.clear();
						for (const auto& hierarchy_comp : registry->GetArray<component::HierarchyComponent>().GetComponents())
						{
							if (HasParentEntity(registry, selected_entity, hierarchy_comp.GetEntity()))
							{
								sub_hierarchical_entities.emplace_back(hierarchy_comp.GetEntity());
							}
						}
					}

					for (auto& [key, value] : registry->GetEntities())
					{
						// �G���e�B�e�B�̊K�w�\��
						// ���߂͈�ԏ�̐e�K�w����n�߂�
						//if (registry->HasComponent<component::HierarchyComponent>(key))
						//	continue;

						if(!registry->GetComponent<component::HierarchyComponent>(key).GetParentEntity().empty())
							continue;;

						//ImGui::Separator();
						is_dragged_entity      = false;
						EntityTree(registry, key, registry->GetName(key));
					}

					//ContextMenu(registry);
					ImGui::TreePop();
				}

			}
		}
		ImGui::End();

		if(ImGui::Begin("Hierarchy Data"))
		{
			if(registry->HasComponent<component::HierarchyComponent>(selected_entity))
			{
				const auto& hierarchy_comp = registry->GetComponent<component::HierarchyComponent>(selected_entity);
				ImGui::Text("Parent Ent : %s", registry->GetName(hierarchy_comp.GetParentEntity()).c_str());
				ImGui::Text("First Child : %s", registry->GetName(hierarchy_comp.GetFirstChild()).c_str());
				ImGui::Text("Back Sibling : %s", registry->GetName(hierarchy_comp.GetBackSibling()).c_str());
				ImGui::Text("Next Sibling : %s", registry->GetName(hierarchy_comp.GetNextSibling()).c_str());
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

			ImGui::Text("Scroll %d", locator::Locator::GetInput()->Mouse().DeltaScrollWheelValue());
		}
		ImGui::End();
	}

	void Hierarchy::ContextMenu(ecs::Registry* const registry)
	{
		if(ImGui::BeginPopup(context_id.c_str()))
		{
			if(!registry->HasComponent<component::HierarchyComponent>(selected_entity))
			{// Hierarchy Component���������Ă��Ȃ��ꍇ -> ��ԏ�̐e�K�w�̏ꍇPrefab�쐬���\��
				if (ImGui::MenuItem("Create Prefab"))
				{
					std::vector<std::string> entities = sub_hierarchical_entities;
					entities.emplace_back(selected_entity);
					CreatePrefab(registry, entities);
				}
			}

			ImGui::MenuItem("Delete");

			ImGui::EndPopup();
		}
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


	void Hierarchy::EntityTree(
		ecs::Registry* const registry,
		const mapping::rename_type::Entity& ent,
		const std::string& entity_name)
	{
		bool is_last		{ true };	// ���G���e�B�e�B�Ɏq���̊K�w�����݂��Ȃ���
		bool is_selectable	{ true };	// �h���b�O & �h���b�v�ł̐e�q�t�����ɑI���\��

		ImGuiTreeNodeFlags node_flg{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
		ecs::ComponentArray<component::HierarchyComponent>& hierarchy_comp_array = registry->GetArray<component::HierarchyComponent>();
		for (const auto& hierarchy_comp : hierarchy_comp_array.GetComponents())
		{
			// ���݂̃G���e�B�e�B�Ɏq�������Ȃ�������
			// �����q�������Ȃ����ImGuiTree��ImGuiTreeNodeFlags_Leaf��ǉ�����
			if (hierarchy_comp.GetParentEntity() != ent)
				continue;

			is_last = false;
		}
		if(HasParentEntity(registry, selected_entity, ent))
		{
			is_selectable = false;
		}

		{// ImGui::TreeNodeEx�ł̃t���O�A�e�L�X�g�F�̐ݒ�
			// ���G���e�B�e�B�̏��K�w�����݂��Ȃ��Ƃ�TreeNode�̖���\�����Ȃ�
			if (is_last)
				node_flg |= ImGuiTreeNodeFlags_Leaf;
			// ���G���e�B�e�B���I������Ă���ꍇTreeNode��I����Ԃɂ���
			if (ent == selected_entity)
				node_flg |= ImGuiTreeNodeFlags_Selected;
			if (!is_selectable && is_dragged_entity)
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
		}
		// �m�[�h�쐬
		const bool is_opened = ImGui::TreeNodeEx(std::string{ ICON_FA_CUBE + entity_name }.c_str(),
												 node_flg);

		if (!is_selectable && is_dragged_entity)
			ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{// �q�G�����L�[�r���[��ł̃A�C�e���I��
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{// ���N���b�N��
				selected_entity = ent;
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{// �E�N���b�N��
				selected_entity = ent;
				ImGui::OpenPopup(context_id.c_str());
			}
		}

		// �h���b�N & �h���b�v����
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
			{// Hierarchy Component�̐e�G���e�B�e�B��ύX
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
				EntityTree(registry, hierarchy_comp.GetEntity(), registry->GetName(hierarchy_comp.GetEntity()));
			}
			ImGui::TreePop();
		}
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
