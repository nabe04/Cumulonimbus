#pragma once

#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "ecs.h"
#include "blend.h"
#include "rasterizer.h"
#include "state_machine.h"
#include "model_data.h"
#include "locator.h"

class FbxModelResource;

namespace shader
{
	class MeshShaderState;
	enum class MeshShaderTypes;
}

enum class FbxAnimationState
{
	Switch,
	Blending,
	Update,
};

class FbxModelComponent final : public Component
{
public:
	struct Node
	{
		const char*					name{};
		Node*						parent{};
		DirectX::XMFLOAT3			scale{};
		DirectX::XMFLOAT4			rotate{};
		DirectX::XMFLOAT3			translate{};
		DirectX::SimpleMath::Matrix	local_transform{};
		DirectX::SimpleMath::Matrix	world_transform{};
	};

	// ��
	DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };

private:
	std::shared_ptr<FbxModelResource>	resource;
	std::vector<Node>					nodes;
	int									current_animation_index		= -1;
	int									prev_animation_index		= -1;	// �O�̃A�j���[�V�����̃C���f�b�N�X�ԍ�(�u�����h�Ŏg�p)
	float								current_seconds				= 0.0f;
	float								prev_seconds				= 0.0f; // �A�j���[�V�������؂�ւ�������_�̑O�̃A�j���[�V�����̃L�[�t���[��(�u�����h�Ŏg�p)
	bool								loop_animation				= false;
	bool								end_animation				= false;
	int									current_keyframe			= 0;	// ���݂̃L�[�t���[��

	float								animation_switch_time		= 0;
	float								changer_timer				= 0;
	int									prev_key_index				= 0;
	ModelData::Animation				prev_animation;
	StateMachine<FbxAnimationState,void ,const float>		anim_states;

public:
	FbxModelComponent(Entity* entity, const std::shared_ptr<FbxModelResource>& resource);
	~FbxModelComponent() = default;

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	virtual void RenderImGui() override;

	// �A�j���[�V����
	[[nodiscard]] bool IsPlayAnimation() const { return current_animation_index >= 0; }
	void SwitchAnimation(int animationIndex, bool loop = false, float switch_time = 0.1f);

	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeMatrix(const char* node_name);
	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodePareantMatrix(const char* node_name);
	[[nodiscard]] const std::vector<Node>& GetNodes() const { return nodes; }
	[[nodiscard]] std::vector<Node>& GetNodes() { return nodes; }
	[[nodiscard]] const FbxModelResource* GetResource() const { return resource.get(); }

	void UpdateAnimState(const float delta_time) { anim_states.Update(delta_time); }

	// ���݂̃L�[�t���[��
	[[nodiscard]] int CurrentKeyframe() { return current_keyframe; };

private:
	[[noreturn]] void Initialize(const std::shared_ptr<FbxModelResource>& resource);

	// �O�̃A�j���[�V�����̃L�[�t���[�����Z�o
	// brief : �A�j���[�V�������؂�ւ�鎞�Ɏg�p
	[[nodiscard]] int CalcPrevKeyframe(int key_index);

	// �s��v�Z
	void CalculateLocalTransform();
	void CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform);

	void BlendNextAnimation(float elapsedTime);
	void UpdateAnimation(float elapsedTime);
};