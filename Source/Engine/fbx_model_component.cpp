#include "fbx_model_component.h"

#include <algorithm>
#include <iterator>
#include <vector>

#include "cereal_helper.h"
#include "collision_component.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "fbx_model_resource.h"
#include "locator.h"
#include "material3d_manager.h"
#include "mesh_object.h"
#include "pbr_material.h"

#include "shader_manager.h"
#include "transform_component.h"

namespace cumulonimbus::component
{
	FbxModelComponent::FbxModelComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const std::shared_ptr<FbxModelResource>& resource)
		:ComponentBase{ registry , ent }
	{
		this->resource = resource;

		// ���f���̃}�e���A�������쐬
		for (int i = 0; i < resource->GetModelData().materials.size(); ++i)
		{
			pbr_materials.emplace_back(std::make_shared<shader_asset::PBRMaterial>());
			materials_manager.emplace_back(std::make_shared<shader_asset::Material3DManager>());
			// �f�t�H���g��Diffuse�V�F�[�_�[���Z�b�g
			materials_manager.back()->SetShaderAsset(mapping::shader_assets::ShaderAsset3D::Diffuse);
		}

		registry->AddComponent<component::MeshObjectComponent>(ent);

		// �m�[�h
		const std::vector<ModelData::Node>& res_nodes = resource->GetModelData().nodes;

		nodes.resize(res_nodes.size());
		for (size_t node_index = 0; node_index < nodes.size(); ++node_index)
		{// ModelResource::Node �� Model::Node�����ѕt����
			auto&& src = res_nodes.at(node_index);
			auto&& dst = nodes.at(node_index);

			dst.name	  = src.name;
			dst.parent_index = src.parent_index >= 0 ? src.parent_index : -1;
			//dst.parent		 = src.parent_index >= 0 ? &nodes.at(src.parent_index) : nullptr;
			dst.scale	  = src.scale;
			dst.rotate	  = src.rotate;
			dst.translate = src.translate;
		}

		anim_states.AddState(FbxAnimationState::Switch, [ent, registry](const float dt) { registry->GetComponent<FbxModelComponent>(ent).BlendNextAnimation(dt); });
		anim_states.AddState(FbxAnimationState::Update, [ent, registry](const float dt) { registry->GetComponent<FbxModelComponent>(ent).UpdateAnimation(dt); });

		// �A�j���[�V�����̏����l���Z�b�g
		SwitchAnimation(0, false, 0.0f);
	}

	void FbxModelComponent::NewFrame(const float delta_time)
	{

	}

	void FbxModelComponent::Update(const float delta_time)
	{
		const auto& world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorld4x4();

		CalculateLocalTransform();
		CalculateWorldTransform(world_transform);

		UpdateAnimState(delta_time);
	}

	void FbxModelComponent::RenderImGui()
	{
		if (ImGui::TreeNode("FBX Model Paramater"))
		{
			// �v�f�̈ʒu�擾
			// std::distance https://cpprefjp.github.io/reference/iterator/distance.html
			auto v = locator::Locator::GetResourceManager()->FbxModelNames();
			auto f = std::find(v.begin(), v.end(), resource->GetModelName());
			int item_current_idx = static_cast<int>(std::distance(v.begin(), f));
			const std::string combo_label = locator::Locator::GetResourceManager()->FbxModelNames().at(item_current_idx).c_str();
			if (ImGui::BeginCombo("Resources", combo_label.c_str()))
			{
				for (int n = 0; n < locator::Locator::GetResourceManager()->FbxModelNames().size(); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(locator::Locator::GetResourceManager()->FbxModelNames().at(n).c_str(), is_selected))
					{
						if (item_current_idx != n)
						{
							item_current_idx = n;
							Initialize(locator::Locator::GetResourceManager()->FbxModelResouece(locator::Locator::GetResourceManager()->FbxModelNames().at(n)));
						}
					}
					//Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			int no = 0;
			for (const auto& material : resource->GetModelData().materials)
			{
				std::string str = "Color" + no;
				ImGui::ColorEdit4(str.c_str(), (float*)&material.color);
				++no;
			}

			// ���b�V���ɓK������V�F�[�_�[�A�V�F�[�_�[�̃p�����[�^�A�e�N�X�`���̌���
			// (���f���̃}�e���A���̌���)
			if (ImGui::CollapsingHeader("Meshes"))
			{
				for (auto& mesh : resource->GetModelData().meshes)
				{// ���f���������b�V������
					if (ImGui::TreeNode(mesh.mesh_name.c_str()))
					{
						for (int material_index = 0; material_index < mesh.material_count; ++material_index)
						{// ���̃��b�V�������}�e���A������
							// Shader�̎�ނ�ς��邽�߂�ImGui
							ImGui::PushID(material_index);
							materials_manager.at(material_index)->RenderImGuiComboShader();

							ImGui::Spacing();
							ImGui::Text("Albedo Texture");
							helper::imgui::Image(resource->GetModelData().materials.at(material_index).shader_resource_view.Get());
							ImGui::SameLine();

							auto* my_texture   = resource->GetModelData().materials.at(material_index).shader_resource_view.Get();
							auto  tex_filename = resource->GetModelData().materials.at(material_index).texture_filename;
							const std::string combo_label = "Textures";
							if(ImGui::BeginCombo(combo_label.c_str(),tex_filename.c_str()))
							{
								for (const auto& tex : locator::Locator::GetTextureResourceManager()->GetTextureResources())
								{
									helper::imgui::Image(tex.second->GetTextureData()->texture_view.Get(), { 50,50 });
									ImGui::SameLine();
									const bool is_selected = (my_texture == tex.second->GetTextureData()->texture_view.Get());
									if (ImGui::Selectable(tex.first.c_str(), is_selected,0,{ 500,50}))
									{
										resource->SetMaterialTexture(material_index, tex.second->GetTextureData()->texture_view.Get());
										resource->SetMaterialFilename(material_index, tex.first);
									}
									if (is_selected)
									{
										ImGui::SetItemDefaultFocus();
									}
								}

								ImGui::EndCombo();
							}

							// ���ݓK������Ă���Shader�̃p�����[�^��ҏW���邽�߂�ImGui
							materials_manager.at(material_index)->RenderImGuiShaderParameter();
							ImGui::PopID();
						}
						ImGui::TreePop();
					}
				}
			}

			ImGui::Text("Current Keyframe : %d", current_keyframe);

			ImGui::TreePop();
		}
	}

	const shader_asset::Material3DManager* FbxModelComponent::GetMaterialsManager(uint material_index) const
	{
		return materials_manager.at(material_index).get();
	}

	const shader_asset::PBRMaterial* FbxModelComponent::GetPBRMaterial(uint material_index) const
	{
		return pbr_materials.at(material_index).get();
	}

	void FbxModelComponent::BindPBRMaterialCBuff(uint material_index) const
	{
		pbr_materials.at(material_index)->BindCBuffer();
	}

	void FbxModelComponent::UnbindPBRMaterialCBuff(uint material_index) const
	{
		pbr_materials.at(material_index)->UnbindCBuffer();
	}

	void FbxModelComponent::BindPBRMaterialTexture(uint material_index) const
	{
		pbr_materials.at(material_index)->BindTexture();
	}

	void FbxModelComponent::UnbindPBRMaterialTexture(uint material_index) const
	{
		pbr_materials.at(material_index)->UnbindTexture();
	}

	void FbxModelComponent::BindPBRMaterialCBuffAndTexture(uint material_index) const
	{
		pbr_materials.at(material_index)->BindCBufferAndTexture();
	}

	void FbxModelComponent::UnbindPBRMaterialCBuffAndTexture(uint material_index) const
	{
		pbr_materials.at(material_index)->UnbindCBufferAndTexture();
	}

	void FbxModelComponent::Initialize(const std::shared_ptr<FbxModelResource>& resource)
	{
		this->resource = resource;

		// �m�[�h
		const std::vector<ModelData::Node>& res_nodes = resource->GetModelData().nodes;

		nodes.resize(res_nodes.size());
		for (size_t node_index = 0; node_index < nodes.size(); ++node_index)
		{// ModelResource::Node �� Model::Node�����ѕt����
			auto&& src = res_nodes.at(node_index);
			auto&& dst = nodes.at(node_index);

			dst.name			= src.name;
			dst.parent_index	= src.parent_index >= 0 ? src.parent_index : -1;
			//dst.parent = src.parent_index >= 0 ? &nodes.at(src.parent_index) : nullptr;
			dst.scale			= src.scale;
			dst.rotate			= src.rotate;
			dst.translate		= src.translate;
		}

		{// �����o�ϐ��̏�����
			current_animation_index = -1;
			prev_animation_index	= -1;
			current_seconds			= 0;
			prev_seconds			= 0;
			current_keyframe		= 0;

			prev_key_index = 0;
		}

		//anim_states.AddState(FbxAnimationState::Switch, [this](const float delta_time) {BlendNextAnimation(delta_time); });
		//anim_states.AddState(FbxAnimationState::Update, [this](const float delta_time) {UpdateAnimation(delta_time); });

		// �A�j���[�V�����̏����l���Z�b�g
		SwitchAnimation(0, false, 0.0f);
	}

	// �A�j���[�V�����Đ�
	void FbxModelComponent::SwitchAnimation(int animation_index, bool loop, float switch_time)
	{
		prev_animation_index	= current_animation_index;
		current_animation_index = animation_index;
		loop_animation			= loop;
		end_animation			= false;
		prev_seconds			= current_seconds;
		current_seconds			= 0.0f;
		animation_switch_time	= switch_time;
		current_keyframe		= 0;

		if (anim_states.GetOldState() == FbxAnimationState::Blending ||
			anim_states.GetOldState() == FbxAnimationState::Switch)
		{
			prev_key_index = 0;
		}

		anim_states.SetState(FbxAnimationState::Switch);
	}

	void FbxModelComponent::BlendNextAnimation(const float elapsed_time)
	{
		if (prev_animation_index < 0)
		{
			anim_states.SetState(FbxAnimationState::Update);
			return;
		}

		if (anim_states.GetInitialize())
		{
			// �O�̃A�j���[�V�����̐؂�ւ�������_�̃L�[�t���[�����Z�o
			prev_animation = resource->GetModelData().animations.at(prev_animation_index);
		}

		float rate = changer_timer / animation_switch_time;

		const ModelData::Keyframe& prev_keyframe = prev_animation.keyframes.at(prev_key_index);
		const ModelData::Keyframe& current_keyframe = resource->GetModelData().animations.at(current_animation_index).keyframes.at(0);

		int nodeCount = static_cast<int>(nodes.size());
		for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
			const ModelData::NodeKeyData& key0 = prev_keyframe.node_keys.at(nodeIndex);
			const ModelData::NodeKeyData& key1 = current_keyframe.node_keys.at(nodeIndex);

			Node& node = nodes[nodeIndex];

			DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
			DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
			DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
			DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
			DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
			DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

			DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
			DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
			DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

			DirectX::XMStoreFloat3(&node.scale, s);
			DirectX::XMStoreFloat4(&node.rotate, r);
			DirectX::XMStoreFloat3(&node.translate, t);
		}

		changer_timer += elapsed_time;
		if (animation_switch_time <= changer_timer)
		{
			changer_timer = 0;
			anim_states.SetState(FbxAnimationState::Update);
		}
	}

	// �A�j���[�V�����v�Z
	void FbxModelComponent::UpdateAnimation(float elapsedTime)
	{
		if (current_animation_index < 0)
		{
			return;
		}

		if (resource->GetModelData().animations.empty())
		{
			return;
		}

		const ModelData::Animation& animation = resource->GetModelData().animations.at(current_animation_index);

		const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;

		size_t key_count = static_cast<size_t>(keyframes.size());
		for (size_t key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
			const ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
			const ModelData::Keyframe& keyframe1 = keyframes.at(key_index + static_cast<size_t>(1));
			if (current_seconds >= keyframe0.seconds && current_seconds < keyframe1.seconds)
			{
				prev_key_index = current_keyframe;
				current_keyframe = static_cast<int>(key_index);
				float rate = (current_seconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

				assert(nodes.size() == keyframe0.node_keys.size());
				assert(nodes.size() == keyframe1.node_keys.size());
				int nodeCount = static_cast<int>(nodes.size());
				for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
				{
					// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
					const ModelData::NodeKeyData& key0 = keyframe0.node_keys.at(nodeIndex);
					const ModelData::NodeKeyData& key1 = keyframe1.node_keys.at(nodeIndex);

					Node& node = nodes[nodeIndex];

					DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
					DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
					DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

					DirectX::XMStoreFloat3(&node.scale, s);
					DirectX::XMStoreFloat4(&node.rotate, r);
					DirectX::XMStoreFloat3(&node.translate, t);
				}
				//prev_key_index = key_index;
				break;
			}
		}

		// �ŏI�t���[������
		if (end_animation)
		{
			end_animation = false;
			current_animation_index = -1;
			prev_key_index = 0;
			return;
		}

		// ���Ԍo��
		current_seconds += elapsedTime;
		if (current_seconds >= animation.seconds_length)
		{
			if (loop_animation)
			{
				current_seconds -= animation.seconds_length;
			}
			else
			{
				current_seconds = animation.seconds_length;
				end_animation = true;
			}
		}
	}

	// ���[�J���ϊ��s��v�Z
	void FbxModelComponent::CalculateLocalTransform()
	{
		for (Node& node : nodes)
		{
			DirectX::XMMATRIX scale, rotate, translate;
			scale		= DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
			rotate		= DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotate.x, node.rotate.y, node.rotate.z, node.rotate.w));
			translate	= DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);

			DirectX::XMStoreFloat4x4(&node.local_transform, scale * rotate * translate);
		}
	}

	// ���[���h�ϊ��s��v�Z
	void FbxModelComponent::CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform)
	{
		for (Node& node : nodes)
		{
			const DirectX::XMMATRIX local_transform_matrix = DirectX::XMLoadFloat4x4(&node.local_transform);
			const DirectX::XMMATRIX world_transform_matrix = DirectX::XMLoadFloat4x4(&world_transform);
			if (node.parent_index > 0)
			{
				const DirectX::XMMATRIX parent_transform = DirectX::XMLoadFloat4x4(&nodes.at(node.parent_index).world_transform);
				DirectX::XMStoreFloat4x4(&node.world_transform, local_transform_matrix * parent_transform);
			}
			else
			{
				DirectX::XMStoreFloat4x4(&node.world_transform, local_transform_matrix * world_transform_matrix);
			}
		}
	}

	int FbxModelComponent::CalcPrevKeyframe(int key_frame)
	{
		const ModelData::Animation& prev_animation = resource->GetModelData().animations.at(prev_animation_index);

		const std::vector<ModelData::Keyframe>& keyframes = prev_animation.keyframes;

		size_t key_count = static_cast<size_t>(keyframes.size());
		for (size_t key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
			const ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
			const ModelData::Keyframe& keyframe1 = keyframes.at(key_index + static_cast<size_t>(1));
			if (current_seconds >= keyframe0.seconds && current_seconds < keyframe1.seconds)
			{
				return static_cast<int>(key_index);
			}
		}

		assert(!"Not found key frame");

		return -1;
	}

	const DirectX::SimpleMath::Matrix& FbxModelComponent::GetNodeMatrix(const char* node_name)
	{
		for (const auto& node : nodes)
		{
			if (node_name == node.name)
			{
				return node.world_transform;
			}
		}

		assert(!"Not found node name");

		return DirectX::SimpleMath::Matrix::Identity;
	}

	const DirectX::SimpleMath::Matrix& FbxModelComponent::GetNodeParentMatrix(const char* node_name)
	{
		for (const auto& node : nodes)
		{
			if (node_name == node.name)
			{
				return node.local_transform;
			}
		}

		assert(!"Not found node name");

		return DirectX::SimpleMath::Matrix::Identity;
	}


	void FbxModelComponent::Save(const std::string& file_path)
	{

	}


	void FbxModelComponent::Load(const std::string& file_path_and_name)
	{

	}

} // cumulonimbus::component
