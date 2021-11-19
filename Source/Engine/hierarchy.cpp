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
	// ImGui::Popup�̃R���e�L�X�g���j���[ID
	const std::string context_id{ "context_menu" };
	// ImGui::Popup�̃h���b�O���̃G���e�B�e�BID
	const std::string dragged_id{ "dragged_entity" };
	// �V�[���Ԃ̃G���e�B�e�B�ړ����̈ꎞ�ۑ��t�@�C���p�X
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
			//-- �E�B���h�E�p�����[�^�̐ݒ� --//
			window_pos		 = DirectX::SimpleMath::Vector2{ ImGui::GetCurrentWindow()->Pos.x,ImGui::GetCurrentWindow()->Pos.y + title_bar_height };
			window_size		 = DirectX::SimpleMath::Vector2{ ImGui::GetContentRegionAvail().x ,ImGui::GetContentRegionAvail().y };
			title_bar_height = ImGui::GetCurrentWindow()->TitleBarHeight();

			//-- �G���e�B�e�B�ǉ����� --//
			if (ImGui::Button(ICON_FA_PLUS, { 30,30 }))
				ImGui::OpenPopup("my_file_popup");
			if (ImGui::BeginPopup("my_file_popup"))
			{
				if (ecs::Registry& registry = *selected_scene.GetRegistry();
					ImGui::MenuItem("Create Empty"))
				{
					// ���ݑI������Ă���V�[���ւ̃G���e�B�e�B�ǉ�
					registry.CreateEntity();
				}
				ImGui::EndPopup();
			}
		}

		//-- �J����Ă���V�[�����̃G���e�B�e�B�\�� & �ҏW --//
		for (const auto& [scene_id, scene] : active_scenes)
		{
			auto& registry = *scene->GetRegistry();
			std::string scene_name = scene->GetSceneName();

			ImGui::PushID(scene_id.c_str());
			if (scene->GetSceneName().empty())
			{
				scene_name = filename_helper::GetNoTitled();
			}

			//-- �u�\���v�A�u��\���v�{�^�� --//
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
			{// �V�[���\�����e�̔��]
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

			//-- �V�[���P�ʂł̃q�G�����L�[�\�� --//
			if (const std::filesystem::path current_scene_path = locator::Locator::GetSystem()->GetCurrentScenePath();
				ImGui::TreeNodeEx(std::string{ ICON_FA_CLOUD_SUN + scene_name }.c_str(),tree_flg))
			{

				if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					// �I������Ă���V�[���̕ύX
					selected_scene_id		= scene_id;
					// �J�ڌ��̃V�[���ύX(�q�G�����L�[�̕ύX�Ɏg�p)
					transition_source_scene = scene.get();
				}

				//-- �V�[���K�w�ւ̃h���b�O & �h���b�v���� --//
				if (ImGui::BeginDragDropTarget())
				{
					// �J�ڐ�̃V�[���X�V
					destination_scene = scene.get();
					// �V�[���K�w�ɑ΂��ẴG���e�B�e�B�h���b�v����
					if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
					{
						if(transition_source_scene == destination_scene)
						{	// ��ԏ�̐e�ɐݒ�
							registry.GetComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(&registry, {});
						}
						else
						{
							// �V�[���Ԃ̈ړ��G���e�B�e�B(�e�̂�)�̃Z�b�g
							scene_transfer_entity	  = selected_entity;
							// �J�ڐ�̐e�G���e�B�e�B�̃Z�b�g
							// -> ��ԏ�̐e�K�w�ɂȂ邽�ߋ�ŗǂ�
							destination_parent_entity = {};
						}
					}

					ImGui::EndDragDropTarget();
				}

				{//-- ���ݑI������Ă���G���e�B�e�B�̏��K�w�G���e�B�e�B���擾 --//
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
					// �G���e�B�e�B�̊K�w�\��
					// ���߂͈�ԏ�̐e�K�w����n�߂�
					if (!registry.GetComponent<component::HierarchyComponent>(key).GetParentEntity().empty())
						continue;;

					is_dragged_entity = false;
					EntityTree(&registry, selected_scene_id, scene_id, key, registry.GetName(key));
				}

				if (!scene_transfer_entity.empty())
				{// �V�[���Ԃ̑J�ڂ��s�������G���e�B�e�B(scene_transfer_entity)
				 // �����݂����ꍇ�̃G���e�B�e�B�폜����
					SceneTransferEntity();
				}

				if(!destroyed_entity.empty())
				{// �폜�������G���e�B�e�B(destroyed_entity)
				 // �����݂����ꍇ�̃G���e�B�e�B�폜����
					DeleteEntity(&registry);
				}

				ImGui::TreePop();
			}

			ImGui::PopStyleColor();

			//-- �h���b�O & �h���b�v�ł̃V�[���ǉ��ǂݍ��� --//
			if (std::filesystem::path selected_asset = ""; // �A�Z�b�g�̃��Z�b�g
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
		//		//-- �V�[���K�w�ւ̃h���b�O & �h���b�v���� --//
		//		if (ImGui::BeginDragDropTarget())
		//		{// �V�[���K�w�ɑ΂��ẴG���e�B�e�B�h���b�v����
		//			if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
		//			{
		//				// ��ԏ�̐e�ɐݒ�
		//				registry->GetComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(registry, {});
		//			}

		//			ImGui::EndDragDropTarget();
		//		}

		//		{//-- ���ݑI������Ă���G���e�B�e�B�̏��K�w�G���e�B�e�B���擾 --//
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
		//			// �G���e�B�e�B�̊K�w�\��
		//			// ���߂͈�ԏ�̐e�K�w����n�߂�
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
			{// Hierarchy Component���������Ă��Ȃ��ꍇ -> ��ԏ�̐e�K�w�̏ꍇPrefab�쐬���\��
				if (ImGui::MenuItem("Create Prefab"))
				{
					std::vector<std::string> entities = sub_hierarchical_entities;
					entities.emplace_back(selected_entity);

					if(auto& selected_hierarchy_comp = registry->GetComponent<component::HierarchyComponent>(selected_entity);
					   !selected_hierarchy_comp.GetParentEntity().empty())
					{// �I�����ꂽ�G���e�B�e�B���e�̃G���e�B�e�B��ێ����Ă����
						// ��ԏ�̊K�w�̐e�ɂ��邽�߂ɐe�K�w���Ȃ���
						selected_hierarchy_comp.SetParentEntity({});
					}

					CreatePrefab(registry, entities);
				}
			}

			if(ImGui::MenuItem("Delete"))
			{// �G���e�B�e�B�̍폜
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
		// �J�ڌ��̃��W�X�g��
		ecs::Registry& transition_source_registry = *transition_source_scene->GetRegistry();
		// �J�ڐ�̃��W�X�g��
		ecs::Registry& destination_registry		  = *destination_scene->GetRegistry();

		// �J�ڂ������G���e�B�e�B(scene_transfer_entity)����ԏ�̐e�K�w�Ɏ����Ă���
		// -> Hierarchy Component��parent�Afirst_child�Anext_sibling�Aback_sibling�̍Đݒ肪�K�v�Ȃ���
		transition_source_registry.GetComponent<component::HierarchyComponent>(scene_transfer_entity).SetParentEntity(&transition_source_registry, {});

		// �ړ��������G���e�B�e�B�S
		std::vector<std::string> entities = sub_hierarchical_entities;
		entities.emplace_back(scene_transfer_entity);

		// �v���n�u�Ƃ��Ĉꎞ�ۑ�����t�@�C���p�X
		const std::string			ent_name  = transition_source_registry.GetName(scene_transfer_entity);
		const std::filesystem::path save_path = transfer_scene_path + "/" +
												ent_name + "/" +
												ent_name + file_path_helper::GetPrefabExtension();

		//-- �J�ڐ�őJ�ڂ��������G���e�B�e�B���̃G���e�B�e�B�쐬 --//
		// �v���n�u�Ƃ��Ĉꎞ�ۑ�
		asset::Prefab transition_ent_prefab{};
		// �J�ڌ��̃��W�X�g���Ƃ��ĕۑ�
		transition_ent_prefab.CreatePrefab(&transition_source_registry, entities, save_path);

		// �J�ڌ��̈ړ��ΏۃG���e�B�e�B�̍폜
		for(const auto& ent : entities)
		{
			transition_source_registry.Destroy(ent);
		}

		// �J�ڐ�̃��W�X�g���ɃC���X�^���X���쐬
		// ent : �J�ڐ�Ńv���n�u����쐬���ꂽ�e�G���e�B�e�B
		const mapping::rename_type::Entity parent_ent = transition_ent_prefab.Instantiate(&destination_registry);

		// �J�ڑΏۂ̃G���e�B�e�B���J�ڐ�Ŏq�G���e�B�e�B�ɂȂ鎞
		if(!destination_parent_entity.empty())
		{
			destination_registry.GetComponent<component::HierarchyComponent>(parent_ent).SetParentEntity(&destination_registry, destination_parent_entity);
			destination_parent_entity.clear();
		}

		// �J�ڑΏۃG���e�B�e�B�̃��Z�b�g
		scene_transfer_entity.clear();
		// �ꎞ�ۑ������t�@�C���̍폜
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
		ImGui::PushID(ent.c_str());
		// �m�[�h�쐬
		const bool is_opened = ImGui::TreeNodeEx(std::string{ ICON_FA_CUBE + entity_name }.c_str(),
												 node_flg);

		if (!is_selectable && is_dragged_entity)
			ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{// �q�G�����L�[�r���[��ł̃A�C�e���I��
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{// ���N���b�N��
				// �I������Ă���G���e�B�e�B�̍X�V
				selected_entity = ent;
				// �I������Ă���V�[���̍X�V
				selected_scene_id = current_scene_id;
				// �J�ڌ��̃V�[���X�V
				transition_source_scene = registry->GetScene();
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{// �E�N���b�N��
				// �I������Ă���G���e�B�e�B�̍X�V
				selected_entity = ent;
				// �|�b�v�A�b�v�����j���[(ContextMenu)���J���O����
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
			// �J�ڐ�̃V�[���X�V
			destination_scene = registry->GetScene();
			// �A�Z�b�g���h���b�v���ꂽ��
			if (ImGui::AcceptDragDropPayload(dragged_id.c_str()))
			{
				// �J�ڌ��̃V�[���ƑJ�ڐ�̃V�[����������
				if(transition_source_scene == destination_scene)
				{// Hierarchy Component�̐e�G���e�B�e�B��ύX
					registry->GetOrEmplaceComponent<component::HierarchyComponent>(selected_entity).SetParentEntity(registry, ent);
				}
				else
				{
					// �V�[���Ԃ̈ړ��G���e�B�e�B(�e�̂�)�̃Z�b�g
					scene_transfer_entity	   = selected_entity;
					// �J�ڐ�̐e�G���e�B�e�B�̃Z�b�g
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
