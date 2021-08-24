#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "collision_component.h"
#include "material_discrimination.h"

namespace cumulonimbus::component
{
	class RayCastComponent final : public CollisionComponent
	{
	public:
		explicit RayCastComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
		explicit RayCastComponent()  = default; // for cereal
		~RayCastComponent() override = default;

		void NewFrame(float dt) override {}
		void Update(float dt)   override {}
		void RenderImGui()		override;

		void Load(const std::string& file_path_and_name) override {}
		void Save(const std::string& file_path) override {}

		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayStartPos()	const;
		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayEndPos()	const;
		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetRayOffset()	const;
		[[nodiscard]] utility::TerrainAttribute GetTerrainAttribute() const;

		void SetRayStartPos(const DirectX::SimpleMath::Vector3& pos);
		void SetRayEndPos(const DirectX::SimpleMath::Vector3& pos);
		void SetRayOffset(const DirectX::SimpleMath::Vector3& offset);
		void SetTerrainAttribute(utility::TerrainAttribute attribute);

	private:
		// ���C�L���X�g���s���ۂ̃��C�̎n�_�ƏI�_
		DirectX::SimpleMath::Vector3 ray_start;
		DirectX::SimpleMath::Vector3 ray_end;
		// ���C�L���X�g���s���ʒu�̃I�t�Z�b�g�l
		// (TransformComponent�ƍ��킹�Ďg�p����)
		DirectX::SimpleMath::Vector3 ray_offset;
		// ���肵���n�`�̑�������
		utility::TerrainAttribute terrain_attribute{};
	};
} // cumulonimbus::component