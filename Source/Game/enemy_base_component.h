#pragma once
#include <unordered_map>
#include <string>

#include "actor3d_component.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::component
{
	class EnemyBaseComponent : public Actor3DComponent
	{
	public:
		explicit EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemyBaseComponent()  = default;
		~EnemyBaseComponent() override = default;

		void Update(float dt) override {}
		void RenderImGui()	  override {}
		void Load(ecs::Registry* registry) override {}

	protected:
		/**
		 * @brief : 最小値、最大値からランダムなfloat値を設定する構造体
		 * @brief : 用途は自由
		 */
		struct RandomFloat
		{
			float min = 0; // 最小値
			float max = 1; // 最大値
			float random_val = 0; // minからmaxまでのランダム値
			float current_time = 0; // 計測時間

			/**
			 * @brief : min,maxの値をもとにrandom_valの値を設定
			 */
			void SetRandomVal();
		};

		
		std::unordered_map<std::string, RandomFloat> transition_timer{};
		// モデルのyaw回転(度数法)のランダム値
		RandomFloat random_rotation_angle{};

		/**
		 * @brief			: "transition_timer"の登録
		 *					  すでに登録されていた場合値の上書きをする
		 * @param key_value	: "transition_timer"で使用するキー値
		 * @param min		: "transition_timer"内のランダム値(最小値)
		 * @param max		: "transition_timer"内のランダム値(最大値)
		 */
		void RegisterTransitionTimer(const std::string& key_value,float min,float max);

		/**
		* @brief		: "random_rotation_angle"メンバ変数の値設定用関数
		* @brief		: min、maxで指定した範囲内でのランダム値の算出
		* @param min	: 最小値
		* @param max	: 最大値
		*/
		void SetRandomRotationAngle(float min, float max);
		
		/**
		 * @brief : 自身の位置と前方ベクトルから索敵範囲内(rad)、索敵距離内(length)に
		 *			ターゲット(target_pos)がいるか判定
		 * @param target_pos    : 索敵対象位置
		 * @param search_range  : 索敵範囲(度数法)
		 * @param search_length : 索敵距離
		 * @return : true -> 索敵範囲内
		 */
		[[nodiscard]] bool Search(const DirectX::SimpleMath::Vector3& target_pos, float search_range, float search_length) const;

		/**
		 * @brief				: 指定した位置への追跡処理(モデルの回転処理も含む)
		 * @param target_pos	: 追跡先の位置
		 * @param velocity		: 追跡速度
		 * @param distance		: 自身と追跡対象の長さを返す
		 */
		void Tracking(DirectX::SimpleMath::Vector3 target_pos, const DirectX::SimpleMath::Vector3& velocity, float& distance);

		/**
		 * @brief : プレイヤーの方向への回転処理
		 */
		void RotateToPlayerDirection() const ;
	};
} // cumulonimbus::component
