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
		 * @brief : CollisionComponent�����G���e�B�e�B�S�̂̓����蔻�菈��
		 */
		void Update(float dt, ecs::Registry* registry);

		void RenderImGui(ecs::Registry* registry);

		/**
		 * @brief : �n�`�Ƃ��Ĕ��肳���������̓o�^
		 *		   Scene��Initialize�֐����Ȃǂ�Component��ǉ������i�K
		 *		   �ɓo�^����
		 */
		void RegistryRayCastModel(mapping::rename_type::Entity ent);
	private:
		// ���C�L���X�g�̔�����s���ۂ̔��肪�s����(�n�`)�f�[�^�Q
		std::vector<mapping::rename_type::Entity> ent_terrains{};


		/**
		 * @brief	: �����W���̎Z�o
		 * @return	: �����W��(0�`1)
		 */
		float CalculateRestitution(
			const component::PhysicMaterialComponent* physic_material_comp_1,
			const component::PhysicMaterialComponent* physic_material_comp_2);

		/**
		 * @brief						: ���o������
		 * @param dt					: delta time
		 * @param registry				: �R���|�[�l���g�̎擾�Ɏg�p
		 * @param ent_1					: ���o���ΏۃG���e�B�e�B(1)
		 * @param ent_2					: ���o���ΏۃG���e�B�e�B(2)
		 * @param mass_point_1			: ���_(1)
		 * @param mass_point_2			: ���_(2)
		 * @param collision_preset_1	: �R���W�����v���Z�b�g(1)
		 * @param collision_preset_2	: �R���W�����v���Z�b�g(2)
		 * @param penetration			: �߂荞�݋
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
		 * @brief : ���C�ƃ��f���̏Փ˔���
		 */
		bool IntersectRayVsModel(
			float dt,
			ecs::Registry* registry,
			const component::FbxModelComponent& model,
			component::RayCastComponent& ray_cast_comp
		);

		/**
		 * @brief : ���Ƌ��̓����蔻��
		 *	        SphereCollisionComponent������Sphere����������
		 */
		bool IntersectSphereVsSphere(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent& sphere_1,
			component::SphereCollisionComponent& sphere_2
		);

		/**
		 * @brief : �J�v�Z���ƃJ�v�Z���̓����蔻��
		 */
		bool IntersectCapsuleVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::CapsuleCollisionComponent& capsule_1,
			component::CapsuleCollisionComponent& capsule_2
		);

		/**
		 * @brief     : ���ƃJ�v�Z���̓����蔻��
		 * @attention : �����G���e�B�e�B���ł̋��ƃJ�v�Z���̔���͍s��Ȃ�
		 */
		bool IntersectSphereVsCapsule(
			float dt,
			ecs::Registry* registry,
			component::SphereCollisionComponent&  sphere,
			component::CapsuleCollisionComponent& capsule
		);
	};

} // cumulonimbus::collision