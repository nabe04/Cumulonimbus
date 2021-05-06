#pragma once

#include <memory>

#include <DirectXMath.h>

#include <cereal/cereal.hpp>

#include "ecs.h"

class Transform;

class TransformComponent :public Component
{
private:
	std::unique_ptr<Transform> transform;

public:
	TransformComponent(Entity* entity, const DirectX::XMFLOAT3& pos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });
	TransformComponent() = default; // for cereal
	~TransformComponent() = default;

	void NewFrame(const float delta_time)override;
	void Update(const float delta_time) override;
	void RenderImGui() override;

	Transform* GetTransform() { return transform.get(); }

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path) override;

	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(component_name),
			CEREAL_NVP(transform)
		);
	}
};