#include "fbx_model_component.h"

#include <algorithm>
#include <iterator>
#include <vector>

#include "ecs.h"
#include "shader_manager.h"
#include "fbx_model_resource.h"
#include "collision_component.h"
#include "transform.h"
#include "transform_component.h"
#include "locator.h"


void FbxModelComponent::NewFrame(const float delta_time)
{

}

void FbxModelComponent::Update(const float delta_time)
{
	const auto& world_transform = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();

	CalculateLocalTransform();
	CalculateWorldTransform(world_transform);

	anim_states;

	UpdateAnimState(delta_time);
}

void FbxModelComponent::RenderImGui()
{
	if (ImGui::TreeNode("FBX Model Paramater"))
	{
		// 要素の位置取得
		// std::distance https://cpprefjp.github.io/reference/iterator/distance.html
		auto v = Locator::GetResourceManager().FbxModelNames();
		auto f = std::find(v.begin(), v.end(), resource->GetModelName());
		int item_current_idx = static_cast<int>(std::distance(v.begin(), f));
		const std::string combo_label = Locator::GetResourceManager().FbxModelNames().at(item_current_idx).c_str();
		if (ImGui::BeginCombo("Resources", combo_label.c_str()))
		{
			for (int n = 0; n < Locator::GetResourceManager().FbxModelNames().size(); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(Locator::GetResourceManager().FbxModelNames().at(n).c_str(), is_selected))
				{
					if (item_current_idx != n)
					{
						item_current_idx = n;
						Initialize(Locator::GetResourceManager().FbxModelResouece(Locator::GetResourceManager().FbxModelNames().at(n)));
					}
				}
				 //Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		int no = 0;
		for(auto& material : resource->GetModelData().materials)
		{
			std::string str = "Color" + no;
			ImGui::ColorEdit4(str.c_str(), (float*)&material.color);
			++no;
		}

		ImGui::DragFloat("Color Alpha", &color.w, 0.5f,0, 255);

		ImGui::Text("Current Keyframe : %d", current_keyframe);

		ImGui::TreePop();
	}
}

FbxModelComponent::FbxModelComponent(Entity* entity, const std::shared_ptr<FbxModelResource>& resource)
	:Component{entity}
{
	this->resource = resource;

	// ノード
	const std::vector<ModelData::Node>& res_nodes = resource->GetModelData().nodes;

	nodes.resize(res_nodes.size());
	for (size_t node_index = 0; node_index < nodes.size(); ++node_index)
	{// ModelResource::Node と Model::Nodeを結び付ける
		auto&& src = res_nodes.at(node_index);
		auto&& dst = nodes.at(node_index);

		dst.name = src.name.c_str();
		dst.parent = src.parent_index >= 0 ? &nodes.at(src.parent_index) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
	}

	anim_states.AddState(FbxAnimationState::Switch, [this](const float delta_time) {BlendNextAnimation(delta_time); });
	anim_states.AddState(FbxAnimationState::Update, [this](const float delta_time) {UpdateAnimation(delta_time); });

	// アニメーションの初期値をセット
	SwitchAnimation(0, false, 0.0f);
}

void FbxModelComponent::Initialize(const std::shared_ptr<FbxModelResource>& resource)
{
	this->resource = resource;

	// ノード
	const std::vector<ModelData::Node>& res_nodes = resource->GetModelData().nodes;

	nodes.resize(res_nodes.size());
	for (size_t node_index = 0; node_index < nodes.size(); ++node_index)
	{// ModelResource::Node と Model::Nodeを結び付ける
		auto&& src = res_nodes.at(node_index);
		auto&& dst = nodes.at(node_index);

		dst.name = src.name.c_str();
		dst.parent = src.parent_index >= 0 ? &nodes.at(src.parent_index) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
	}

	{// メンバ変数の初期化
		current_animation_index = -1;
		prev_animation_index = -1;
		current_seconds = 0;
		prev_seconds = 0;
		current_keyframe = 0;

		prev_key_index = 0;

	}

	//anim_states.AddState(FbxAnimationState::Switch, [this](const float delta_time) {BlendNextAnimation(delta_time); });
	//anim_states.AddState(FbxAnimationState::Update, [this](const float delta_time) {UpdateAnimation(delta_time); });

	// アニメーションの初期値をセット
	SwitchAnimation(0, false, 0.0f);
}

// アニメーション再生
void FbxModelComponent::SwitchAnimation(int animation_index, bool loop, float switch_time)
{
	prev_animation_index		= current_animation_index;
	current_animation_index		= animation_index;
	loop_animation				= loop;
	end_animation				= false;
	prev_seconds				= current_seconds;
	current_seconds				= 0.0f;
	animation_switch_time		= switch_time;
	current_keyframe			= 0;

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
		// 前のアニメーションの切り替わった時点のキーフレームを算出
		prev_animation = resource->GetModelData().animations.at(prev_animation_index);
	}

	float rate = changer_timer / animation_switch_time;

	const ModelData::Keyframe& prev_keyframe = prev_animation.keyframes.at(prev_key_index);
	const ModelData::Keyframe& current_keyframe = resource->GetModelData().animations.at(current_animation_index).keyframes.at(0);

	int nodeCount = static_cast<int>(nodes.size());
	for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		// ２つのキーフレーム間の補完計算
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

// アニメーション計算
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
		// 現在の時間がどのキーフレームの間にいるか判定する
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
				// ２つのキーフレーム間の補完計算
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

	// 最終フレーム処理
	if (end_animation)
	{
		end_animation = false;
		current_animation_index = -1;
		prev_key_index = 0;
		return;
	}

	// 時間経過
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

// ローカル変換行列計算
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

// ワールド変換行列計算
void FbxModelComponent::CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform)
{
	for (Node& node : nodes)
	{

		DirectX::XMMATRIX local_transform_matrix = DirectX::XMLoadFloat4x4(&node.local_transform);
		DirectX::XMMATRIX world_transform_matrix = DirectX::XMLoadFloat4x4(&world_transform);
		if (node.parent != nullptr)
		{
			DirectX::XMMATRIX parent_transform = DirectX::XMLoadFloat4x4(&node.parent->world_transform);
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
		// 現在の時間がどのキーフレームの間にいるか判定する
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
		if (strcmp(node_name, node.name) == 0)
		{
			return node.world_transform;
		}
	}

	assert(!"Not found node name");

	return DirectX::SimpleMath::Matrix::Identity;
}

const DirectX::SimpleMath::Matrix& FbxModelComponent::GetNodePareantMatrix(const char* node_name)
{
	for (const auto& node : nodes)
	{
		if (strcmp(node_name, node.name) == 0)
		{
			return node.local_transform;
		}
	}

	assert(!"Not found node name");

	return DirectX::SimpleMath::Matrix::Identity;
}