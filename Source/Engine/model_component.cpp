#include "model_component.h"

#include <filesystem>

#include "ecs.h"
#include "cum_imgui_helper.h"
#include "locator.h"
// loaders and  assets
#include "material_loader.h"
#include "model_loader.h"
// components
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::ModelComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::ModelComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::ModelComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void ModelComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),

			CEREAL_NVP(model_id),
			CEREAL_NVP(material_ids),
			CEREAL_NVP(nodes),
			CEREAL_NVP(graphics_state),

			CEREAL_NVP(prev_key_index),
			CEREAL_NVP(current_keyframe),
			CEREAL_NVP(prev_animation_index),
			CEREAL_NVP(current_animation_index),

			CEREAL_NVP(prev_seconds),
			CEREAL_NVP(changer_timer),
			CEREAL_NVP(current_seconds),
			CEREAL_NVP(animation_switch_time),

			CEREAL_NVP(is_visible),
			CEREAL_NVP(loop_animation),
			CEREAL_NVP(end_animation),

			CEREAL_NVP(prev_animation),
			CEREAL_NVP(anim_states)
		);
	}

	template <class Archive>
	void ModelComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),

			CEREAL_NVP(model_id),
			CEREAL_NVP(material_ids),
			CEREAL_NVP(nodes),
			CEREAL_NVP(graphics_state),

			CEREAL_NVP(prev_key_index),
			CEREAL_NVP(current_keyframe),
			CEREAL_NVP(prev_animation_index),
			CEREAL_NVP(current_animation_index),

			CEREAL_NVP(prev_seconds),
			CEREAL_NVP(changer_timer),
			CEREAL_NVP(current_seconds),
			CEREAL_NVP(animation_switch_time),

			CEREAL_NVP(is_visible),
			CEREAL_NVP(loop_animation),
			CEREAL_NVP(end_animation),

			CEREAL_NVP(prev_animation),
			CEREAL_NVP(anim_states)
		);

	}

	ModelComponent::ModelComponent(
		ecs::Registry* registry, const mapping::rename_type::Entity ent,
		const mapping::rename_type::UUID& model_id)
		:ComponentBase{ registry,ent }
	{
		Initialize(registry, ent, model_id);
	}

	ModelComponent::ModelComponent(
		ecs::Registry* registry, const mapping::rename_type::Entity ent,
		const ModelComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void ModelComponent::CommonUpdate(const float dt)
	{
		UpdateAnimState(dt);
	}

	void ModelComponent::PostCommonUpdate(float dt)
	{
		const auto& world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorldMatrix();

		CalculateLocalTransform();
		CalculateWorldTransform(world_transform);
	}

	void ModelComponent::SceneUpdate(const float dt)
	{

	}

	void ModelComponent::PreGameUpdate(const float dt)
	{
	}

	void ModelComponent::GameUpdate(const float dt)
	{

	}

	void ModelComponent::RenderImGui()
	{
		if (const ImGuiTreeNodeFlags tree_flg{ ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
			ImGui::CollapsingHeader("Model Component", tree_flg))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& asset_sheet_manager = asset_manager.GetAssetSheetManager();
			auto* model_loader = locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>();
			std::filesystem::path current_path{};
			std::string current_item{};
			if (model_loader->HasModel(model_id))
			{
				current_path = asset_sheet_manager.GetAssetFilename<asset::Model>(model_id);
				current_item = current_path.filename().string();
			}
			std::vector<std::filesystem::path> items_path{};
			std::vector<std::string> items{};
			items_path.reserve(asset_sheet_manager.GetSheet<asset::Model>().sheet.size());
			items.reserve(asset_sheet_manager.GetSheet<asset::Model>().sheet.size());
			for (const auto& value : asset_sheet_manager.GetSheet<asset::Model>().sheet | std::views::values)
			{
				items_path.emplace_back(value);
				items.emplace_back(items_path.back().filename().string());
			}

			if (helper::imgui::Combo("Model", current_item, items))
			{// ImGui::Combo内でアイテム選択時
				for (const auto& [key, value] : asset_sheet_manager.GetSheet<asset::Model>().sheet)
				{
					if (current_item != std::filesystem::path{ value }.filename().string())
						continue;
					if (model_id != key)
					{
						/*
						 * 選択されたモデルが現在保持しているモデルと違う場合
						 * model_idを更新 & ModelComponentの初期化処理
						 */
						Initialize(GetRegistry(), GetEntity(), key);
					}
				}
			}

			// モデルがなければ処理を抜ける
			if (!model_loader->HasModel(model_id))
				return;

			// マテリアル編集処理
			if (ImGui::TreeNodeEx("Materials", tree_flg))
			{
				for (auto& mesh : asset_manager.GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData().GetMeshes())
				{
					if (ImGui::TreeNodeEx(mesh.mesh_name.c_str(), tree_flg))
					{// メッシュ毎のマテリアル
						for (const asset::ModelData::Subset& subset : mesh.subsets)
						{// メッシュが持つマテリアル郡
							if (const auto& material_id = GetMaterialID(subset.material_index);
								ImGui::TreeNodeEx(asset_manager.GetLoader<asset::MaterialLoader>()->GetMaterialName(material_id).c_str(), tree_flg))
							{
								// マテリアルの編集処理実装部
								locator::Locator::GetAssetManager()->GetLoader<asset::MaterialLoader>()->GetMaterial(material_id).RenderImGui();

								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			// アニメーション編集処理
			if (ImGui::TreeNodeEx("Animations", tree_flg))
			{
				if (ImGui::Button("Save Animation Data"))
				{
					std::filesystem::path save_path = asset_sheet_manager.GetAssetFilename<asset::Model>(model_id);
					model_loader->GetModel(model_id).Save(save_path.replace_extension());
				}

				ImGui::Text("Current Keyframe : %d", current_keyframe);
				ImGui::Text("Current Anim Index %d", current_animation_index);

				for (int animation_index = 0; animation_index < model_loader->GetModel(model_id).GetModelData().GetAnimations().size(); ++animation_index)
				{
					ImGui::PushID(animation_index);
					asset::ModelData& model_data = model_loader->GetModel(model_id).GetModelData();
					auto& animation = model_data.GetAnimations().at(animation_index);
					if (ImGui::TreeNode(animation.animation_name.c_str()))
					{
						ImGui::Text("Seconds Length : %f", animation.seconds_length);
						ImGui::Text("Sampling Rate : %f" , animation.sampling_rate);
						ImGui::Text("Sampling Time : %f" , animation.sampling_time);

						ImGui::InputInt("End Frame", &animation.num_key_frame);
						ImGui::DragFloat("Playback Speed", &animation.playback_speed, 0.01f, 0.01f, 10.0f);
						if (ImGui::Button("Switch Animation"))
						{
							SwitchAnimation(animation_index, true);
						}
						animation.default_key_frame = animation.num_key_frame;
						model_data.SetAnimationKeyFrame(animation_index, animation.num_key_frame);

						ImGui::TreePop();
					}

					ImGui::PopID();
				}
				ImGui::TreePop();
			}

			graphics_state.RenderImGui();
		}
	}

	void ModelComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
		// StateMachineの再登録
		InitializeAnimState(registry, GetEntity());
	}

	void ModelComponent::OnDeserialize(ecs::Registry* registry, const std::map<mapping::rename_type::Entity, mapping::rename_type::Entity>& connector)
	{
		SetRegistry(registry);
		// StateMachineの再登録
		InitializeAnimState(registry, GetEntity());
	}


	bool ModelComponent::IsPlayAnimation() const
	{
		const asset::ModelData::Animation& animation
			= locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData().GetAnimations().at(current_animation_index);

		return current_keyframe < (animation.num_key_frame - 1);
	}

	int ModelComponent::GetNodeIndexFromName(const std::string& node_name) const
	{
		for(u_int node_index = 0; node_index < nodes.size();++node_index)
		{
			if(nodes.at(node_index).name.compare(node_name) == 0)
			{// ノード名が見つかったので現在のインデックス番号を返す
				return node_index;
			}
		}

		// ノード名が見つからなかったので-1(インデックスで存在しない番号)を返す
		return -1;
	}

	bool ModelComponent::HasNodeFromName(const std::string& node_name) const
	{
		for (u_int node_index = 0; node_index < nodes.size(); ++node_index)
		{
			if (nodes.at(node_index).name.compare(node_name) == 0)
			{// ノード名が見つかったので現在のインデックス番号を返す
				return true;
			}
		}

		// ノード名が見つからなかったので-1(インデックスで存在しない番号)を返す
		return false;
	}

	void ModelComponent::SwitchAnimation(
		const int animation_index,
		const bool loop, const float switch_time)
	{
		loop_animation			= loop;
		end_animation			= false;
		current_keyframe		= 0;
		prev_animation_index	= current_animation_index;
		current_animation_index = animation_index;
		prev_seconds			= current_seconds;
		current_seconds			= 0.0f;
		animation_switch_time	= switch_time;

		if (anim_states.GetState() == AnimationState::Blending ||
			anim_states.GetState() == AnimationState::Switch)
		{
			prev_key_index = 0;
		}

		anim_states.SetState(AnimationState::Switch);
	}

	const mapping::rename_type::UUID& ModelComponent::GetMaterialID(const u_int material_index) const
	{
		if (material_index >= material_ids.size())
			assert(!"You are trying to access more than the number of elements(ModelComponent::GetMaterialID)");
		return material_ids.at(material_index);
	}

	const DirectX::SimpleMath::Matrix& ModelComponent::GetNodeMatrix(const char* node_name)
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

	const DirectX::SimpleMath::Matrix& ModelComponent::GetNodeParentMatrix(const char* node_name)
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

	void ModelComponent::UpdateAnimState(const float delta_time)
	{
		if (anim_states.GetNumState() <= 0)
			return;
		anim_states.Update(delta_time);
	}

	void ModelComponent::InitializeParameter()
	{
		model_id = {};
		nodes.clear();

		prev_key_index			= 0;
		current_keyframe		= 0;
		prev_animation_index	= -1;
		current_animation_index = -1;

		prev_seconds		  = 0.0f;
		changer_timer		  = 0;
		current_seconds		  = 0.0f;
		animation_switch_time = 0;

		end_animation	= false;
		loop_animation	= false;
	}

	void ModelComponent::InitializeAnimState(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
		anim_states.ClearState();
		anim_states.AddState(AnimationState::Switch, [ent, registry](const float dt) { registry->GetComponent<ModelComponent>(ent).BlendNextAnimation(dt); });
		anim_states.AddState(AnimationState::Update, [ent, registry](const float dt) { registry->GetComponent<ModelComponent>(ent).UpdateAnimation(dt); });
		// アニメーションの初期値をセット
		SwitchAnimation(0, false, 0.0f);
	}

	void ModelComponent::Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent,
		const mapping::rename_type::UUID& model_id)
	{
		InitializeParameter();
		this->model_id = model_id;
		asset::Model& model =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id);

		// ノード
		const std::vector<asset::ModelData::Node>& res_nodes = model.GetModelData().GetNodes();
		nodes.resize(res_nodes.size());
		for (size_t node_index = 0; node_index < nodes.size(); ++node_index)
		{// ModelResource::Node と Model::Nodeを結び付ける
			auto&& src = res_nodes.at(node_index);
			auto&& dst = nodes.at(node_index);

			dst.name = src.name;
			dst.parent_index = src.parent_index >= 0 ? src.parent_index : -1;
			dst.scale = src.scale;
			dst.rotate = src.rotate;
			dst.translate = src.translate;
		}

		// マテリアルIDのコピー
		const auto& mat_vec = model.GetModelData().GetMaterialsID();
		material_ids.clear();
		std::copy(mat_vec.begin(), mat_vec.end(), std::back_inserter(material_ids));

		InitializeAnimState(registry, ent);
	}

	int ModelComponent::CalcPrevKeyframe() const
	{
		const asset::ModelData& model_data =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData();

		const asset::ModelData::Animation& prev_animation = model_data.GetAnimations().at(prev_animation_index);

		const std::vector<asset::ModelData::Keyframe>& keyframes = prev_animation.keyframes;

		size_t key_count = static_cast<size_t>(keyframes.size());
		for (size_t key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const asset::ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
			const asset::ModelData::Keyframe& keyframe1 = keyframes.at(key_index + static_cast<size_t>(1));
			if (current_seconds >= keyframe0.seconds && current_seconds < keyframe1.seconds)
			{
				return static_cast<int>(key_index);
			}
		}

		assert(!"Not found key frame");

		return -1;
	}

	void ModelComponent::CalculateLocalTransform()
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

	void ModelComponent::CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform)
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

	void ModelComponent::BlendNextAnimation(const float dt)
	{
		const asset::ModelData& model_data =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData();

		if (prev_animation_index < 0)
		{
			anim_states.SetState(AnimationState::Update);
			return;
		}

		if(prev_animation_index == current_animation_index)
		{
			anim_states.SetState(AnimationState::Update);
			return;
		}

		if(animation_switch_time <= 0.0f)
		{
			anim_states.SetState(AnimationState::Update);
			return;
		}

		if (anim_states.GetInitialize())
		{
			// 前のアニメーションの切り替わった時点のキーフレームを算出
			const size_t keyframe = model_data.GetAnimations().at(prev_animation_index).num_key_frame;
			if (prev_key_index >= static_cast<int>(keyframe))
			{
				prev_key_index = static_cast<int>(keyframe);
			}
			prev_animation = model_data.GetAnimations().at(prev_animation_index);
		}

		float rate = changer_timer / animation_switch_time;

		const asset::ModelData::Keyframe& prev_keyframe = prev_animation.keyframes.at(prev_key_index);
		const asset::ModelData::Keyframe& current_keyframe = model_data.GetAnimations().at(current_animation_index).keyframes.at(0);

		const int node_count = static_cast<int>(nodes.size());
		for (int node_index = 0; node_index < node_count; ++node_index)
		{
			// ２つのキーフレーム間の補完計算
			const asset::ModelData::NodeKeyData& key0 = prev_keyframe.node_keys.at(node_index);
			const asset::ModelData::NodeKeyData& key1 = current_keyframe.node_keys.at(node_index);

			Node& node = nodes[node_index];

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

		changer_timer += dt;
		GetRegistry()->GetComponent<TransformComponent>(GetEntity()).SetDirtyFlg(TransformComponent::Animation_Changed);
		if (animation_switch_time <= changer_timer)
		{
			changer_timer = 0;
			anim_states.SetState(AnimationState::Update);
		}
	}

	void ModelComponent::UpdateAnimation(float dt)
	{
		if (current_animation_index < 0)
			return;

		const asset::ModelData& model_data =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData();

		if (model_data.GetAnimations().empty())
			return;

		const asset::ModelData::Animation& animation = model_data.GetAnimations().at(current_animation_index);

		const std::vector<asset::ModelData::Keyframe>& keyframes = animation.keyframes;

		for (size_t key = 0; key < animation.num_key_frame; ++key)
		{
			const size_t key_index = (key < keyframes.size() - 1 ? key : keyframes.size() - 2);
			// 現在の時間がどのキーフレームの間にいるか判定する
			const asset::ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
			const asset::ModelData::Keyframe& keyframe1 = keyframes.at(key_index + static_cast<size_t>(1));

			if (current_seconds >= keyframe0.seconds && current_seconds < keyframe1.seconds)
			{
				prev_key_index	 = current_keyframe;
				current_keyframe = static_cast<int>(key_index);
				float rate = (current_seconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

				assert(nodes.size() == keyframe0.node_keys.size());
				assert(nodes.size() == keyframe1.node_keys.size());
				const int node_count = static_cast<int>(nodes.size());
				for (int node_index = 0; node_index < node_count; ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const asset::ModelData::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
					const asset::ModelData::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

					Node& node = nodes[node_index];

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

		GetRegistry()->GetComponent<TransformComponent>(GetEntity()).SetDirtyFlg(TransformComponent::Animation_Changed);
		// 最終フレーム処理
		if (end_animation)
		{
			end_animation = false;
			//current_animation_index = -1;
			prev_key_index = animation.num_key_frame - 1;
			return;
		}

		// 時間経過
		current_seconds += dt * animation.playback_speed;
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
} // cumulonimbus::component
