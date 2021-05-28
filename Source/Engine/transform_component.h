#pragma once

#include <memory>

#include <DirectXMath.h>

#include <cereal/cereal.hpp>

#include "ecs.h"

class Transform;

class TransformComponent :public Component
{
public:
	explicit TransformComponent(Entity* entity, const DirectX::XMFLOAT3& pos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });
	TransformComponent()  = default; // for cereal
	~TransformComponent() override = default;

	void NewFrame(const float delta_time)override;
	void Update(const float delta_time) override;
	void RenderImGui() override;

	[[nodiscard]] Transform* GetTransform() const { return transform.get(); }

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path) override;

	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			cereal::make_nvp("Component Name",component_name),
			cereal::make_nvp("Transform"	 ,transform)
		);
	}

private:
	std::unique_ptr<Transform> transform;
};

CEREAL_REGISTER_TYPE(TransformComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, TransformComponent)