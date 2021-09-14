#include "model_component.h"

#include "ecs.h"
#include "locator.h"
#include "model_loader.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::ModelComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::ModelComponent)

namespace cumulonimbus::component
{
	template <class Archive>
	void ModelComponent::serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<ComponentBase>(this),

			CEREAL_NVP(model_id),
			CEREAL_NVP(nodes),

			CEREAL_NVP(prev_key_index),
			CEREAL_NVP(current_keyframe),
			CEREAL_NVP(prev_animation_index),
			CEREAL_NVP(current_animation_index),

			CEREAL_NVP(prev_seconds),
			CEREAL_NVP(changer_timer),
			CEREAL_NVP(current_seconds),
			CEREAL_NVP(animation_switch_time),

			CEREAL_NVP(loop_animation),
			CEREAL_NVP(end_animation),

			CEREAL_NVP(prev_animation),
			CEREAL_NVP(anim_states),
			CEREAL_NVP(graphics_state)
		);
	}

	ModelComponent::ModelComponent(
		ecs::Registry* registry, const mapping::rename_type::Entity ent,
		const mapping::rename_type::UUID& model_id)
	{
		Initialize(registry, ent, model_id);
	}

	void ModelComponent::NewFrame(float delta_time)
	{
	}

	void ModelComponent::Update(float delta_time)
	{
		const auto& world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorld4x4();

		CalculateLocalTransform();
		CalculateWorldTransform(world_transform);

		UpdateAnimState(delta_time);
	}

	void ModelComponent::RenderImGui()
	{
	}

	bool ModelComponent::IsPlayAnimation() const
	{
		const asset::ModelData::Animation& animation
			= locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData().animations.at(current_animation_index);

		return current_keyframe < (animation.num_key_frame - 1);
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

		if (anim_states.GetState() == FbxAnimationState::Blending ||
			anim_states.GetState() == FbxAnimationState::Switch)
		{
			prev_key_index = 0;
		}

		anim_states.SetState(FbxAnimationState::Switch);
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

	void ModelComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
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

	void ModelComponent::Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent,
		const mapping::rename_type::UUID& model_id)
	{
		InitializeParameter();
		this->model_id = model_id;
		asset::Model& model =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id);

		// ノード
		const std::vector<asset::ModelData::Node>& res_nodes = model.GetModelData().nodes;
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

		anim_states.AddState(FbxAnimationState::Switch, [ent, registry](const float dt) { registry->GetComponent<ModelComponent>(ent).BlendNextAnimation(dt); });
		anim_states.AddState(FbxAnimationState::Update, [ent, registry](const float dt) { registry->GetComponent<ModelComponent>(ent).UpdateAnimation(dt); });

		// アニメーションの初期値をセット
		SwitchAnimation(0, false, 0.0f);
	}

	int ModelComponent::CalcPrevKeyframe() const
	{
		const asset::ModelData& model_data =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData();

		const asset::ModelData::Animation& prev_animation = model_data.animations.at(prev_animation_index);

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
			anim_states.SetState(FbxAnimationState::Update);
			return;
		}

		if (anim_states.GetInitialize())
		{
			// 前のアニメーションの切り替わった時点のキーフレームを算出
			const size_t keyframe = model_data.animations.at(prev_animation_index).num_key_frame;
			if (prev_key_index >= keyframe)
			{
				prev_key_index = keyframe;
			}
			prev_animation = model_data.animations.at(prev_animation_index);
		}

		float rate = changer_timer / animation_switch_time;

		const asset::ModelData::Keyframe& prev_keyframe = prev_animation.keyframes.at(prev_key_index);
		const asset::ModelData::Keyframe& current_keyframe = model_data.animations.at(current_animation_index).keyframes.at(0);

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
		if (animation_switch_time <= changer_timer)
		{
			changer_timer = 0;
			anim_states.SetState(FbxAnimationState::Update);
		}
	}

	void ModelComponent::UpdateAnimation(float dt)
	{
		if (current_animation_index < 0)
			return;

		const asset::ModelData& model_data =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_id).GetModelData();

		if (model_data.animations.empty())
			return;

		const asset::ModelData::Animation& animation = model_data.animations.at(current_animation_index);

		const std::vector<asset::ModelData::Keyframe>& keyframes = animation.keyframes;

		for (size_t key_index = 0; key_index < animation.num_key_frame; ++key_index)
		{
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
