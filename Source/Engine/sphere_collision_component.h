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
			DirectX::SimpleMath::Matrix  world_transform_matrix{ DirectX::SimpleMath::Matrix::Identity }; // ���[���h�ϊ��s��
			DirectX::SimpleMath::Vector3 offset{};		// ���̈ʒu���ߒl
			std::string					 bone_name{""};	// FBX���f���̃{�[���̈ʒu��
			float						 radius{ 1.f };	// ���a
			HitResult				     hit_result{};	// �q�b�g����
			CollisionPreset				 collision_preset{CollisionPreset::OverlapAll};	// �R���W�����̃v���t�@�C��
		};
	} // collision

	namespace component
	{
		class SphereCollisionComponent final : public CollisionComponent
		{
		public:
			using CollisionComponent::CollisionComponent;
			explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
			explicit SphereCollisionComponent()  = default; // for cereal
			~SphereCollisionComponent() override = default;

			void PreGameUpdate(float dt)   override;
			void GameUpdate(float dt)     override;
			void PostGameUpdate(float dt) override;
			void RenderImGui()	 	  override;

			void Load(ecs::Registry* registry) override;

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
			std::unordered_map<std::string, collision::Sphere> spheres;
		};
	} // component

} // cumulonimbus

CEREAL_REGISTER_TYPE(cumulonimbus::component::SphereCollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::CollisionComponent, cumulonimbus::component::SphereCollisionComponent)

//#include "transform.h"
//#include "geometric_primitive_resource.h"
//#include "component_base.h"
//
//struct SphereCollisionData
//{
//	DirectX::SimpleMath::Matrix  world_matrix;
//	DirectX::SimpleMath::Matrix  bone_matrix;
//	DirectX::SimpleMath::Vector3 ajust_pos;
//	XMFLOAT3		color_RGB;
//	float			radius;
//	bool			isHit = false;
//};
//
//namespace cumulonimbus::component
//{
//	class SphereCollisionComponent : public CollisionComponent
//	{
//	private:
//		std::vector<SphereCollisionData> sphere_datas;
//
//		//-- Debug primitive info --//
//		geometric_primitive_resource::Mesh mesh;
//
//	private:
//		bool IntersectSphere(component::SphereCollisionComponent* other) override;
//
//	public:
//		explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
//		explicit SphereCollisionComponent() = default;
//		~SphereCollisionComponent() = default;
//
//		void Update(const float delta_time)override;
//
//		void OnCollision(const CollisionComponent* other) override;
//
//		void AddSphere(XMFLOAT3 position, float radius);
//
//		/*
//		* brief : Set all radius
//		*/
//		void SetRadiusAll(const float radius);
//
//		/*
//		* brief : Set the radius by specifying an index number
//		*/
//		void SetRadius(const float radius, const int index);
//
//		const geometric_primitive_resource::Mesh& GetMesh() { return mesh; }
//		std::vector<SphereCollisionData>& GetSphereData() { return sphere_datas; }
//
//		// For ImGui
//		void RenderImGui() override;
//	};
//}
//
//
//CEREAL_REGISTER_TYPE(cumulonimbus::component::SphereCollisionComponent)
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::CollisionComponent, cumulonimbus::component::SphereCollisionComponent)