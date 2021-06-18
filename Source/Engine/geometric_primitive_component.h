#pragma once

#include <memory>
#include <vector>
#include <string>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cereal/cereal.hpp>

#include "ecs.h"
#include "component_base.h"
#include "shader.h"
#include "geometric_primitive_resource.h"

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
		GeomPrimComponent(ecs::Registry* const registry, const ecs::Entity ent,
			GeomPrimType primType = GeomPrimType::Cube,
			const XMFLOAT3& pos = XMFLOAT3{ 0.0f,0.0f,0.0f });
		~GeomPrimComponent() override = default;

		void NewFrame(const float delta_time)override;
		void Update(const float delta_time)override;
		void RenderImGui() override;

		void Save(const std::string& file_path) override{}
		void Load(const std::string& file_path_and_name) override{}

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

CEREAL_REGISTER_TYPE(cumulonimbus::component::GeomPrimComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::GeomPrimComponent)
