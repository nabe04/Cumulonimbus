#include "child_actor.h"

#include <assert.h>

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

	SimpleMath::Matrix ent_matrix = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();
	ent_matrix *= parent_matrix;
	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->SetWorld4x4(ent_matrix);
}

void ChildActorComponent::RenderImGui()
{
}

void ChildActorComponent::Link(std::string node_name)
{
	auto pareant_tag = GetEntity()->GetScene()->GetOtherEntity(parent_tag);

	if (!pareant_tag)
		assert(!"Not fount entity");

	auto tag_nodes = pareant_tag->GetComponent<FbxModelComponent>()->GetNodes();

	for (auto node : tag_nodes)
	{
		if (strcmp(node_name.c_str(), node.name) == 0)
		{
			parent_matrix = node.world_transform;
		}
	}
}