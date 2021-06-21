#pragma once

#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>


#include "component_base.h"
#include "constant_buffer.h"
#include "locator.h"
#include "model_data.h"
#include "rename_type_mapping.h"
#include "shader_interop_renderer.h"
#include "state_machine.h"

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

namespace cumulonimbus::component
{
	class FbxModelComponent final : public ComponentBase
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

		explicit FbxModelComponent() = default; // for cereal
		explicit FbxModelComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const std::shared_ptr<FbxModelResource>& resource);
		~FbxModelComponent() override = default;

		void NewFrame(float delta_time) override;
		void Update(float delta_time) override;
		void RenderImGui() override;

		// アニメーション
		[[nodiscard]] bool IsPlayAnimation() const { return current_animation_index >= 0; }
		void SwitchAnimation(int animation_index, bool loop = false, float switch_time = 0.1f);

		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeMatrix(const char* node_name);
		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodePareantMatrix(const char* node_name);
		[[nodiscard]] const std::vector<Node>& GetNodes() const { return nodes; }
		[[nodiscard]] std::vector<Node>& GetNodes() { return nodes; }
		[[nodiscard]] const FbxModelResource* GetResource() const { return resource.get(); }
		[[nodiscard]] const DirectX::XMFLOAT4& GetColor() const { return color; }

		void UpdateAnimState(const float delta_time) { anim_states.Update(delta_time); }

		// 現在のキーフレーム
		[[nodiscard]] int CurrentKeyframe() { return current_keyframe; }

		/*
		 * brief : アニメーションを再生させるか
		 * true  : アニメーションなし
		 * false : アニメーションあり
		 */
		void SetIsStatic(const bool flg) { is_static = flg; }

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		// For cereal
		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<ComponentBase>(this),

				CEREAL_NVP(resource),
				CEREAL_NVP(nodes),

				CEREAL_NVP(current_animation_index),
				CEREAL_NVP(prev_animation_index),
				CEREAL_NVP(current_keyframe),
				CEREAL_NVP(prev_key_index),

				CEREAL_NVP(current_seconds),
				CEREAL_NVP(prev_seconds),
				CEREAL_NVP(animation_switch_time),
				CEREAL_NVP(changer_timer),

				CEREAL_NVP(loop_animation),
				CEREAL_NVP(end_animation),

				CEREAL_NVP(prev_animation),
				CEREAL_NVP(anim_states)
			);
		}

	private:
		std::shared_ptr<FbxModelResource>	resource{nullptr};
		std::vector<Node>					nodes{};

		int	current_animation_index = -1;
		int	prev_animation_index	= -1;	// 前のアニメーションのインデックス番号(ブレンドで使用)
		int	current_keyframe		= 0;	// 現在のキーフレーム
		int	prev_key_index			= 0;

		float current_seconds		= 0.0f;
		float prev_seconds			= 0.0f; // アニメーションが切り替わった時点の前のアニメーションのキーフレーム(ブレンドで使用)
		float animation_switch_time = 0;
		float changer_timer			= 0;

		bool is_static				= true;  // アニメーションの再生をさせるか(true : アニメーションなし、 false : アニメーションあり)
		bool loop_animation			= false; // アニメーションのループ再生
		bool end_animation			= false;


		ModelData::Animation prev_animation;
		StateMachine<FbxAnimationState, void, const float>	anim_states;

		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };

		void Initialize(const std::shared_ptr<FbxModelResource>& resource);

		// 前のアニメーションのキーフレームを算出
		// brief : アニメーションが切り替わる時に使用
		[[nodiscard]] int CalcPrevKeyframe(int key_index);

		// 行列計算
		void CalculateLocalTransform();
		void CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform);

		void BlendNextAnimation(float elapsedTime);
		void UpdateAnimation(float elapsedTime);
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::FbxModelComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::FbxModelComponent)
