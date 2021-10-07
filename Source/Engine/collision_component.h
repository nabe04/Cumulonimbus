#pragma once
#include <DirectXMath.h>

#include <cereal/cereal.hpp>

#include "component_base.h"
#include "cereal_helper.h"

namespace cumulonimbus::component
{
	class SphereCollisionComponent;
} // cumulonimbus::component


struct OriginCollisionData
{
	// Empty Data
};

/*
* brief : Shape of the hit decision
*/
enum class CollisionType
{
	Sphere,
	InnerSphere,
};

/*
* brief : Tag for judging a hit
*/
enum class CollisionTag
{
	Object,
	Player,
	Enemy,
	Camera,
};

namespace cumulonimbus::collision
{
	/**
	 * @brief : �R���W�����̃v���t�@�C��
	 */
	enum class CollisionPreset
	{
		BlockAll,		// ���ׂẴA�N�^���u���b�N����(���݂��̃R���W������BlockAll���Ɍ���)(RigidBodyComponent�������̂Ɍ���)
		OverlapAll,		// ���ׂẴA�N�g���I�[�o�[���b�v����
		NoCollision,	// �R���W�����Ȃ�

		End
	};

	/**
	 * @brief : �R���W�����ɔ�������q�b�g�C�x���g
	 */
	enum class HitEvent
	{
		OnCollisionEnter,	// ����Collision�ɐG�ꂽ�Ƃ�
		OnCollisionExit,	// ����Collision�ɐG���̂���߂��Ƃ�
		OnCollisionStay,	// ����Collision�ɐG��Ă����
		None,				// ���̂ǂ�Collision�ɂ��G��Ă��Ȃ���

		End
	};

	/**
	 * @brief : �q�b�g����
	 */
	struct HitResult
	{
		DirectX::XMFLOAT3	position		= { 0.0f,0.0f,0.0f };	// �q�b�g�ʒu
		DirectX::XMFLOAT3	normal			= { 0.0f,0.0f,0.0f };	// �q�b�g�ʒu�̖@��
		float				distance		= 0.0f;
		int					material_index	= -1;
		bool				is_hit			= false;				// �q�b�g���
		bool				is_old_hit		= false;				// 1�t���[���O��"is_hit"�l
		HitEvent			hit_event;								// �q�b�g�C�x���g

		template <class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(position),
				CEREAL_NVP(normal),
				CEREAL_NVP(distance),
				CEREAL_NVP(material_index),
				CEREAL_NVP(is_hit),
				CEREAL_NVP(is_old_hit),
				CEREAL_NVP(hit_event)
			);
		}
	};
} // cumulonimbus::collision

namespace cumulonimbus::component
{
	class CollisionComponent : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CollisionComponent& copy_comp); // for prefab
		explicit CollisionComponent()  = default; // for cereal
		~CollisionComponent() override = default;

		void PreGameUpdate(float dt)   override {}
		void GameUpdate(float dt)     override {}
		void PostGameUpdate(float dt) override {}
		void RenderImGui()			 override {}

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void serialize(Archive&& archive);

		void SetCollisionTag(CollisionTag tag);
		[[nodiscard]] CollisionTag GetCollisionTag() const;
	protected:
		CollisionTag		 collision_tag;
	};
} // cumulonimbus::component

