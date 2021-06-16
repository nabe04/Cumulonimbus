#include "render_path.h"

#include "ecs.h"
#include "view.h"
#include "light.h"
#include "depth_map.h"
#include "locator.h"
#include "fullscreen_quad.h"
#include "frame_buffer.h"
#include "shader.h"
#include "shader_manager.h"
#include "rasterizer.h"
#include "depth_stencil.h"
#include "blend.h"
#include "texture.h"
#include "sampler_mapping.h"

#include "mesh_object.h"
#include "sprite.h"
#include "geometric_primitive_component.h"

namespace cumulonimbus::renderer
{
	RenderPath::RenderPath(ID3D11Device* device, Locator* locator)
		:locator{ locator }
	{
		back_buffer		= std::make_unique<FrameBuffer>(device, Locator::GetWindow()->Width(), Locator::GetWindow()->Height());
		fullscreen_quad = std::make_unique<FullscreenQuad>(device);
		depth_map		= std::make_unique<DepthMap>(device);

		shader_manager	= std::make_unique<shader::ShaderManager>( device );
		blend			= std::make_unique<Blend>(device);
		depth_stencil	= std::make_unique<DepthStencil>(device);
		rasterizer		= std::make_unique<Rasterizer>(device);

		cbuffer_transformation	= std::make_unique<buffer::ConstantBuffer<shader::CB_CoordinateTransformation>>(device);
		cbuffer_material		= std::make_unique<buffer::ConstantBuffer<shader::CB_Material>>(device);
		cbuffer_light			= std::make_unique<buffer::ConstantBuffer<shader::CB_Light>>(device);

		samplers.at(Linear_Border)	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR	, D3D11_TEXTURE_ADDRESS_BORDER	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplers.at(Point_Wrap)		= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_POINT	, D3D11_TEXTURE_ADDRESS_WRAP	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplers.at(Linear_Wrap)	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR	, D3D11_TEXTURE_ADDRESS_WRAP	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplers.at(Anistropic)		= std::make_unique<Sampler>(device, D3D11_FILTER_ANISOTROPIC			, D3D11_TEXTURE_ADDRESS_WRAP	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

		dummy_texture = std::make_unique<DummyTexture>( device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f } );
	}

	void RenderPath::ActivateDirectXStates(ID3D11DeviceContext* immediate_context,
										   const component::MeshObjectComponent* const mesh_object,
										   bool set_rasterizer   , bool set_sampler,
										   bool set_depth_stencil, bool set_blend)
	{
		if(set_rasterizer)
			rasterizer->Activate(immediate_context, mesh_object->GetRasterizerState());
		if(set_sampler)
			samplers.at(mesh_object->GetSamplerState())->Activate(immediate_context, mapping::sampler::SSlot_OnDemand0);
		if(set_depth_stencil)
			depth_stencil->Activate(immediate_context, mesh_object->GetDepthStencilState());
		if(set_blend)
			blend->Activate(immediate_context, mesh_object->GetBlendState());
	}


	void RenderPath::Render(ID3D11DeviceContext* const immediate_context,
							ecs::Registry* registry,
							const View* view, const Light* light)
	{
	}

	void RenderPath::RenderShadow_Begin(ID3D11DeviceContext* const immediate_context) const
	{
		depth_map->Begin(immediate_context);
	}

	void RenderPath::RenderShadow(ID3D11DeviceContext* immediate_context,
								  ecs::Registry* const registry,
								  const View* view, const Light* light)
	{
		for(auto& mesh_object : registry->GetArray<component::MeshObjectComponent>().GetComponents())
		{
			const ecs::Entity ent = mesh_object.GetEntity();

			// MeshObjectComponentが持つstate類の実行
			// sampler stateはメッシュに関係なくLinearBorderを使用
			ActivateDirectXStates(immediate_context, &mesh_object, true, false, true, true);
			samplers.at(RenderingSampleState::Linear_Border)->Activate(immediate_context, 0);


		}

	}

	void RenderPath::RenderShadow_End(ID3D11DeviceContext* immediate_context) const
	{
		depth_map->End(immediate_context);
	}
	void RenderPath::Render3D_Begin(ID3D11DeviceContext* immediate_context)
	{
	}

	void RenderPath::Render3D(ID3D11DeviceContext* immediate_context,
							  ecs::Registry* registry,
							  const View* view, const Light* light)
	{
	}

	void RenderPath::Render3D_End(ID3D11DeviceContext* immediate_context)
	{
	}

	void RenderPath::Render2D(ID3D11DeviceContext* immediate_context,
							  ecs::Registry* registry)
	{
	}

	void RenderPath::RenderSprite(ID3D11DeviceContext* immediate_context,
								  ecs::Registry* registry)
	{
	}

	void RenderPath::RenderAnimSprite(ID3D11DeviceContext* immediate_context,
									  ecs::Registry* registry)
	{
	}

	void RenderPath::RenderGeomPrim(ID3D11DeviceContext* immediate_context,
									ecs::Registry* registry, ecs::Entity entity,
									const component::MeshObjectComponent* const mesh_object,
									const View* view, const Light* light)
	{
		component::GeomPrimComponent& geom_prim = registry->GetComponent<component::GeomPrimComponent>(entity);

		light->ActivateCBuffer(immediate_context, true, true);

		ActivateDirectXStates(immediate_context, &registry->GetComponent<component::MeshObjectComponent>(entity));
		Locator::GetDx11Configurator()->BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// テクスチャのセット
		if(auto* texture = registry->TryGetComponent<component::SpriteComponent>(entity))
		{// テクスチャを保持している場合、そのテクスチャをセット
			
		}
	}

	void RenderPath::RenderOBJ(ID3D11DeviceContext* immediate_context,
							   ecs::Registry* registry, ecs::Entity entity,
							   const component::MeshObjectComponent* mesh_object,
							   const View* view, const Light* light)
	{
		
	}

	void RenderPath::RenderFBX(ID3D11DeviceContext* immediate_context,
							   ecs::Registry* registry, ecs::Entity entity,
							   const component::MeshObjectComponent* mesh_object,
							   const View* view, const Light* light)
	{
	}

	void RenderPath::RenderSkyBox(ID3D11DeviceContext* immediate_context,
								  ecs::Registry* registry, ecs::Entity entity,
								  const component::MeshObjectComponent* mesh_object,
								  const View* view, const Light* light)
	{
	}
}
