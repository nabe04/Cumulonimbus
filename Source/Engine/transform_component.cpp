#include "transform_component.h"

#include <fstream>
#include <string>
#include <imgui.h>
#include <cereal/archives/json.hpp>

#include "transform.h"
#include "cereal_helper.h"

using namespace DirectX;

TransformComponent::TransformComponent(Entity* entity, const XMFLOAT3& pos)
	: Component{ entity }
{
	component_name = "Transform";
	transform = std::make_unique<Transform>(pos);
}

void TransformComponent::NewFrame(const float delta_time)
{
	transform->SetOldPosition(transform->GetPosition());
}

void TransformComponent::Update(const float delta_time)
{
	transform->NormalizeAngle();
	transform->CreateWorldTransformMatrix();
}

void TransformComponent::RenderImGui()
{
	transform->RenderImgui();
}

void TransformComponent::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		CEREAL_NVP(component_name),
		CEREAL_NVP(transform)
	);
}

void TransformComponent::Load(Entity* entity,std::string file_path_and_name)
{
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(
			CEREAL_NVP(component_name),
			CEREAL_NVP(transform)
		);

		this->entity = entity;
	}
}

