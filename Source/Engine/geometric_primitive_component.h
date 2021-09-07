#pragma once
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "geometric_primitive_resource.h"
#include "rename_type_mapping.h"
#include "shader.h"

/*******************************************************
*	GeomPrimComponent Class
*	brief : Components for generating geometric models
*	constructor :
*   Entity* entity, ID3D11Device* device,
*	GeomPrimType primType = GeomPrimType::Cube,
*	const XMFLOAT3& pos = XMFLOAT3{0.0f,0.0f,0.0f}
*********************************************************/

namespace cumulonimbus::component
{
	class GeomPrimComponent final :public ComponentBase
	{
	public:
		GeomPrimComponent() = default;
		GeomPrimComponent(ecs::Registry* registry, mapping::rename_type::Entity ent,
			GeomPrimType primType = GeomPrimType::Cube,
			const XMFLOAT3& pos = XMFLOAT3{ 0.0f,0.0f,0.0f });
		~GeomPrimComponent() override = default;

		void NewFrame(const float delta_time)override;
		void Update(const float delta_time)override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override{}

		geometric_primitive_resource::Mesh GetMesh() const { return mesh; }
		shader::CB_Material GetMaterial() const { return material; }
		//void AddDrawData(const GeomPrimType& primType);
		void SwitchMesh(const GeomPrimType& type);

	private:
		geometric_primitive_resource::Mesh	mesh{};
		shader::CB_Material					material{};
		GeomPrimType geom_prim_type = GeomPrimType::Box;

		//-- Constant buffer --//
		Microsoft::WRL::ComPtr<ID3D11Buffer> cb_transformation;
		Microsoft::WRL::ComPtr<ID3D11Buffer> cb_material;
		Microsoft::WRL::ComPtr<ID3D11Buffer> cb_light;

		//-- Imgui --//
		inline static const char* items[] = { "Cube","Box","Sphere","Cylinder","Tetrahedron" ,"Octahedron","Dodecahedron","Icosahedron","Cone","Tours","Teapot" };
		int item_current = 0;
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::GeomPrimComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::GeomPrimComponent)
