#include "child_actor.h"

#include <cassert>

#include "fbx_model_component.h"
#include "scene.h"
#include "transform.h"
#include "transform_component.h"

using namespace DirectX;

ChildActorComponent::ChildActorComponent(Entity* entity, EntityTag tag, std::string node_name)
	:MeshObject{ entity }
	, parent_tag{ tag }
	, tag_node_name{ node_name }
{
}

void ChildActorComponent::NewFrame(const float delta_time)
{
}

void ChildActorComponent::Update(const float delta_time)
{
	Link(tag_node_name);

	SimpleMath::Matrix ent_matrix = GetEntity()->GetComponent<TransformComponent>()->GetWorld4x4();
	ent_matrix *= parent_matrix;
	GetEntity()->GetComponent<TransformComponent>()->SetWorld4x4(ent_matrix);
}

void ChildActorComponent::RenderImGui()
{
}

void ChildActorComponent::Link(std::string node_name)
{
	const auto parent = GetEntity()->GetScene()->GetOtherEntity(parent_tag.GetCurrentState());

	if (!parent)
		assert(!"Not fount entity");

	for (const auto& node : parent->GetComponent<FbxModelComponent>()->GetNodes())
	{
		if (strcmp(node_name.c_str(), node.name) == 0)
		{
			parent_matrix = node.world_transform;
		}
	}
}

void ChildActorComponent::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		cereal::base_class<MeshObject>(this),
		CEREAL_NVP(parent_tag),
		CEREAL_NVP(parent_matrix),
		CEREAL_NVP(tag_node_name)
	);
}

void ChildActorComponent::Load(Entity* entity, std::string file_path_and_name)
{
	std::ifstream ifs(file_path_and_name);
	cereal::JSONInputArchive i_archive(ifs);
	i_archive(
		cereal::base_class<MeshObject>(this),
		CEREAL_NVP(parent_tag),
		CEREAL_NVP(parent_matrix),
		CEREAL_NVP(tag_node_name)
	);

	this->entity = entity;
}

