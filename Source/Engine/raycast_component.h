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
			HitResult  hit_result{};	// �q�b�g����
			utility::TerrainAttribute terrain_attribute{};	// ���肵���n�`�̑�������
			bool is_block = true; // ���肪�s��ꂽ�Ƃ��Ƀ��f���ƒn�`�Ƃ��u���b�N���邩
		};
	} // collision

	namespace component
	{
		class RayCastComponent final : public CollisionComponent
		{
		public:
			explicit RayCastComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
			explicit RayCastComponent() = default; // for cereal
			~RayCastComponent() override = default;

			void NewFrame(float dt) override {}
			void Update(float dt)   override {}
			void RenderImGui()		override;

			void Load(const std::string& file_path_and_name) override {}
			void Save(const std::string& file_path) override {}

			void AddRay(const std::string& ray_name, const collision::Ray& ray);

			[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayStartPos(const std::string& ray_name)	const;
			[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayEndPos(const std::string& ray_name)		const;
			[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayOffset(const std::string& ray_name)		const;
			[[nodiscard]] utility::TerrainAttribute GetTerrainAttribute(const std::string& ray_name)		const;
			[[nodiscard]] std::unordered_map< std::string, collision::Ray>& GetRays();
			

			void SetRayStartPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos);
			void SetRayEndPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos);
			void SetRayOffset(const std::string& ray_name, const DirectX::SimpleMath::Vector3& offset);
			void SetTerrainAttribute(const std::string& ray_name, utility::TerrainAttribute attribute);

		private:
			std::unordered_map< std::string, collision::Ray> rays;
		};
	} // component
} // cumulonimbus