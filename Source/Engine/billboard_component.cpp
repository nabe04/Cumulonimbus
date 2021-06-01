#include "billboard_component.h"

#include "scene.h"
#include "transform.h"
#include "transform_component.h"
#include "geometric_primitive_component.h"

BillbordComponent::BillbordComponent(Entity* entity)
	:MeshObject{ entity }
{
	GetEntity()->AddComponent<GeomPrimComponent>(GeomPrimType::Cube);

	// Active billboard
	GetEntity()->GetComponent<TransformComponent>()->ActiveBillboard();
}

void BillbordComponent::Update(const float delta_time)
{
	auto* transform = GetEntity()->GetComponent<TransformComponent>();

	GetEntity()->GetComponent<TransformComponent>()->GetBillboardRotation(transform->GetPosition(), GetEntity()->GetScene()->GetView()->GetEyePosition());
}

void BillbordComponent::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		cereal::base_class<MeshObject>(this)
	);
}

void BillbordComponent::Load(Entity* entity, std::string file_path_and_name)
{
	std::ifstream ifs(file_path_and_name);
	cereal::JSONInputArchive i_archive(ifs);
	i_archive(
		cereal::base_class<MeshObject>(this)
	);

	this->entity = entity;
}
