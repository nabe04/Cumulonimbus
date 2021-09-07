#pragma once
#include <unordered_map>
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "collision_component.h"
#include "material_discrimination.h"


namespace cumulonimbus
{
	namespace collision
	{
		struct Ray
		{
			DirectX::SimpleMath::Vector3 ray_start;		// レイの始点
			DirectX::SimpleMath::Vector3 ray_end;		// レイの終点
			DirectX::SimpleMath::Vector3 ray_offset;	// レイキャストを行う位置のオフセット値(TransformComponentと合わせて使用する)
			DirectX::SimpleMath::Vector3 block_pos;		// is_blockがtrue時の地形とのブロック位置
			HitResult  hit_result{};	// ヒット結果
			utility::TerrainAttribute terrain_attribute{};	// 判定した地形の属性判別
			bool is_block		= true;		// 判定が行われたときにモデルと地形とをブロックするか
			bool is_block_hit	= false;	// block_posと地形とが当たったとき
		};
	} // collision

	namespace component
	{
		class RayCastComponent final : public CollisionComponent
		{
		public:
			using CollisionComponent::CollisionComponent;
			explicit RayCastComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
			explicit RayCastComponent() = default; // for cereal
			~RayCastComponent() override = default;

			void NewFrame(float dt) override {}
			void Update(float dt)   override {}
			void RenderImGui()		override;

			void Load(ecs::Registry* registry) override {}

			void AddRay(const std::string& ray_name, const collision::Ray& ray);

			[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayStartPos(const std::string& ray_name) const;
			[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayEndPos(const std::string& ray_name)	const;
			[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayOffset(const std::string& ray_name)	const;
			[[nodiscard]] const DirectX::SimpleMath::Vector3& GetBlockPos(const std::string& ray_name) const;
			[[nodiscard]] utility::TerrainAttribute GetTerrainAttribute(const std::string& ray_name) const;
			[[nodiscard]] bool GetIsBlock(const std::string& ray_name) const;
			[[nodiscard]] bool GetIsBlockHit(const std::string& ray_name) const;
			[[nodiscard]] std::unordered_map< std::string, collision::Ray>& GetRays();

			void SetRayStartPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos);
			void SetRayEndPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos);
			void SetRayOffset(const std::string& ray_name, const DirectX::SimpleMath::Vector3& offset);
			void SetBlockPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos);
			void SetTerrainAttribute(const std::string& ray_name, utility::TerrainAttribute attribute);
			void SetIsBlock(const std::string& ray_name, bool flg);
			void SetIsBlockHit(const std::string& ray_name, bool flg);

		private:
			std::unordered_map< std::string, collision::Ray> rays;
		};
	} // component
} // cumulonimbus