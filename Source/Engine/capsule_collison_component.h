#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <unordered_map>
#include <string>

#include "collision_component.h"

namespace cumulonimbus
{
	namespace collision
	{
		struct Capsule
		{
			bool						 is_enable{ true }; // ���菈�����s����
			bool						 is_visible{ true }; // �\�����邩
			bool						 hidden_in_game{ true }; // �Q�[�����ɕ\�����邩
			float						 length{ 2 };		 // �J�v�Z���̒[�_�Ԃ̒���
			float						 radius{ 1 };		 // �J�v�Z���̔��a
			std::string					 name{};
			std::string					 bone_name{};		 // FBX���f���̃{�[���̈ʒu��
			mapping::rename_type::UUID   id{};				 // �J�v�Z��ID(�Q�[���J�n���ɕς��)
			CollisionType				 collision_type{ CollisionType::OverlapAll }; // �R���W�����̃v���t�@�C��
			CollisionTag				 collision_tag{ CollisionTag::Object }; // ����p�^�O
			DirectX::SimpleMath::Matrix  world_transform_matrix{ DirectX::SimpleMath::Matrix::Identity }; // ���[���h�ϊ��s��
			DirectX::SimpleMath::Vector3 offset{};			// �J�v�Z���̈ʒu���ߒl
			DirectX::SimpleMath::Vector3 rotation{};		// �J�v�Z���̉�]���ߒl(�x���@)
			DirectX::SimpleMath::Vector3 start{ 0,-1,0 };	// �J�v�Z���̎n�_(�f�t�H���g�l : { 0, 1, 0})
			DirectX::SimpleMath::Vector3 end  { 0, 1,0 };	// �J�v�Z���̏I�_(�f�t�H���g�l : { 0,-1, 0})
			DirectX::SimpleMath::Vector4 base_color{ 1.f,1.f,1.f,1.f }; // �R���W�����̊�{�F(����O��)
			DirectX::SimpleMath::Vector4 hit_color{ 1.f,.0f,.0f,1.f };  // �R���W�����̔���F(���莞)
			std::unordered_map<mapping::rename_type::Entity, HitResult> hit_results{}; // �q�b�g����(�I�u�W�F�N�g��)

			template<class Archive>
			void load(Archive&& archive, uint32_t version);

			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;

			HitResult* TryGetHitResult(const mapping::rename_type::Entity& ent);
			/**
			 * @brief : ���肳�ꂽ�G���e�B�e�B�̓o�^
			 */
			void RegisterHitEntity(const mapping::rename_type::Entity& ent, const HitResult& hit_result);
			/**
			 * @brief : ���肳�ꂽ�G���e�B�e�B�̓o�^����
			 */
			void UnRegisterHitEntity(const mapping::rename_type::Entity& ent);
		};
	} // collision

	namespace component
	{
		class CapsuleCollisionComponent final : public CollisionComponent
		{
		public:
			using CollisionComponent::CollisionComponent;
			explicit CapsuleCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, collision::CollisionTag tag);
			explicit CapsuleCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CapsuleCollisionComponent& copy_comp); // for prefab
			explicit CapsuleCollisionComponent()  = default; // for cereal
			~CapsuleCollisionComponent() override = default;

			void Start() override;

			void CommonUpdate(float dt)		override;
			void PreGameUpdate(float dt)	override;
			void GameUpdate(float dt)		override;
			void PostGameUpdate(float dt)	override;
			void RenderImGui()				override;

			void Load(ecs::Registry* registry) override;

			template<class Archive>
			void load(Archive&& archive, uint32_t  version);

			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;

			/**
			 * @brief		 : �J�v�Z���̒ǉ�
			 * @param name	 : �J�v�Z����(std::unordered_map�̃L�[�l�Ƃ��Ďg�p)
			 * @capsule		 : ����p�J�v�Z���f�[�^
			 * @attencion	 : name���u""�v�Ƃ����ꍇ�A���O���ucapsule(�ԍ�)�v�ɂȂ�
			 *
			 */
			std::string AddCapsule(
				const std::string&		  name = "",
				const collision::Capsule& capsule = {});

