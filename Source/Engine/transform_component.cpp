#include "transform_component.h"

#include <fstream>
#include <string>
#include <imgui.h>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/bitset.hpp>
#include "cereal_helper.h"


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

// cereal
//template <class Archive>
//void Transform::serialize(Archive&& archive)
//{
//	archive(
//		CEREAL_NVP(position),
//		CEREAL_NVP(prev_pos),
//		CEREAL_NVP(scale),
//		CEREAL_NVP(angle),
//		CEREAL_NVP(prev_angle),
//
//		CEREAL_NVP(world_f4x4),
//		CEREAL_NVP(scaling_matrix),
//		CEREAL_NVP(rotation_matrix),
//		CEREAL_NVP(translation_matrix),
//
//		CEREAL_NVP(model_right),
//		CEREAL_NVP(model_front),
//		CEREAL_NVP(model_up),
//		CEREAL_NVP(orientation),
//
//		// Quaternion
//		CEREAL_NVP(axis),
//		CEREAL_NVP(local_quaternion),
//		CEREAL_NVP(world_quaternion),
//
//		CEREAL_NVP(set_angle_bit_flg),
//		CEREAL_NVP(is_billboard),
//		CEREAL_NVP(is_quaternion)
//
//	);
//}

