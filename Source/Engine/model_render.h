#pragma once

#include <array>
#include <memory>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "bloom.h"
#include "constant_buffer.h"
#include "sobel_filter.h"
#include "geometric_primitive_component.h"
#include "shader.h"
#include "sampler.h"
#include "depth_stencil.h"
#include "frame_buffer.h"
#include "rendering_buffer.h"
#include "fullscreen_quad.h"
#include "shadow_map.h"

class MeshObject;
class View;
class Light;
class GeomPrimComponent;
class ObjModelComponent;
class FbxModelComponent;
class Spherecollision_component;
class InnerSpherecollision_component;
class Texture2DComponent;
class DummyTexture;
class FillTexture2DComponent;
class MeshParticle;

class Blend;
class Rasterizer;

enum  RenderingSampleState
{
	Linear_Border,
	Point_Wrap,
	Linear_Wrap,
	Anistropic,

	End,
};

class MeshRenderer final
{
private:
	//-- Constant buffer --//
	std::unique_ptr<buffer::ConstantBuffer<shader::CB_CoordinateTransformation>> cbuffer_transformation;
	std::unique_ptr<buffer::ConstantBuffer<shader::CB_Material>>				 cbuffer_material;
	std::unique_ptr<buffer::ConstantBuffer<shader::CB_Light>>					 cbuffer_light;

	//-- DirectX States --//
	std::unique_ptr<Blend>																blend = nullptr;
	std::unique_ptr<Rasterizer>															rasterizer = nullptr;
	std::unique_ptr<DepthStencil>														depth_stencil = nullptr;
	std::array<std::unique_ptr<Sampler>,static_cast<int>(RenderingSampleState::End)>	samplers;

	std::unique_ptr<FrameBufferManager> frame_buffer_manager;
	std::unique_ptr<FullscreenQuad>		full_screen_quad = nullptr;
	std::unique_ptr<DummyTexture>		dummy_texture;

	//-- Effect --//
	std::unique_ptr<Bloom>			bloom;
	std::unique_ptr<SorbelFilter>	sorbel_filter;
	std::unique_ptr<ShadowMap>		shadow_map;

	// ShaderResoueceView
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_sky_map;

private:
	void RenderGeomPrim(ID3D11DeviceContext* immediate_context,
		MeshObject* actor, GeomPrimComponent* geomComponent,
		std::unique_ptr<View>const& view, std::unique_ptr<Light>const& light,
		XMFLOAT4 color = XMFLOAT4{ .0f,.0f,.0f,1.f });

	void RenderOBJ(ID3D11DeviceContext* immediate_context,
		MeshObject* actor, ObjModelComponent* modelComp,
		std::unique_ptr<View>const& view, std::unique_ptr<Light>const& light);

	void RenderFBX(ID3D11DeviceContext* immediate_context,
		MeshObject* actor, const FbxModelComponent* modelComp,
		std::unique_ptr<View>const& view,
		std::unique_ptr<Light>const& light_dir);

	void RenderSphereCollision(ID3D11DeviceContext* immediate_context,
		MeshObject* actor, Spherecollision_component* collision_comp,
		std::unique_ptr<View>const& view);

	void RenderInnerSphereCollision(ID3D11DeviceContext* immediate_context,
		MeshObject* actor, InnerSpherecollision_component* collision_comp,
		std::unique_ptr<View>const& view);

	void RenderGeomPrimMesh(ID3D11DeviceContext* immediate_context, const geometric_primitive_resource::Mesh& mesh);

	void RenderParticle(ID3D11DeviceContext* immediate_context,
		MeshObject* actor, MeshParticle* mesh_particle,
		std::unique_ptr<View>const& view);

	void Blit(ID3D11DeviceContext* immediate_context, MeshObject* actor);

public:
	MeshRenderer(ID3D11Device* device);
	~MeshRenderer() = default;

	void Begin(ID3D11DeviceContext* immediate_context);
	void End(ID3D11DeviceContext* immediate_context);

	void ShadowBegin(ID3D11DeviceContext* immediate_context);
	void ShadowEnd(ID3D11DeviceContext* immediate_context);

	void Render(ID3D11DeviceContext* immediate_context,
		const Entity* ent,
		std::unique_ptr<View>const& view,
		std::unique_ptr<Light>const& light);

	void RenderShadow(ID3D11DeviceContext* immediate_context,
		const Entity* ent,
		std::unique_ptr<View>const& view,
		std::unique_ptr<Light>const& light);

	void RenderSkyBox(ID3D11DeviceContext* immediate_context,
		const Entity* ent,
		std::unique_ptr<View>const& view,
		std::unique_ptr<Light>const& light);

	void RenderImgui();
};