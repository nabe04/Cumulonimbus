#include "geometric_primitive_component.h"

#include <GeometricPrimitive.h>

#include "scene.h"
#include "shader.h"
#include "texture.h"
#include "transform.h"
#include "dx11_configurator.h"
#include "light.h"
#include "view.h"
#include "component_list.h"

//*************************************************
//
//	GeomPrimComponent Class
//
//**************************************************
GeomPrimComponent::GeomPrimComponent(Entity* entity, GeomPrimType prim_type, const XMFLOAT3& pos)
	: Component{ entity }
{
	// Set position
	GetEntity()->GetComponent<TransformComponent>()->SetPosition(pos);

	// Create Geometric Primitive
	mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(prim_type);

}

void GeomPrimComponent::NewFrame(const float delta_time)
{

}

void GeomPrimComponent::Update(const float delta_time)
{
	auto* view = GetEntity()->GetScene()->GetView();
}

void GeomPrimComponent::RenderImGui()
{
	if (ImGui::TreeNode("GeomPrim Types"))
	{
		static int old_type = item_current;

		item_current = static_cast<int>(geom_prim_type); // To correspond to the Shader that the current Component has
		ImGui::Combo("Shader Type", &item_current, items, IM_ARRAYSIZE(items));

		geom_prim_type = static_cast<GeomPrimType>(item_current);
		mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(static_cast<GeomPrimType>(geom_prim_type));

		material.EditParam();

		ImGui::TreePop();
	}
}

void GeomPrimComponent::SwitchMesh(const GeomPrimType& type)
{
	mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(static_cast<GeomPrimType>(type));
}


namespace cumulonimbus::component
{
	//GeomPrimComponent::GeomPrimComponent(ecs::Registry* const registry, const ecs::Entity ent,
	//									 GeomPrimType prim_type, const XMFLOAT3& pos)
	//	: ComponentBase{ registry, entity }
	//{
	//	// Set position
	//	GetEntity()->GetComponent<TransformComponent>()->SetPosition(pos);

	//	// Create Geometric Primitive
	//	mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(prim_type);

	//}

	//void GeomPrimComponent::NewFrame(const floBat delta_time)
	//{

	//}

	//void GeomPrimComponent::Update(const float delta_time)
	//{
	//	auto* view = GetEntity()->GetScene()->GetView();
	//}

	//void GeomPrimComponent::RenderImGui()
	//{
	//	if (ImGui::TreeNode("GeomPrim Types"))
	//	{
	//		static int old_type = item_current;

	//		item_current = static_cast<int>(geom_prim_type); // To correspond to the Shader that the current Component has
	//		ImGui::Combo("Shader Type", &item_current, items, IM_ARRAYSIZE(items));

	//		geom_prim_type = static_cast<GeomPrimType>(item_current);
	//		mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(static_cast<GeomPrimType>(geom_prim_type));

	//		material.EditParam();

	//		ImGui::TreePop();
	//	}
	//}

	//void GeomPrimComponent::SwitchMesh(const GeomPrimType& type)
	//{
	//	mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(static_cast<GeomPrimType>(type));
	//}

}
