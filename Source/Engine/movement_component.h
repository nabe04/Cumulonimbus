#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"

namespace cumulonimbus::component
{
	class MovementComponent final : public ComponentBase
	{
	public:
		explicit MovementComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent);
		explicit MovementComponent()  = default; // for cereal
		~MovementComponent() override = default;

		void NewFrame(float dt) override;
		void Update(float dt) override;
		void PostUpdate(float dt) override;
		void RenderImGui() override;
		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		/**
		 * @brief		: モデルの前方ベクトルに力を加える
		 * @param force	: 力を加える量
		 */
		void AddForce(const DirectX::SimpleMath::Vector3& force);
		/**
		 * @brief			: ジャンプ処理関数
		 * @param strength	: ジャンプ強度(0未満の場合はメンバ変数(jump_strength)の値を使用する)
		 * @attention		: 関数を呼ぶたびにジャンプされる(多段ジャンプのようになる)
		 */
		void Jump(float strength = -1);
		/**
		 * @brief		: ジャンプを止める
		 * @param flg	: true  -> 重力を含む位置更新処理を止める(is_gravityとfalseにする)。current_gravityの値を0にリセット
		 *				  false -> is_gravityをtrueにする
		 */
		void JumpStop(bool flg);

		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetVelocity()     const { return velocity; }
		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetAcceleration() const { return acceleration; }
		[[nodiscard]] float GetJumpStrength()								const { return jump_strength; }
		[[nodiscard]] float GetGravity()									const { return gravity; }
		[[nodiscard]] float GetCurrentGravity()								const { return current_gravity; }
	private:
		DirectX::SimpleMath::Vector3 velocity{};
		DirectX::SimpleMath::Vector3 acceleration{};

		float jump_strength   = 500; // ジャンプ強度
		float gravity		  = -20; // 重力
		float current_gravity = 0;

		bool is_gravity = true;	// 重力フラグ
		bool is_landing;	// 地面フラグ
		bool is_jumping;	// ジャンプフラグ

		void Integrate(float dt);
	};
} // cumulonimbus::component