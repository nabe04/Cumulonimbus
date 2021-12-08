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
		struct Sphere
		{
			bool						 is_visible{ true };
			bool						 hidden_in_game{ true };
			float						 radius{ 1.f };	// ���a
			mapping::rename_type::UUID   id{}; // �����ʗpID(���j�[�N�Ȃ���)
			std::string					 bone_name{};	// FBX���f���̃{�[���̈ʒu��
			CollisionPreset				 collision_preset{CollisionPreset::OverlapAll};	// �R���W�����̃v���t�@�C��
			CollisionTag				 collision_tag{ CollisionTag::Object };
			DirectX::SimpleMath::Matrix  world_transform_matrix{ DirectX::SimpleMath::Matrix::Identity }; // ���[���h�ϊ��s��
			DirectX::SimpleMath::Vector3 offset{};		// ���̈ʒu���ߒl
			DirectX::SimpleMath::Vector4 base_color{ 1.f,1.f,1.f,1.f };
			DirectX::SimpleMath::Vector4 hit_color{ 1.f,.0f,.0f,1.f };
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
		class SphereCollisionComponent final : public CollisionComponent
		{
		public:
			using CollisionComponent::CollisionComponent;
			explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, collision::CollisionTag tag);
			explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SphereCollisionComponent& copy_comp); // for prefab
			explicit SphereCollisionComponent()  = default; // for cereal
			~SphereCollisionComponent() override = default;

			void Start() override;

			void CommonUpdate(float dt) override;
			void PreGameUpdate(float dt)   override;
			void GameUpdate(float dt)     override;
			void PostGameUpdate(float dt) override;
			void RenderImGui()	 	  override;

			void Load(ecs::Registry* registry) override;

			template<class Archive>
			void load(Archive&& archive, uint32_t  version);

			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;

			/**
			 * @brief		 : ���̒ǉ�
			 * @param name	 : ����(std::unordered_map�̃L�[�l�Ƃ��Ďg�p)
			 * @param sphere : ����p�̋��f�[�^
			 * @attention	 : name���u""�v�Ƃ����ꍇ�A���O���usphere(�ԍ�)�v�ɂȂ�
			 */
			std::string AddSphere(const std::string& name = "", const collision::Sphere& sphere = {});

			/**
			 * @brief				: ���ǉ��A�ʒu��FBX���f���̃{�[���ɃA�^�b�`
			 * @param bone_name		: �֘A�t������FBX���f���̃{�[����
			 * @param sphere_name	: ����(std::unordered_map�̃L�[�l�Ƃ��Ďg�p)
			 * @param sphere		: ����p�̋��f�[�^
			 * @attention			: name���u""�v�Ƃ����ꍇ�A���O���usphere(�ԍ�)�v�ɂȂ�
			 */
			std::string AddSphereAndRegisterBoneName(
					const std::string&		 bone_name,
					const std::string&	     sphere_name = "",
					const collision::Sphere& sphere = {});

			/**
			 * @brief : sphere_id����ێ����Ă��鋅�R���W�������擾
			 * @remark : sphere_id����v���Ȃ��ꍇnullptr��Ԃ�
			 * @param sphere_id : �擾���������R���W�������
			 */
			collision::Sphere* TryGetSphere(const mapping::rename_type::UUID& sphere_id);

			/**
			 * @brief				: �X��sphere��"offset"�̃Z�b�g
			 * @param sphere_name	: spheres�ɓo�^����Ă���L�[�l
			 */
			void SetOffset(const std::string& sphere_name, const DirectX::SimpleMath::Vector3& offset);
			/**
			 * @brief : spheres�S�Ă�"offset"�̃Z�b�g
			 */
			void SetAllOffset(const DirectX::SimpleMath::Vector3& offset);

			/**
			 * @brief				: �X��sphere1��"radius"�̃Z�b�g
			 * @param sphere_name	: spheres�ɓo�^����Ă���L�[�l
			 * @param radius		: ���a
			 */
			void SetRadius(const std::string& sphere_name, float radius);
			/**
			 * @brief				: spheres�S�Ă�"offset"�̃Z�b�g
			 * @param radius		: ���a
			 */
			void SetAllRadius(float radius);

			/**
			 * @brief				: �X��sphere��"collision_preset"�̃Z�b�g
			 * @param sphere_name	: spheres�ɓo�^����Ă���L�[�l
			 * @param preset		: �R���W�����̃v���t�@�C��
			 */
			void SetCollisionPreset(const std::string& sphere_name, collision::CollisionPreset preset);
			/**
			 * @brief		 : spheres�S�Ă�"collision_preset"�̃Z�b�g
			 * @param preset : �R���W�����̃v���t�@�C��
			 */
			void SetAllCollisionPreset(collision::CollisionPreset preset);

			/**
			 * @brief : map�ɓo�^����Ă���collision::Sphere�̎擾
			 */
			[[nodiscard]] std::unordered_map<std::string, collision::Sphere>& GetSpheres();
		private:
			// ����p�f�[�^�Q
			std::unordered_map<std::string, collision::Sphere> spheres{};
		};
	} // component

} // cumulonimbus