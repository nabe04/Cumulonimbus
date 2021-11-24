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

	namespace system
	{
		class System;
	} // system

	namespace scene
	{
		class SceneManager;
	} // scene

	namespace component
	{
		class ModelComponent;
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
	class CollisionPrimitiveAsset final
	{
	public:
		explicit CollisionPrimitiveAsset(system::System& system);
		explicit CollisionPrimitiveAsset()									= default;
		~CollisionPrimitiveAsset()											= default;
		CollisionPrimitiveAsset(CollisionPrimitiveAsset&)					= delete;
		CollisionPrimitiveAsset(CollisionPrimitiveAsset&&)					= delete;
		CollisionPrimitiveAsset(const CollisionPrimitiveAsset&)				= delete;
		CollisionPrimitiveAsset(const CollisionPrimitiveAsset&&)			= delete;
		CollisionPrimitiveAsset& operator=(CollisionPrimitiveAsset&)		= delete;
		CollisionPrimitiveAsset& operator=(CollisionPrimitiveAsset&&)		= delete;
		CollisionPrimitiveAsset& operator=(const CollisionPrimitiveAsset&)	= delete;
		CollisionPrimitiveAsset& operator=(const CollisionPrimitiveAsset&&) = delete;

		template<class Archive>
		void load(Archive&& archive,uint32_t version)
		{
			archive(
				CEREAL_NVP(sphere_id),
				CEREAL_NVP(capsule_id)
			);
		}

		template<class Archive>
		void save(Archive&& archive,uint32_t version) const
		{
			archive(
				CEREAL_NVP(sphere_id),
				CEREAL_NVP(capsule_id)
			);
		}

		void RenderImGui(ecs::Registry* registry);

		[[nodiscard]]
		const mapping::rename_type::UUID& GetSphereId() const
		{
			return sphere_id;
		}

		[[nodiscard]]
		const mapping::rename_type::UUID& GetCapsuleId() const
		{
			return capsule_id;
		}
	private:
		mapping::rename_type::UUID sphere_id{};
		mapping::rename_type::UUID capsule_id{};
	};

	class CollisionManager final
	{
	public:
		explicit CollisionManager(system::System& system);
		explicit CollisionManager()								= default; // for cereal
		~CollisionManager()										= default;
		CollisionManager(CollisionManager&)						= delete;
		CollisionManager(CollisionManager&&)					= delete;
		CollisionManager(const CollisionManager&)				= delete;
		CollisionManager(const CollisionManager&&)				= delete;
		CollisionManager& operator=(CollisionManager&)			= delete;
		CollisionManager& operator=(CollisionManager&&)			= delete;
		CollisionManager& operator=(const CollisionManager&)	= delete;
		CollisionManager& operator=(const CollisionManager&&)	= delete;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(CEREAL_NVP(ent_terrains));
		}

		/**
		 * @brief : CollisionComponentを持つエンティティ全体の当たり判定処理
		 */
		void Update(float dt, ecs::Registry* registry);

		void Update(float dt, scene::SceneManager& scene_manager);

		void RenderImGui(ecs::Registry* registry);

		/**
		 * @brief : 地形として判定させたいモデルの登録
		 *		    SceneのInitialize関数内などのComponentを追加した段階
		 *		    に登録する
		 */
		void RegistryRayCastModel(mapping::rename_type::Entity ent);

		[[nodiscard]]
		bool IntersectRayVsDragModel(ecs::Registry* registry,
									 const DirectX::SimpleMath::Vector3& ray_start,
									 const DirectX::SimpleMath::Vector3& ray_end,
									 DirectX::SimpleMath::Vector3* hit_pos);

		/**
		 * @brief : レイキャスト対象のモデルが存在するか
		 * @return : true -> 存在する
		 * @return : false -> 存在しない
		 */
		[[nodiscard]]
		bool HasTerrain(mapping::rename_type::Entity ent);
	private:
		// レイキャストの判定を行う際の判定が行われる(地形)データ群
		std::vector<mapping::rename_type::Entity> ent_terrains{};

		/**
		 * @brief	: 反発係数の算出
		 * @return	: 反発係数(0〜1)
		 */
		float CalculateRestitution(
			const component::PhysicMaterialComponent* physic_material_comp_1,
			const component::PhysicMaterialComponent* physic_material_comp_2);

		/**
		 * @brief : 押出し処理
		 * @remark : 同一シーン同士
		 * @param dt : delta time
		 * @param registry : コンポーネントの取得に使用
		 * @param ent_1	: 押出し対象エンティティ(1)
		 * @param ent_2	: 押出し対象エンティティ(2)
		 * @param mass_point_1 : 質点(1)
		 * @param mass_point_2 : 質点(2)
		 * @param collision_preset_1 : コリジョンプリセット(1)
		 * @param collision_preset_2 : コリジョンプリセット(2)
		 * @param penetration : めり込み具合
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
		 * @brief : 押出し処理
		 * @remark : 別シーン同士
		 * @param dt : delta time
		 * @param registry_1 : シーン1のレジストリ
		 * @param registry_2 : シーン2のレジストリ
		 * @param ent_1	: 押出し対象エンティティ(1)
		 * @param ent_2	: 押出し対象エンティティ(2)
		 * @param mass_point_1 : 質点(1)
		 * @param mass_point_2 : 質点(2)
		 * @param collision_preset_1 : コリジョンプリセット(1)
		 * @param collision_preset_2 : コリジョンプリセット(2)
		 * @param penetration : めり込み具合
		 */
		void Extrude(
			float dt,
			ecs::Registry* registry_1,
			ecs::Registry* registry_2,
			const mapping::rename_type::Entity& ent_1,
			const mapping::rename_type::Entity& ent_2,
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
			const component::ModelComponent& model,
			component::RayCastComponent& ray_cast_comp);

		/**
		 * @brief : 球と球の当たり判定
		 * @remark : 同一シーン同士
		 */
		bool IntersectSphereVsSphere(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent& sphere_1,
			component::SphereCollisionComponent& sphere_2);
		/**
		 * @brief : 球と球の当たり判定
		 * @remark : 別シーン同士
		 */
		bool IntersectSphereVsSphere(
			float dt,
			ecs::Registry* registry_1, ecs::Registry* registry_2,
			component::SphereCollisionComponent& sphere_1,
			component::SphereCollisionComponent& sphere_2);

		/**
		 * @brief : カプセルとカプセルの当たり判定
		 * @remark : 同一のシーン同士
		 */
		bool IntersectCapsuleVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::CapsuleCollisionComponent& capsule_1,
			component::CapsuleCollisionComponent& capsule_2);
		/**
		 * @brief : カプセルとカプセルの当たり判定
		 * @remark : 別シーン同士
		 */
		bool IntersectCapsuleVsCapsule(
			float dt,
			ecs::Registry* registry_1, ecs::Registry* registry_2,
			component::CapsuleCollisionComponent& capsule_1,
			component::CapsuleCollisionComponent& capsule_2);

		/**
		 * @brief     : 球とカプセルの当たり判定
		 * @attention : 同じエンティティ内での球とカプセルの判定は行わない
		 */
		bool IntersectSphereVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent& sphere,
			component::CapsuleCollisionComponent& capsule);
	};

} // cumulonimbus::collision