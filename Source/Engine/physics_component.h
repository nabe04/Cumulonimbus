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
		 * @brief		 : 力の加算
		 * @param force  : 力を加えたい方向
		 */
		void AddForce(const DirectX::SimpleMath::Vector3& force);

	private:
		DirectX::SimpleMath::Vector3 velocity{};		// 速度[m/s]

		DirectX::SimpleMath::Vector3 force{};			// 力の合力
		DirectX::SimpleMath::Vector3 acceleration{};	// 加速度[m/s^2]

		float mass = 1;	// 物体の質量[kg]

		/**
		 * @brief : 力の合力(force)と加速度(acceleration)を元に位置を算出
		 */
		void Integrate(float dt);
	};
} // cumulonimbus::component
