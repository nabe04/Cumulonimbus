//#include "geometric_primitive_component.h"
//
//#include <GeometricPrimitive.h>
//#include <cereal/types/bitset.hpp>
//
//#include "scene.h"
//#include "shader.h"
//#include "transform_component.h"
//
////*************************************************
////
////	GeomPrimComponent Class
////
////**************************************************
//namespace cumulonimbus::component
//{
//	GeomPrimComponent::GeomPrimComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent,
//										 GeomPrimType prim_type, const XMFLOAT3& pos)
//		: ComponentBase{ registry, ent }
//	{
//		// Set position
//		GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).SetPosition(pos);
//
//		// Create Geometric Primitive
//		mesh = GetRegistry()->GetScene()->GetGeomPrimRes()->GetMeshData(prim_type);
//
//	}
//
//	void GeomPrimComponent::NewFrame(const float delta_time)
//	{
//
//	}
//
//	void GeomPrimComponent::Update(const float delta_time)
//	{
//		//auto* view = GetRegistry()->GetScene()->GetView();
//	}
//
//	void GeomPrimComponent::RenderImGui()
//	{
//		if (ImGui::TreeNode("GeomPrim Types"))
//		{
//			static int old_type = item_current;
//
//			item_current = static_cast<int>(geom_prim_type); // To correspond to the Shader that the current Component has
//			ImGui::Combo("Shader Type", &item_current, items, IM_ARRAYSIZE(items));
//
//			geom_prim_type = static_cast<GeomPrimType>(item_current);
//			mesh = GetRegistry()->GetScene()->GetGeomPrimRes()->GetMeshData(static_cast<GeomPrimType>(geom_prim_type));
//
//			material.EditParam();
//
//			ImGui::TreePop();
//		}
//	}
//
//	void GeomPrimComponent::SwitchMesh(const GeomPrimType& type)
//	{
//		mesh = GetRegistry()->GetScene()->GetGeomPrimRes()->GetMeshData(static_cast<GeomPrimType>(type));
//	}
//
//}
