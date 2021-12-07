#pragma once
#include <functional>

#include <DirectXMath.h>

#include <cereal/cereal.hpp>

#include "component_base.h"
#include "cereal_helper.h"
#include "constant_buffer.h"
#include "debug_collision.h"
#include "event.h"

namespace cumulonimbus::component
{
	class SphereCollisionComponent;
} // cumulonimbus::component

///*
//* brief : Shape of the hit decision
//*/
//enum class CollisionType
//{
//	Sphere,
//	InnerSphere,
//};
//

namespace cumulonimbus::collision
{
	/*
	 * brief : Tag for judging a hit
	 */
	enum class CollisionTag
	{
		Object,
		Player,
		Sword,
		Enemy,
		Camera,
		Player_Weapon,
		Enemy_Weapon,

		End
	};

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
		DirectX::XMFLOAT3			position		{ 0.0f,0.0f,0.0f };	// �q�b�g�ʒu
		DirectX::XMFLOAT3			normal			{ 0.0f,0.0f,0.0f };	// �q�b�g�ʒu�̖@��
		float						distance		{0.0f};
		int							material_index	{ -1 };
		bool						is_hit			{ false };	// �q�b�g���
		bool						is_old_hit		{ false };	// 1�t���[���O��"is_hit"�l
		HitEvent					hit_event		{};	// �q�b�g�C�x���g
		collision::CollisionTag     collision_tag   {}; // ���肳�ꂽ�R���W�����^�O
		ecs::Registry*				registry		{};	// ���肳�ꂽ���W�X�g��
		mapping::rename_type::UUID  entity			{};	// ���肳�ꂽ�G���e�B�e�B
		mapping::rename_type::UUID	collision_id	{}; // ���肳�ꂽ�R���W����ID

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
		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, collision::CollisionTag tag);
		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CollisionComponent& copy_comp); // for prefab
		explicit CollisionComponent()  = default; // for cereal
		CollisionComponent(const CollisionComponent& other);
		CollisionComponent& operator=(const CollisionComponent& other);
		~CollisionComponent() override = default;

		void PreGameUpdate(float dt)   override {}
		void GameUpdate(float dt)     override {}
		void PostGameUpdate(float dt) override {}
		void RenderImGui()			 override {}

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		[[nodiscard]]
		buffer::ConstantBuffer<DebugCollisionCB>& GetCBuffer() const
		{
			return *cbuffer.get();
		}

		/**
		 * @brief : ����Collision�ɐG�ꂽ�Ƃ��̃C�x���g�o�^
		 */
		void RegisterEventEnter(const mapping::rename_type::UUID& id,const std::function<void(const collision::HitResult&)>& func);

		/**
		 * @brief : ����Collision�ɐG���̂���߂��Ƃ��̃C�x���g�o�^
		 */
		void RegisterEventExit(const mapping::rename_type::UUID& id, const std::function<void(const collision::HitResult&)>& func);

		/**
		 * @brief : ����Collision�ɐG��Ă���Ԃ̃C�x���g�o�^
		 */
		void RegisterEventStay(const mapping::rename_type::UUID& id, const std::function<void(const collision::HitResult&)>& func);

		/**
		 * @brief : ���̂ǂ�Collision�ɂ��G��Ă��Ȃ��Ԃ̃C�x���g�o�^
		 */
		void RegisterEventNone(const mapping::rename_type::UUID& id, const std::function<void(const collision::HitResult&)>& func);

	protected:
		std::unique_ptr<buffer::ConstantBuffer<DebugCollisionCB> >cbuffer{};
		// ����Collision�ɐG�ꂽ�Ƃ��̃C�x���g
		system::Event<void, const collision::HitResult&> on_collision_enter_event;
		// ����Collision�ɐG���̂���߂��Ƃ��̃C�x���g
		system::Event<void, const collision::HitResult&> on_collision_exit_event;
		// ����Collision�ɐG��Ă���Ԃ̃C�x���g
		system::Event<void, const collision::HitResult&> on_collision_stay_event;
		// ���̂ǂ�Collision�ɂ��G��Ă��Ȃ��Ԃ̃C�x���g
		system::Event<void, const collision::HitResult&> on_collision_none;
		// GUI��Ō��ݑI������Ă���R���W�����̖��O
		std::string selected_collision_name{};
		collision::CollisionTag tag{};

		/**
		 * @brief : ModelComponent��ێ����Ă����ꍇ�̃{�[���ʒu�̕ύX
		 * @remark : ImGui�`�揈�����Ŏg�p����
		 */
		void AttachSocket(std::string& socket_name);
	};
} // cumulonimbus::component

