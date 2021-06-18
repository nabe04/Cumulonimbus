#include "child_actor.h"

#include <cassert>

#include "fbx_model_component.h"
#include "scene.h"
#include "transform.h"
#include "transform_component.h"

using namespace DirectX;

namespace cumulonimbus::component
{
	ChildActorComponent::ChildActorComponent(ecs::Registry* registry, ecs::Entity ent, EntityTag tag, std::string node_name)
		:MeshObjectComponent{ registry , ent }
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

		SimpleMath::Matrix ent_matrix = GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).GetWorld4x4();
		ent_matrix *= parent_matrix;
		GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).SetWorld4x4(ent_matrix);
	}

	void ChildActorComponent::RenderImGui()
	{
	}

	void ChildActorComponent::Link(std::string node_name)
	{
		//const auto parent = GetRegistry()->GetScene()->GetOtherEntity(parent_tag.GetCurrentState());

		//if (!parent)
		//	assert(!"Not fount entity");

		//for (const auto& node : parent->GetComponent<FbxModelComponent>()->GetNodes())
		//{
		//	if (node_name == node.name)
		//	{
		//		parent_matrix = node.world_transform;
		//	}
		//}
	}

	void ChildActorComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + file_path_helper::GetTypeName<ChildActorComponent>() + file_path_helper::GetJsonExtension();
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(
			cereal::base_class<MeshObjectComponent>(this),
			CEREAL_NVP(parent_tag),
			CEREAL_NVP(parent_matrix),
			CEREAL_NVP(tag_node_name)
		);
	}

	void ChildActorComponent::Load(const std::string& file_path_and_name)
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(
			cereal::base_class<MeshObjectComponent>(this),
			CEREAL_NVP(parent_tag),
			CEREAL_NVP(parent_matrix),
			CEREAL_NVP(tag_node_name)
		);
	}

}