			/**
			 * @brief : �J�v�Z���̒ǉ��A�ʒu��FBX���f���̃{�[���ɃA�^�b�`
			 * @param bone_name		: �֘A�t������FBX���f���̃{�[����
			 * @param capsule_name	: ����(std::unordered_map�̃L�[�l�Ƃ��Ďg�p)
			 * @param capsule		: ����p�̋��f�[�^
			 * @attention			: name���u""�v�Ƃ����ꍇ�A���O���ucapsule(�ԍ�)�v�ɂȂ�
			 */
			std::string AddAndRegisterBoneName(
				const std::string&		  bone_name,
				const std::string&		  capsule_name = "",
				const collision::Capsule& capsule = {});

			/**
			 * @brief : capsule_id����ێ����Ă���J�v�Z���R���W�������擾
			 * @remark : capsule_id����v���Ȃ��ꍇnullptr��Ԃ�
			 * @param capsule_id : �擾�������J�v�Z���R���W���������
			 */
			collision::Capsule* TryGetCapsuleFromId(const mapping::rename_type::UUID& capsule_id);
			/**
			 * @brief : �J�v�Z��������X�̃J�v�Z�����擾
			 * @remark : ���݂��Ȃ��ꍇ��nullptr��Ԃ�
			 */
			[[nodiscard]]
			collision::Capsule* TryGetCapsuleFromName(const std::string& c_name);
			/**
			 * @brief : �S�ẴJ�v�Z���̓����蔻��̍s�����̐ݒ�
			 */
			void SetAllCollisionEnable(bool is_enable);

			/**
			 * @brief				: �X�̃J�v�Z����"offset"�̃Z�b�g
			 * @param capsule_name	: capsule�ɓo�^����Ă���L�[�l
			 */
			void SetOffset(const std::string& capsule_name, const DirectX::SimpleMath::Vector3& offset);
			/**
			 * @brief : �S�ẴJ�v�Z����"offset"�̃Z�b�g
			 */
			void SetAllOffset(const DirectX::SimpleMath::Vector3& offset);

			/**
			 * @brief				: �X��capsule��"radius"�̃Z�b�g
			 * @param capsule_name	: capsule�ɓo�^����Ă���L�[�l
			 * @param radius		: ���a
			 */
			void SetRadius(const std::string& capsule_name, float radius);
			/**
			 * @brief		  : �S�Ă�capsule��"offset"�̃Z�b�g
			 * @param radius  : ���a
			 */
			void SetAllRadius(float radius);

			/**
			 * @brief				: �X��capsule��"length"�̃Z�b�g
			 * @param capsule_name	: capsule�ɓo�^����Ă���L�[�l
			 * @param length		: �J�v�Z�����̐����̒���
			 */
			void SetLength(const std::string& capsule_name, float length);
			/**
			 * @brief		  : �S�Ă�capsule��"length"�̃Z�b�g
			 * @param length  : �J�v�Z�����̐����̒���
			 */
			void SetAllLength(float length);

			/**
			 * @brief				: �X��capsule��"collision_type"�̃Z�b�g
			 * @param capsule_name	: capsules�ɓo�^����Ă���L�[�l
			 * @param preset		: �R���W�����̃v���t�@�C��
			 */
			void SetCollisionType(const std::string& capsule_name, collision::CollisionType preset);
			/**
			 * @brief : capsules�S�Ă�"collision_type"�̃Z�b�g
			 */
			void SetAllCollisionType(collision::CollisionType preset);

			/**
			* @brief : map�ɓo�^����Ă���collision::Capsule�̎擾
			*/
			[[nodiscard]]
			std::unordered_map<std::string, collision::Capsule>& GetCapsules();
		private:
			// ����p�f�[�^�Q
			std::unordered_map<std::string, collision::Capsule> capsules{};
		};
	} // component

} // cumulonimbus
