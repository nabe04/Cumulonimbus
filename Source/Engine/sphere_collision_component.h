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
			DirectX::SimpleMath::Matrix  world_matrix{ DirectX::SimpleMath::Matrix::Identity };		  // ワールド行列
			DirectX::SimpleMath::Vector3 offset{};		 // 球の位置調節値
			std::string bone_name{""};					 // FBXモデルのボーンの位置名
			float		radius{ 1.f };					 // 半径
			bool		is_hit{ false };				 // 判定フラグ
		};
	} // collision

	namespace component
	{
		class SphereCollisionComponent final : public CollisionComponent
		{
		public:
			explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
			explicit SphereCollisionComponent()  = default; // for cereal
			~SphereCollisionComponent() override = default;

			void NewFrame(float dt)   override;
			void Update(float dt)     override;
			void PostUpdate(float dt) override;
			void RenderImGui()	 	  override;

			void Save(const std::string& file_path)			 override;
			void Load(const std::string& file_path_and_name) override;

			/*
			 * brief	 : 球の追加
			 * name		 : 球名(std::unordered_mapのキー値として使用)
			 * sphere	 : 判定用の球データ
			 * ※caution : nameを「""」とした場合、名前が「sphere(番号)」になる
			 */
			void AddSphere(std::string& name, const collision::Sphere& sphere = {});

			/*
			 * brief	   : 球の位置をFBXモデルのボーンの位置追加
			 * bone_name   : 関連付けたいFBXモデルのボーン名
			 * sphere_name : 球名(std::unordered_mapのキー値として使用)
			 * sphere	   : 判定用の球データ
			 * ※caution   : nameを「""」とした場合、名前が「sphere(番号)」になる
			 */
			void AddSphereAndRegisterBoneName(
					const std::string& bone_name,
					std::string&	   sphere_name,
					const collision::Sphere& sphere = {});

			/*
			 * brief		: 個々の"offset"のセット
			 * sphere_name	: spheresに登録されているキー値
			 */
			void SetOffset(const std::string& sphere_name, const DirectX::SimpleMath::Vector3& offset);
			/*
			 * brief : spheres全ての"offset"のセット
			 */
			void SetAllOffset(const DirectX::SimpleMath::Vector3& offset);
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