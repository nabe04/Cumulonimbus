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
			DirectX::SimpleMath::Vector3 ray_start;		// ���C�̎n�_
			DirectX::SimpleMath::Vector3 ray_end;		// ���C�̏I�_
			DirectX::SimpleMath::Vector3 ray_offset;	// ���C�L���X�g���s���ʒu�̃I�t�Z�b�g�l(TransformComponent�ƍ��킹�Ďg�p����)
			DirectX::SimpleMath::Vector3 block_pos;		// is_block��true���̒n�`�Ƃ̃u���b�N�ʒu
			HitResult  hit_result{};	// �q�b�g����
			utility::TerrainAttribute terrain_attribute{};	// ���肵���n�`�̑�������
			bool is_block		= true;		// ���肪�s��ꂽ�Ƃ��Ƀ��f���ƒn�`�Ƃ��u���b�N���邩
			bool is_block_hit	= false;	// block_pos�ƒn�`�Ƃ����������Ƃ�
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