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
		struct		Capsule;
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
		 * @brief : CollisionComponent�����G���e�B�e�B�S�̂̓����蔻�菈��
		 */
		void Update(float dt, ecs::Registry* registry);

		void Update(float dt, scene::SceneManager& scene_manager);

		void RenderImGui(ecs::Registry* registry);

		/**
		 * @brief : �n�`�Ƃ��Ĕ��肳���������f���̓o�^
		 *		    Scene��Initialize�֐����Ȃǂ�Component��ǉ������i�K
		 *		    �ɓo�^����
		 */
		void RegistryRayCastModel(mapping::rename_type::Entity ent);

		[[nodiscard]]
		bool IntersectRayVsDragModel(ecs::Registry* registry,
									 const DirectX::SimpleMath::Vector3& ray_start,
									 const DirectX::SimpleMath::Vector3& ray_end,
									 DirectX::SimpleMath::Vector3* hit_pos);

		/**
		 * @brief : ���C�L���X�g�Ώۂ̃��f�������݂��邩
		 * @return : true -> ���݂���
		 * @return : false -> ���݂��Ȃ�
		 */
		[[nodiscard]]
		bool HasTerrain(mapping::rename_type::Entity ent);
	private:
		// ���C�L���X�g�̔�����s���ۂ̔��肪�s����(�n�`)�f�[�^�Q
		std::vector<mapping::rename_type::Entity> ent_terrains{};
		// �q�b�g�������R���W����(key : ���̎���ID�A value : �������G���e�B�e�B)
		std::unordered_map<mapping::rename_type::UUID, mapping::rename_type::Entity >hit_ent_sphere{};
		// �q�b�g�����J�v�Z���R���W����(key : �J�v�Z���̎���ID�A value : �������G���e�B�e�B)
		std::unordered_map<mapping::rename_type::UUID, mapping::rename_type::Entity> hit_ent_capsule{};

		/**
		 * @brief	: �����W���̎Z�o
		 * @return	: �����W��(0�`1)
		 */
		float CalculateRestitution(
			const component::PhysicMaterialComponent* physic_material_comp_1,
			const component::PhysicMaterialComponent* physic_material_comp_2);

		/**
		 * @brief : �Փ˂��������̒ǉ�
		 * @remark : ����ID��ǉ����悤�Ƃ����ꍇ�㏑�������
		 * @param sphere_id : ��ID
		 * @param ent : �ۗL��̃G���e�B�e�B
		 */
		void AddHitSphere(
			const mapping::rename_type::UUID& sphere_id,
			const mapping::rename_type::Entity& ent);

		/**
		 * @brief : �Փ˂����J�v�Z�����̒ǉ�
		 * @remark : ����ID��ǉ����悤�Ƃ����ꍇ�㏑�������
		 * @param capsule_id : ��ID
		 * @param ent : �ۗL��̃G���e�B�e�B
		 */
		void AddHitCapsule(
			const mapping::rename_type::UUID& capsule_id,
			const mapping::rename_type::Entity& ent);

		/**
		 * @brief : ���肵������HitResult�̍X�V
		 * @remark : result_1�̏��result_2�Ɋi�[����,
		 *			 result_2�̏��result_1�Ɋi�[�����
		 * @param sphere_1 : Sphere���(1)
		 * @param sphere_2 : Sphere���(2)
		 * @param result_1 : sphere_1��HitResult
		 * @param result_2 : sphere_2��HitResult
		 * @param ent_1 : result_1�̃G���e�B�e�B
		 * @param ent_2 : result_2�̃G���e�B�e�B
		 */
		void UpdateHitResult(
			const Sphere& sphere_1,
			const Sphere& sphere_2,
			HitResult& result_1,
			HitResult& result_2,
			const mapping::rename_type::UUID& ent_1,
			const mapping::rename_type::UUID& ent_2);

		/**
		 * @brief : ���肵������HitResult�̍X�V
		 * @remark : result_1�̏��result_2�Ɋi�[����,
		 *			 result_2�̏��result_1�Ɋi�[�����
		 * @param capsule_1 : Capsule���(1)
		 * @param capsule_2 : Capsule���(2)
		 * @param result_1 : sphere_1��HitResult
		 * @param result_2 : sphere_2��HitResult
		 * @param ent_1 : result_1�̃G���e�B�e�B
		 * @param ent_2 : result_2�̃G���e�B�e�B
		 */
		void UpdateHitResult(
			const Capsule& capsule_1,
			const Capsule& capsule_2,
			HitResult& result_1,
			HitResult& result_2,
			const mapping::rename_type::UUID& ent_1,
			const mapping::rename_type::UUID& ent_2);

		/**
		 * @brief : ���肵������HitResult�̍X�V
		 * @remark : sphere�̏��capsule�Ɋi�[����,
		 *			 capsule�̏��sphere�Ɋi�[�����
		 * @param sphere : Sphere���(1)
		 * @param capsule : Capsule���(2)
		 * @param s_result : sphere��HitResult
		 * @param c_result : capsule��HitResult
		 * @param s_ent : sphere�̃G���e�B�e�B
		 * @param c_ent : capsule�̃G���e�B�e�B
		 */
		void UpdateHitResult(
			const Sphere& sphere, const Capsule& capsule,
			HitResult& s_result, HitResult& c_result,
			const mapping::rename_type::UUID& s_ent,
			const mapping::rename_type::UUID& c_ent);

		/**
		 * @brief : ���o������
		 * @remark : ����V�[�����m
		 * @param dt : delta time
		 * @param registry : �R���|�[�l���g�̎擾�Ɏg�p
		 * @param ent_1	: ���o���ΏۃG���e�B�e�B(1)
		 * @param ent_2	: ���o���ΏۃG���e�B�e�B(2)
		 * @param mass_point_1 : ���_(1)
		 * @param mass_point_2 : ���_(2)
		 * @param collision_preset_1 : �R���W�����v���Z�b�g(1)
		 * @param collision_preset_2 : �R���W�����v���Z�b�g(2)
		 * @param penetration : �߂荞�݋
		 */
		void Extrude(
			float dt,
			ecs::Registry* registry,
			const mapping::rename_type::Entity& ent_1,
			const mapping::rename_type::Entity& ent_2,
			const DirectX::SimpleMath::Vector3& mass_point_1,
			const DirectX::SimpleMath::Vector3& mass_point_2,
			CollisionPreset collision_preset_1,
			CollisionPreset collision_preset_2,
			float penetration);
		/**
		 * @brief : ���o������
		 * @remark : �ʃV�[�����m
		 * @param dt : delta time
		 * @param registry_1 : �V�[��1�̃��W�X�g��
		 * @param registry_2 : �V�[��2�̃��W�X�g��
		 * @param ent_1	: ���o���ΏۃG���e�B�e�B(1)
		 * @param ent_2	: ���o���ΏۃG���e�B�e�B(2)
		 * @param mass_point_1 : ���_(1)
		 * @param mass_point_2 : ���_(2)
		 * @param collision_preset_1 : �R���W�����v���Z�b�g(1)
		 * @param collision_preset_2 : �R���W�����v���Z�b�g(2)
		 * @param penetration : �߂荞�݋
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
		 * @brief : ���C�ƃ��f���̏Փ˔���
		 */
		bool IntersectRayVsModel(
			float dt,
			ecs::Registry* registry,
			const component::ModelComponent& model,
			component::RayCastComponent& ray_cast_comp);

		/**
		 * @brief : ���Ƌ��̓����蔻��
		 * @remark : ����V�[�����m�̔��菈��
		 */
		bool IntersectSphereVsSphere(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent& sphere_1,
			component::SphereCollisionComponent& sphere_2);
		/**
		 * @brief : ���Ƌ��̓����蔻��
		 * @remark : �ʃV�[�����m�̔��菈��
		 */
		bool IntersectSphereVsSphere(
			float dt,
			ecs::Registry* registry_1, ecs::Registry* registry_2,
			component::SphereCollisionComponent& sphere_1,
			component::SphereCollisionComponent& sphere_2);

		/**
		 * @brief : �J�v�Z���ƃJ�v�Z���̓����蔻��
		 * @remark : ����̃V�[�����m�̔��菈��
		 */
		bool IntersectCapsuleVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::CapsuleCollisionComponent& capsule_1,
			component::CapsuleCollisionComponent& capsule_2);
		/**
		 * @brief : �J�v�Z���ƃJ�v�Z���̓����蔻��
		 * @remark : �ʃV�[�����m�̔��菈��
		 */
		bool IntersectCapsuleVsCapsule(
			float dt,
			ecs::Registry* registry_1, ecs::Registry* registry_2,
			component::CapsuleCollisionComponent& capsule_1,
			component::CapsuleCollisionComponent& capsule_2);

		/**
		 * @brief : ���ƃJ�v�Z���̓����蔻��
		 * @remark : �����G���e�B�e�B���ł̋��ƃJ�v�Z���̔���͍s��Ȃ�
		 * @remark : ����V�[�����m�̔���
		 */
		bool IntersectSphereVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent& sphere,
			component::CapsuleCollisionComponent& capsule);
		/**
		 * @brief : ���ƃJ�v�Z���̓����蔻��
		 * @remark : �����G���e�B�e�B���ł̋��ƃJ�v�Z���̔���͍s��Ȃ�
		 * @remark : �ʃV�[�����m�̔���
		 */
		bool IntersectSphereVsCapsule(
			float dt,
			ecs::Registry* registry_1, ecs::Registry* registry_2,
			component::SphereCollisionComponent& sphere,
			component::CapsuleCollisionComponent& capsule);
	};

} // cumulonimbus::collision