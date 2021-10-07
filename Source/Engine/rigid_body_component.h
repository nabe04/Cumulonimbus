#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"

namespace cumulonimbus::component
{
	class RigidBodyComponent final : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit RigidBodyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit RigidBodyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const RigidBodyComponent& copy_comp); // for prefab
		explicit RigidBodyComponent()  = default; // for cereal
		~RigidBodyComponent() override = default;

		void PreGameUpdate(float dt)		override;
		void GameUpdate(float dt)			override;
		void PostGameUpdate(float dt)		override;
		void RenderImGui()					override;
		void Load(ecs::Registry* registry)	override;

		template<class Archive>
		void serialize(Archive&& archive);

		void Integrate(float dt);

		/**
		 * @brief   : 速度の加算
		 * @param v : 加算する速度
		 */
		void AddVelocity(const DirectX::SimpleMath::Vector3& v);

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
		void GravityStop(bool flg);

		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetVelocity()     const { return velocity; }
		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetAcceleration() const { return acceleration; }
		[[nodiscard]] float GetJumpStrength()								const { return jump_strength; }
		[[nodiscard]] float GetGravity()									const { return gravity; }
		[[nodiscard]] float GetCurrentGravity()								const { return current_gravity; }
		[[nodiscard]] float GetMass()										const { return mass; }
		[[nodiscard]] bool  GetIsGravity()									const { return is_gravity; }

		void SetCurrentGravity(const float gravity)	{ current_gravity = gravity; }

		void SetMass(const float mass) { this->mass = mass; }
	private:
		DirectX::SimpleMath::Vector3 velocity{};
		DirectX::SimpleMath::Vector3 acceleration{};

		float mass			  = 1.0f; // 質量
		float jump_strength   = 500;  // ジャンプ強度
		float gravity		  = -20;  // 重力
		float current_gravity = 0;

		bool is_gravity = true;	// 重力フラグ
		bool is_landing;		// 地面フラグ
		bool is_jumping;		// ジャンプフラグ
	};
} // cumulonimbus::component