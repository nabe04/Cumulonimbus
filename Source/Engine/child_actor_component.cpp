//#include "child_actor_component.h"
//
//#include <cassert>
//
//#include "fbx_model_component.h"
//#include "scene.h"
//#include "transform_component.h"
//
//using namespace DirectX;
//
//namespace cumulonimbus::component
//{
//	ChildActorComponent::ChildActorComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, EntityTag tag, std::string node_name)
//		:MeshObjectComponent{ registry , ent }
//		, tag_node_name{ node_name }
//	{
//	}
//
//	void ChildActorComponent::PreGameUpdate(const float delta_time)
//	{
//	}
//
//	void ChildActorComponent::GameUpdate(const float delta_time)
//	{
//		Link(tag_node_name);
//		SimpleMath::Matrix ent_matrix = GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).GetWorldMatrix();
//		ent_matrix *= parent_matrix;
//		GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).SetWorldMatrix(ent_matrix);
//	}
//
//	void ChildActorComponent::RenderImGui()
//	{
//	}
//
//	void ChildActorComponent::Link(std::string node_name)
//	{
//		//const auto parent = GetRegistry()->GetScene()->GetOtherEntity(parent_tag.GetCurrentState());
//
//		//if (!parent)
//		//	assert(!"Not fount entity");
//
//		//for (const auto& node : parent->GetComponent<FbxModelComponent>()->GetNodes())
//		//{
//		//	if (node_name == node.name)
//		//	{
//		//		parent_matrix = node.world_transform;
//		//	}
//		//}
//	}
//
//	void ChildActorComponent::Load(ecs::Registry* registry)
//	{
//		SetRegistry(registry);
//	}
//
//} // cumulonimbus::component
