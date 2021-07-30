#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"

namespace cumulonimbus::component
{
	class MovementComponent final : public ComponentBase
	{
	public:
		explicit MovementComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent);
		explicit MovementComponent()  = default; // for cereal
		~MovementComponent() override = default;

		void Update(float dt) override;
		void RenderImGui() override;
		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		void AddForce(const DirectX::SimpleMath::Vector3& force);
		void Jump(float strength = -1);

		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetVelocity()     const { return velocity; }
		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetAcceleration() const { return acceleration; }
		[[nodiscard]] float GetJumpStrength()								const { return jump_strength; }
		[[nodiscard]] float GetGravity()									const { return gravity; }
		[[nodiscard]] float GetCurrentGravity()								const { return current_gravity; }
	private:
		DirectX::SimpleMath::Vector3 velocity{};
		DirectX::SimpleMath::Vector3 acceleration{};

		float jump_strength   = 500; // �W�����v���x
		float gravity		  = -20; // �d��
		float current_gravity = 0;

		bool is_gravity = true;	// �d�̓t���O
		bool is_landing;	// �n�ʃt���O
		bool is_jumping;	// �W�����v�t���O

		void Integrate(float dt);
	};
} // cumulonimbus::component