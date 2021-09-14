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

		// アニメーション
		[[nodiscard]] bool IsPlayAnimation() const;
		/**
		 * @brief					: アニメーションの遷移
		 * @param animation_index	: 格納されているアニメーションの要素番号
		 * @param loop				: ループ再生をするか
		 * @param switch_time		: 次のアニメーションに遷移するまでの時間
		 */
		void SwitchAnimation(int animation_index, bool loop = false, float switch_time = 0.1f);

		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeMatrix(const char* node_name);
		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeParentMatrix(const char* node_name);
		[[nodiscard]] const std::vector<Node>& GetNodes()		const { return nodes; }
		[[nodiscard]] std::vector<Node>& GetNodes() { return nodes; }
		// Resource部を現IDが持つモデルの参照を返すようにする
		//[[nodiscard]] const FbxModelResource* GetResource()	const { return resource.get(); }
		//[[nodiscard]] Model* GetResource() { return resource.get(); }

		void UpdateAnimState(const float delta_time);

		// 現在のキーフレーム
		[[nodiscard]] int CurrentKeyframe() const { return current_keyframe; }

		void Load(ecs::Registry* registry) override;
	private:
		mapping::rename_type::UUID model_id{};
		std::vector<Node> nodes{};

		int	prev_key_index{0};
		int	current_keyframe{0};	// 現在のキーフレーム
		int	prev_animation_index{-1};	// 前のアニメーションのインデックス番号(ブレンドで使用)
		int	current_animation_index{-1};

		float prev_seconds{.0f}; // アニメーションが切り替わった時点の前のアニメーションのキーフレーム(ブレンドで使用)
		float changer_timer{.0f};
		float current_seconds{.0f};
		float animation_switch_time{.0f};

		bool end_animation{false};
		bool loop_animation{false}; // アニメーションのループ再生

		asset::ModelData::Animation prev_animation{};
		StateMachine<FbxAnimationState, void, const float>	anim_states{};

		void InitializeParameter();
		// ModelのIDから情報を取得
		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent, const mapping::rename_type::UUID& model_id);

		// 前のアニメーションのキーフレームを算出
		// brief : アニメーションが切り替わる時に使用
		[[nodiscard]]
		int CalcPrevKeyframe() const;

		// 行列計算
		void CalculateLocalTransform();
		void CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform);

		void BlendNextAnimation(float dt);
		void UpdateAnimation(float dt);
	};
} // cumulonimbus::component
