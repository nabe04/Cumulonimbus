#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"

namespace cumulonimbus::component
{
	class PhysicsComponent final : public ComponentBase
	{
	public:
		explicit PhysicsComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PhysicsComponent()  = default; // for cereal
		~PhysicsComponent() override = default;

		void Update(float dt) override;
		void RenderImGui() override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		/**
		 * @brief		 : �͂̉��Z
		 * @param force  : �͂�������������
		 */
		void AddForce(const DirectX::SimpleMath::Vector3& force);

	private:
		DirectX::SimpleMath::Vector3 velocity{};		// ���x[m/s]

		DirectX::SimpleMath::Vector3 force{};			// �͂̍���
		DirectX::SimpleMath::Vector3 acceleration{};	// �����x[m/s^2]

		float mass = 1;	// ���̂̎���[kg]

		/**
		 * @brief : �͂̍���(force)�Ɖ����x(acceleration)�����Ɉʒu���Z�o
		 */
		void Integrate(float dt);
	};
} // cumulonimbus::component
