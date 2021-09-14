#pragma once
#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"
#include "model.h"
#include "state_machine.h"
#include "rename_type_mapping.h"

enum class FbxAnimationState
{
	Switch,
	Blending,
	Update,
};

namespace cumulonimbus::component
{
	class ModelComponent final : public ComponentBase
	{
	public:
		struct Node
		{
			std::string					name{};
			int							parent_index{ -1 };
			//Node*						parent{};
			DirectX::XMFLOAT3			scale{};
			DirectX::XMFLOAT4			rotate{};
			DirectX::XMFLOAT3			translate{};
			DirectX::SimpleMath::Matrix	local_transform{};
			DirectX::SimpleMath::Matrix	world_transform{};

			template<typename Archive>
			void serialize(Archive&& archive)
			{
				archive(
					cereal::make_nvp("node_name", name),
					CEREAL_NVP(parent_index),
					//cereal::make_nvp("parent node", parent),
					CEREAL_NVP(scale),
					CEREAL_NVP(rotate),
					CEREAL_NVP(translate),
					CEREAL_NVP(local_transform),
					CEREAL_NVP(world_transform)
				);
			}
		};
		using ComponentBase::ComponentBase;
		explicit ModelComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const mapping::rename_type::UUID& model_id);
		explicit ModelComponent() = default; // for cereal
		~ModelComponent() override = default;

		void NewFrame(float delta_time) override;
		void Update(float delta_time) override;
		void RenderImGui() override;

		template<class Archive>
		void serialize(Archive&& archive);

		// �A�j���[�V����
		[[nodiscard]] bool IsPlayAnimation() const;
		/**
		 * @brief					: �A�j���[�V�����̑J��
		 * @param animation_index	: �i�[����Ă���A�j���[�V�����̗v�f�ԍ�
		 * @param loop				: ���[�v�Đ������邩
		 * @param switch_time		: ���̃A�j���[�V�����ɑJ�ڂ���܂ł̎���
		 */
		void SwitchAnimation(int animation_index, bool loop = false, float switch_time = 0.1f);

		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeMatrix(const char* node_name);
		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeParentMatrix(const char* node_name);
		[[nodiscard]] const std::vector<Node>& GetNodes()		const { return nodes; }
		[[nodiscard]] std::vector<Node>& GetNodes() { return nodes; }
		// Resource������ID�������f���̎Q�Ƃ�Ԃ��悤�ɂ���
		//[[nodiscard]] const FbxModelResource* GetResource()	const { return resource.get(); }
		//[[nodiscard]] Model* GetResource() { return resource.get(); }

		void UpdateAnimState(const float delta_time);

		// ���݂̃L�[�t���[��
		[[nodiscard]] int CurrentKeyframe() const { return current_keyframe; }

		void Load(ecs::Registry* registry) override;
	private:
		mapping::rename_type::UUID model_id{};
		std::vector<Node> nodes{};

		int	prev_key_index{0};
		int	current_keyframe{0};	// ���݂̃L�[�t���[��
		int	prev_animation_index{-1};	// �O�̃A�j���[�V�����̃C���f�b�N�X�ԍ�(�u�����h�Ŏg�p)
		int	current_animation_index{-1};

		float prev_seconds{.0f}; // �A�j���[�V�������؂�ւ�������_�̑O�̃A�j���[�V�����̃L�[�t���[��(�u�����h�Ŏg�p)
		float changer_timer{.0f};
		float current_seconds{.0f};
		float animation_switch_time{.0f};

		bool end_animation{false};
		bool loop_animation{false}; // �A�j���[�V�����̃��[�v�Đ�

		asset::ModelData::Animation prev_animation{};
		StateMachine<FbxAnimationState, void, const float>	anim_states{};

		void InitializeParameter();
		// Model��ID��������擾
		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent, const mapping::rename_type::UUID& model_id);

		// �O�̃A�j���[�V�����̃L�[�t���[�����Z�o
		// brief : �A�j���[�V�������؂�ւ�鎞�Ɏg�p
		[[nodiscard]]
		int CalcPrevKeyframe() const;

		// �s��v�Z
		void CalculateLocalTransform();
		void CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform);

		void BlendNextAnimation(float dt);
		void UpdateAnimation(float dt);
	};
} // cumulonimbus::component
