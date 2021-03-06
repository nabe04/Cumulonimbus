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
#include "graphics_state.h"

enum class AnimationState
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
		explicit ModelComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const ModelComponent& copy_comp); // for prefab
		explicit ModelComponent() = default; // for cereal
		~ModelComponent() override = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void CommonUpdate(float dt)     override;
		void PostCommonUpdate(float dt) override;

		void SceneUpdate(float dt) override;

		void PreGameUpdate(float dt) override;
		void GameUpdate(float dt) override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;
		void OnDeserialize(ecs::Registry* registry, const std::map<mapping::rename_type::Entity, mapping::rename_type::Entity>& connector) override;

		/**
		 * @brief					: アニメーションの遷移
		 * @param animation_index	: 格納されているアニメーションの要素番号
		 * @param loop				: ループ再生をするか
		 * @param switch_time		: 次のアニメーションに遷移するまでの時間
		 */
		void SwitchAnimation(int animation_index, bool loop = false, float switch_time = 0.1f);

		void UpdateAnimState(const float delta_time);


		[[nodiscard]]
		bool IsPlayAnimation() const;
		/**
		 * @brief : ノード名からノードのインデック番号を取得する
		 * @remark : 存在しない場合-1を返す
		 * @param node_name : 取得したいインデックス番号のノード名
		 * @return : 存在する : 取得したノードのインデックス番号
		 * @return : 存在しない : -1
		 */
		[[nodiscard]]
		int GetNodeIndexFromName(const std::string& node_name) const;
		/**
		 * @brief : 指定のノード名が存在するかの判別
		 * @param node_name : 判別対象のノード名
		 */
		[[nodiscard]]
		bool HasNodeFromName(const std::string& node_name) const;

		/**
		 * @brief : アセットの持つマテリアルを取得
		 * @param material_index : Subsetのもつmaterial_indexを指定
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetMaterialID(u_int material_index) const;
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetNodeMatrix(const char* node_name);
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetNodeParentMatrix(const char* node_name);
		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetNodeWorldPos(const char* node_name);

		bool HasNode(const char* node_name) const;
		[[nodiscard]]
		const std::vector<Node>& GetNodes()	const { return nodes; }
		[[nodiscard]]
		std::vector<Node>& GetNodes() { return nodes; }
		// 現在のキーフレーム
		[[nodiscard]]
		int CurrentKeyframe() const { return current_keyframe; }
		[[nodiscard]]
		const mapping::rename_type::UUID& GetModelID() const { return model_id; }
		[[nodiscard]]
		bool GetIsVisible() const { return is_visible; }
		[[nodiscard]]
		const graphics::GraphicsState& GetGraphicsState() const { return graphics_state; }
		[[nodiscard]]
		bool GetIsStopAnimation() const { return is_stop_animation; }

		void SetIsVisible(const bool result)
		{
			is_visible = result;
		}

		void SetIsUseTimeScale(const bool arg)
		{
			is_use_time_scale = arg;
		}

		void SetIsStopAnimation(const bool arg)
		{
			is_stop_animation = arg;
		}

		/**
		 * @brief : ImGui上でのノード名選択関数
		 * @param node_name : 取得するノード名
		 * @return : true -> ノード名が変更された
		 */
		bool ImSelectableNode(std::string& node_name);
	private:
		mapping::rename_type::UUID				model_id{};
		std::vector<mapping::rename_type::UUID> material_ids{};	// マテリアルコピー用
		std::vector<Node>						nodes{};
		graphics::GraphicsState					graphics_state{};

		int	prev_key_index			{ 0 };
		int	current_keyframe		{ 0 };	// 現在のキーフレーム
		int	prev_animation_index	{ -1 };	// 前のアニメーションのインデックス番号(ブレンドで使用)
		int	current_animation_index	{ -1 };

		float prev_seconds			{ .0f }; // アニメーションが切り替わった時点の前のアニメーションのキーフレーム(ブレンドで使用)
		float changer_timer			{ .0f };
		float current_seconds		{ .0f };
		float animation_switch_time	{ .0f };

		bool is_visible		{ true };	// 描画するか
		bool end_animation	{ false };
		bool loop_animation	{ false };  // アニメーションのループ再生
		bool is_use_time_scale{ true }; // アニメーション計算時にタイムスケールを使用するか
		bool is_stop_animation{ false };

		StateMachine<AnimationState, void, const float>	anim_states{};
		asset::ModelData::Animation prev_animation{};

		/**
		 * @brief : メンバ変数の初期化
		 */
		void InitializeParameter();
		/**
		 * @brief : anim_statesの初期化 & 再設定
		 */
		void InitializeAnimState(ecs::Registry* registry, mapping::rename_type::Entity ent);
		/**
		 * @brief : ModelのIDから情報を取得
		 */
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
