#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "rename_type_mapping.h"

class ModelData;

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	}// ecs

	namespace component
	{
		class FbxModelComponent;
		class SphereCollisionComponent;
		class CapsuleCollisionComponent;
		class RayCastComponent;
		class PhysicMaterialComponent;
	} // component

	namespace collision
	{
		struct		HitResult;
		struct		Sphere;
		enum class	CollisionPreset;
	} // collision

} // cumulonimbus

namespace cumulonimbus::collision
{
	class CollisionManager final
	{
	public:
		explicit CollisionManager() = default;
		~CollisionManager()			= default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(CEREAL_NVP(ent_terrains));
		}

		/**
		 * @brief : CollisionComponentを持つエンティティ全体の当たり判定処理
		 */
		void Update(float dt, ecs::Registry* registry);

		void RenderImGui(ecs::Registry* registry);

		/**
		 * @brief : 地形として判定させたい物の登録
		 *		   SceneのInitialize関数内などのComponentを追加した段階
		 *		   に登録する
		 */
		void RegistryRayCastModel(mapping::rename_type::Entity ent);
	private:
		// レイキャストの判定を行う際の判定が行われる(地形)データ群
		std::vector<mapping::rename_type::Entity> ent_terrains{};


		/**
		 * @brief	: 反発係数の算出
		 * @return	: 反発係数(0～1)
		 */
		float CalculateRestitution(
			const component::PhysicMaterialComponent* physic_material_comp_1,
			const component::PhysicMaterialComponent* physic_material_comp_2);

		/**
		 * @brief						: 押出し処理
		 * @param dt					: delta time
		 * @param registry				: コンポーネントの取得に使用
		 * @param ent_1					: 押出し対象エンティティ(1)
		 * @param ent_2					: 押出し対象エンティティ(2)
		 * @param mass_point_1			: 質点(1)
		 * @param mass_point_2			: 質点(2)
		 * @param collision_preset_1	: コリジョンプリセット(1)
		 * @param collision_preset_2	: コリジョンプリセット(2)
		 * @param penetration			: めり込み具合
		 */
		void Extrude(
			float dt,
			ecs::Registry* registry,
			mapping::rename_type::Entity ent_1,
			mapping::rename_type::Entity ent_2,
			const DirectX::SimpleMath::Vector3& mass_point_1,
			const DirectX::SimpleMath::Vector3& mass_point_2,
			CollisionPreset collision_preset_1,
			CollisionPreset collision_preset_2,
			float penetration);

		/**
		 * @brief : レイとモデルの衝突判定
		 */
		bool IntersectRayVsModel(
			float dt,
			ecs::Registry* registry,
			const component::FbxModelComponent& model,
			component::RayCastComponent& ray_cast_comp
		);

		/**
		 * @brief : 球と球の当たり判定
		 *	        SphereCollisionComponentが持つSphere分処理を回す
		 */
		bool IntersectSphereVsSphere(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent& sphere_1,
			component::SphereCollisionComponent& sphere_2
		);

		/**
		 * @brief : カプセルとカプセルの当たり判定
		 */
		bool IntersectCapsuleVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::CapsuleCollisionComponent& capsule_1,
			component::CapsuleCollisionComponent& capsule_2
		);

		/**
		 * @brief     : 球とカプセルの当たり判定
		 * @attention : 同じエンティティ内での球とカプセルの判定は行わない
		 */
		bool IntersectSphereVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent&  sphere,
			component::CapsuleCollisionComponent& capsule
		);
	};

} // cumulonimbus::collision