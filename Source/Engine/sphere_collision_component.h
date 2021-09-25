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
			DirectX::SimpleMath::Matrix  world_transform_matrix{ DirectX::SimpleMath::Matrix::Identity }; // ワールド変換行列
			DirectX::SimpleMath::Vector3 offset{};		// 球の位置調節値
			std::string					 bone_name{""};	// FBXモデルのボーンの位置名
			float						 radius{ 1.f };	// 半径
			HitResult				     hit_result{};	// ヒット結果
			CollisionPreset				 collision_preset{CollisionPreset::OverlapAll};	// コリジョンのプロファイル
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
			 * @brief		 : 球の追加
			 * @param name	 : 球名(std::unordered_mapのキー値として使用)
			 * @param sphere : 判定用の球データ
			 * @attention	 : nameを「""」とした場合、名前が「sphere(番号)」になる
			 */
			std::string AddSphere(const std::string& name = "", const collision::Sphere& sphere = {});

			/**
			 * @brief				: 球追加、位置をFBXモデルのボーンにアタッチ
			 * @param bone_name		: 関連付けたいFBXモデルのボーン名
			 * @param sphere_name	: 球名(std::unordered_mapのキー値として使用)
			 * @param sphere		: 判定用の球データ
			 * @attention			: nameを「""」とした場合、名前が「sphere(番号)」になる
			 */
			std::string AddSphereAndRegisterBoneName(
					const std::string&		 bone_name,
					const std::string&	     sphere_name = "",
					const collision::Sphere& sphere = {});

			/**
			 * @brief				: 個々のsphereの"offset"のセット
			 * @param sphere_name	: spheresに登録されているキー値
			 */
			void SetOffset(const std::string& sphere_name, const DirectX::SimpleMath::Vector3& offset);
			/**
			 * @brief : spheres全ての"offset"のセット
			 */
			void SetAllOffset(const DirectX::SimpleMath::Vector3& offset);

			/**
			 * @brief				: 個々のsphere1の"radius"のセット
			 * @param sphere_name	: spheresに登録されているキー値
			 * @param radius		: 半径
			 */
			void SetRadius(const std::string& sphere_name, float radius);
			/**
			 * @brief				: spheres全ての"offset"のセット
			 * @param radius		: 半径
			 */
			void SetAllRadius(float radius);

			/**
			 * @brief				: 個々のsphereの"collision_preset"のセット
			 * @param sphere_name	: spheresに登録されているキー値
			 * @param preset		: コリジョンのプロファイル
			 */
			void SetCollisionPreset(const std::string& sphere_name, collision::CollisionPreset preset);
			/**
			 * @brief		 : spheres全ての"collision_preset"のセット
			 * @param preset : コリジョンのプロファイル
			 */
			void SetAllCollisionPreset(collision::CollisionPreset preset);

			/**
			 * @brief : mapに登録されているcollision::Sphereの取得
			 */
			[[nodiscard]] std::unordered_map<std::string, collision::Sphere>& GetSpheres();
		private:
			// 判定用データ群
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