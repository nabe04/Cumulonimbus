#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class PhysicMaterialComponent : public ComponentBase
	{
	public:
		/**
		 * @attention : 接触している 2 つのコライダーに異なる combine モードが設定されているときは特別で最も高い優先度をもつ機能が使用される。
		 *				 Average < Minimum < Multiply < Maximum
		 *				 あるマテリアルが Average モードを持ち、他が Maximum モードを持つとき、 combine 機能は Maximum で使用される。
		 */
		enum class CombineSetting
		{
			Average,
			Minimum,
			Maximum,
			Multiply,

			End
		};

	public:
		using ComponentBase::ComponentBase;
		explicit PhysicMaterialComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PhysicMaterialComponent()  = default;
		~PhysicMaterialComponent() override = default;

		void Update(float dt) override;
		void RenderImGui() override;
		void Load(ecs::Registry* registry) override;

		[[nodiscard]] float GetDynamicFriction()			const { return dynamic_friction; }
		[[nodiscard]] float GetStaticFriction()				const { return static_friction; }
		[[nodiscard]] float GetBounciness()					const { return bounciness; }
		[[nodiscard]] CombineSetting GetFrictionCombine()	const { return friction_combine; }
		[[nodiscard]] CombineSetting GetBounceCombine()		const { return bounce_combine; }

		void SetDynamicFriction(const float friction)			{ dynamic_friction = friction; }
		void SetStaticFriction(const float friction)			{ static_friction = friction; }
		void SetBounciness(const float bounciness)				{ this->bounciness = bounciness; }
		void SetFrictionCombine(const CombineSetting setting)	{ friction_combine = setting; }
		void SetBounceCombine(const CombineSetting setting)		{ bounce_combine = setting; }

	private:
		float dynamic_friction = 0.5f;	// 移動している物体に対する摩擦。0 から 1の間の値を使用。
		float static_friction  = 0.5f;  // 面上で静止しているオブジェクトに使用される摩擦。通常は、0 から 1 の間の値を使用。
		float bounciness	   = 0.5f;  // 表面にどれだけ弾性があるか。0 の場合は弾性なし。1 の場合はエネルギーが減ることなく跳ねる。
		CombineSetting friction_combine = CombineSetting::Average;	// 衝突するオブジェクト間の摩擦をどう処理するか
		CombineSetting bounce_combine   = CombineSetting::Average;	// 衝突するオブジェクト間の跳ね返し度合いをどう処理するか。
	};
} // cumulonimbus::component
