#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "ecs.h"
#include "dx11_configurator.h"
#include "shader.h"
#include "shader_manager.h"
#include "geometric_primitive_resource.h"
#include "transform.h"
#include "transform_component.h"
#include "blend.h"
#include "rasterizer.h"

class Light;

/*******************************************************
*	GeomPrimComponent Class
*	brief : Components for generating geometric models
*	constructor :
*   Entity* entity, ID3D11Device* device,
*	GeomPrimType primType = GeomPrimType::Cube,
*	const XMFLOAT3& pos = XMFLOAT3{0.0f,0.0f,0.0f}
*********************************************************/
class GeomPrimComponent final :public Component
{
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
public:
	GeomPrimComponent(Entity* entity,
		GeomPrimType primType = GeomPrimType::Cube,
		const XMFLOAT3& pos = XMFLOAT3{0.0f,0.0f,0.0f});
	~GeomPrimComponent()
	{

	}

	virtual void NewFrame(const float delta_time)override;
	virtual void Update(const float delta_time)override;
	virtual void RenderImGui() override;

	geometric_primitive_resource::Mesh GetMesh() const { return mesh; }
	shader::CB_Material GetMaterial() const { return material; }
	//void AddDrawData(const GeomPrimType& primType);
	void SwitchMesh(const GeomPrimType& type);

	//auto& GetDrawData() { return draw_datas; }
};