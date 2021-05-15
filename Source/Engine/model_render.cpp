#include "model_render.h"

#include <vector>

#include "mesh_object.h"
#include "blend.h"
#include "collision_component.h"
#include "fbx_model_component.h"
#include "fbx_model_resource.h"
#include "framework.h"
#include "inner_sphere_collision.h"
#include "light.h"
#include "obj_model_component.h"
#include "model_data.h"
#include "rasterizer.h"
#include "sampler.h"
#include "scene.h"
#include "sphere_collision_component.h"
#include "sprite.h"
#include "sky_box.h"
#include "texture.h"
#include "transform.h"
#include "transform_component.h"
#include "view.h"
#include "locator.h"
#include "mesh_particle.h"

using namespace shader;

MeshRenderer::MeshRenderer(ID3D11Device* device)
{
	cbuffer_transformation	= std::make_unique<buffer::ConstantBuffer<shader::CB_CoordinateTransformation>>(device);
	cbuffer_material		= std::make_unique<buffer::ConstantBuffer<shader::CB_Material>>(device);
	cbuffer_light			= std::make_unique<buffer::ConstantBuffer<shader::CB_Light>>(device);

	//-- DirectX States --//
	blend			= std::make_unique<Blend>(device);
	rasterizer		= std::make_unique<Rasterizer>(device);
	depth_stencil	= std::make_unique<DepthStencil>(device);

	samplers.at(Linear_Border)	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplers.at(Point_Wrap)		= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_POINT,	 D3D11_TEXTURE_ADDRESS_WRAP,   D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplers.at(Linear_Wrap)	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP,   D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplers.at(Anistropic)		= std::make_unique<Sampler>(device, D3D11_FILTER_ANISOTROPIC,		 D3D11_TEXTURE_ADDRESS_WRAP,   D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

	frame_buffer_manager = std::make_unique<FrameBufferManager>(device);

	full_screen_quad = std::make_unique<FullscreenQuad>(device);

	dummy_texture = std::make_unique<DummyTexture>(device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f });

	//-- Effects --//
	bloom			= std::make_unique<Bloom>(device, Locator::GetWindow()->Width(), Locator::GetWindow()->Height());
	sorbel_filter	= std::make_unique<SorbelFilter>(device, Locator::GetWindow()->Width(), Locator::GetWindow()->Height());
	shadow_map		= std::make_unique<ShadowMap>(device, Locator::GetWindow()->Width(), Locator::GetWindow()->Height());
}

void MeshRenderer::Begin(ID3D11DeviceContext* immediate_context)
{
	frame_buffer_manager->Clear(immediate_context);
}

void MeshRenderer::End(ID3D11DeviceContext* immediate_context)
{
	// Blending
	blend->Activate(immediate_context, BlendState::Add);
	bloom->Genarate(immediate_context, frame_buffer_manager->UsingBuffer(static_cast<int>(RenderingBuffer::Bloom))->render_target_shader_resource_view.Get());
	bloom->Blit(immediate_context);

	sorbel_filter->Blit(immediate_context, frame_buffer_manager->UsingBuffer(static_cast<int>(RenderingBuffer::SorbelFilter))->render_target_shader_resource_view.Get());

	ID3D11ShaderResourceView* const pSRV[1] = { nullptr };
	immediate_context->PSSetShaderResources(0, 1, pSRV);

	immediate_context->VSSetShader(nullptr, 0, 0);
	immediate_context->PSSetShader(nullptr, 0, 0);
}

void MeshRenderer::Blit(ID3D11DeviceContext* immediate_context, MeshObject* actor)
{
	immediate_context->PSSetShaderResources(0, 1, frame_buffer_manager->UsingBuffer(static_cast<int>(RenderingBuffer::BackBuffer))->render_target_shader_resource_view.GetAddressOf());
	full_screen_quad->Blit(immediate_context, true, true, true);
	ID3D11ShaderResourceView* const pSRV[1] = { nullptr };
	immediate_context->PSSetShaderResources(0, 1, pSRV);
	immediate_context->PSSetShaderResources(1, 1, pSRV);
	immediate_context->PSSetShaderResources(2, 1, pSRV);
}

void MeshRenderer::Render(ID3D11DeviceContext* immediate_context,
	const Entity* ent,
	std::unique_ptr<View>const& view,
	std::unique_ptr<Light>const& light)
{
	if (!ent->GetComponent<MeshObject>())
		return;

	auto* actor_comp = ent->GetComponent<MeshObject>();

	for (int index = 0; index < actor_comp->UsingBuffer()->rendering_buffer_bitset.size(); ++index)
	{
		frame_buffer_manager->UsingBuffer(index)->Activate(immediate_context);
		rasterizer->Activate(immediate_context, actor_comp->GetRasterizeState());
		blend->Activate(immediate_context, actor_comp->GetBlendState());
		depth_stencil->Activate(immediate_context, actor_comp->GetDepthStencilState());
		samplers.at(actor_comp->GetSamplerState())->Activate(immediate_context, 0);
		immediate_context->PSSetShaderResources(1, 1, shadow_map->GetDepthExtractionFB()->render_target_shader_resource_view.GetAddressOf());
		immediate_context->PSSetShaderResources(2, 1, srv_sky_map.GetAddressOf());

		if (actor_comp->UsingBuffer()->rendering_buffer_bitset.test(index))
		{
			ShaderManager::GetInstance()->Activate(immediate_context, actor_comp->GetShaderState());
		}
		else
		{
			ShaderManager::GetInstance()->ActiveteSingleColor(immediate_context);
		}

		if (auto* geom = ent->GetComponent<GeomPrimComponent>())
		{
			RenderGeomPrim(immediate_context, actor_comp, geom, view, light);
		}
		else if (auto* mesh_obj = ent->GetComponent<ObjModelComponent>())
		{
			// Render process
			RenderOBJ(immediate_context, actor_comp, mesh_obj, view, light);
		}
		else if (auto* model_fbx = ent->GetComponent<FbxModelComponent>())
		{
			RenderFBX(immediate_context, actor_comp, model_fbx, view, light);
		}

		if (auto* mesh_particle = ent->GetComponent<MeshParticle>())
		{
			RenderParticle(immediate_context, actor_comp, mesh_particle, view);
		}

#ifdef _DEBUG
		//--Render collision range --//
		if (index == static_cast<int>(RenderingBuffer::BackBuffer))
		{
			if (!ent->GetSameComponents<Spherecollision_component>().empty())
			{
				typedef std::vector<Spherecollision_component*> my_collision_types;

				const std::vector<Spherecollision_component*> collisions = ent->GetSameComponents<Spherecollision_component>();

				for (int i = 0; i < collisions.size(); ++i)
				{
					ShaderManager::GetInstance()->Activate(immediate_context, collisions.at(i)->GetShaderState());

					rasterizer->Activate(immediate_context, RasterizeState::Wireframe);
					RenderSphereCollision(immediate_context, actor_comp, collisions.at(i), view);

					ShaderManager::GetInstance()->Deactivate(immediate_context, collisions.at(i)->GetShaderState());
				}
			}

			if (auto* collision = ent->GetComponent<InnerSpherecollision_component>())
			{
				ShaderManager::GetInstance()->Activate(immediate_context, collision->GetShaderState());

				rasterizer->Activate(immediate_context, RasterizeState::Wireframe);
				RenderInnerSphereCollision(immediate_context, actor_comp, collision, view);

				ShaderManager::GetInstance()->Deactivate(immediate_context, collision->GetShaderState());
			}
		}
#endif

		if (actor_comp->UsingBuffer()->rendering_buffer_bitset.test(index))
		{
			ShaderManager::GetInstance()->Deactivate(immediate_context, actor_comp->GetShaderState());
		}
		else
		{
			ShaderManager::GetInstance()->DeactivateSingleColor(immediate_context);
		}

		frame_buffer_manager->UsingBuffer(index)->Deactivate(immediate_context);
	}

	Blit(immediate_context, actor_comp);
}

void MeshRenderer::ShadowBegin(ID3D11DeviceContext* immediate_context)
{
	shadow_map->Begin(immediate_context);
}

void MeshRenderer::ShadowEnd(ID3D11DeviceContext* immediate_context)
{
	//ImGui::Image((void*)shadow_map->GetDepthExtractionFB()->render_target_shader_resource_view.Get(), { 300,300 });
	shadow_map->End(immediate_context);
}

void MeshRenderer::RenderShadow(ID3D11DeviceContext* immediate_context,
	const Entity* ent,
	std::unique_ptr<View>const& view,
	std::unique_ptr<Light>const& light)
{
	if (!ent->GetComponent<MeshObject>())
		return;

	auto* actor_comp = ent->GetComponent<MeshObject>();

	rasterizer->Activate(immediate_context, actor_comp->GetRasterizeState());
	blend->Activate(immediate_context, actor_comp->GetBlendState());
	depth_stencil->Activate(immediate_context, actor_comp->GetDepthStencilState());

	for (int i = 0; i < static_cast<int>(ShadowMap::RenderProcess::End); ++i)
	{
		shadow_map->GetBiginRenderingState()->SetState(static_cast<ShadowMap::RenderProcess>(i));
		shadow_map->GetBiginRenderingState()->Update(immediate_context, ShadowMap::M_ShadowMap{});

		if (auto* geom = ent->GetComponent<GeomPrimComponent>())
		{
			RenderGeomPrim(immediate_context, actor_comp, geom, view, light);
		}
		else if (auto* mesh_obj = ent->GetComponent<ObjModelComponent>())
		{
			// Render process
			RenderOBJ(immediate_context, actor_comp, mesh_obj, view, light);
		}
		else if (auto* model_fbx = ent->GetComponent<FbxModelComponent>())
		{
			RenderFBX(immediate_context, actor_comp, model_fbx, view, light);
		}

		shadow_map->GetEndRenderingState()->SetState(static_cast<ShadowMap::RenderProcess>(i));
		shadow_map->GetEndRenderingState()->Update(immediate_context);
	}
}

void MeshRenderer::RenderSkyBox(ID3D11DeviceContext* immediate_context,
	const Entity* ent,
	std::unique_ptr<View>const& view,
	std::unique_ptr<Light>const& light)
{
	auto* sky_box = ent->GetComponent<SkyBox>();

	if (!sky_box)
		return;

	sky_box->ActivateShader(immediate_context);
	frame_buffer_manager->UsingBuffer(static_cast<int>(RenderingBuffer::BackBuffer))->Activate(immediate_context);

	{// Transform
		shader::CB_CoordinateTransformation cb{};
		cb.mat_view_projection = view->GetView() * view->GetProjection();

		cb.bone_transforms[0] = ent->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();
		cbuffer_transformation->data = cb;
		cbuffer_transformation->Activate(immediate_context, 0);
	}

	// Set of primitives topology
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	immediate_context->PSSetShaderResources(0, 1, sky_box->GetShaderResoueceViewAddress());
	samplers.at(RenderingSampleState::Linear_Border)->Activate(immediate_context, 0);

	depth_stencil->Activate(immediate_context, DepthStencilState::Depth_First);

	rasterizer->Activate(immediate_context, RasterizeState::Cull_None);

	// Set of Vertex Buffers
	UINT stride = sizeof(shader::Vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, sky_box->GetVertexBufferAddress(), &stride, &offset);
	immediate_context->IASetIndexBuffer(sky_box->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	immediate_context->DrawIndexed(36, 0, 0);

	srv_sky_map = sky_box->GetShaderResoueceView();
	frame_buffer_manager->UsingBuffer(static_cast<int>(RenderingBuffer::BackBuffer))->Deactivate(immediate_context);
	Blit(immediate_context, nullptr);

	sky_box->DeactivateShader(immediate_context);
}

void MeshRenderer::RenderGeomPrimMesh(ID3D11DeviceContext* immediate_context, const geometric_primitive_resource::Mesh& mesh)
{
	// Set of Vertex Buffers
	UINT stride = sizeof(shader::Vertex);
	UINT offset = 0;

	immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
	// Set of Index Buffer
	immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Rendering the index buffer
	immediate_context->DrawIndexed(mesh.num_indices, 0, 0);
}

void MeshRenderer::RenderGeomPrim(ID3D11DeviceContext* immediate_context,
	MeshObject* actor ,GeomPrimComponent* geomComponent,
	std::unique_ptr<View>const& view,std::unique_ptr<Light>const& light,
	XMFLOAT4 color)
{
	auto ent = geomComponent->GetEntity();
	//-- Set of Constant Buffer --//
	{// Light
		//shader::CB_Light cb;
		//cb.light_dir			 = light->GetLightDir();
		//cb.color				 = light->GetColor();
		//cb.ambient				 = light->GetAmbient();
		//cb.eye_pos				 = XMFLOAT4{ view->GetPos().x,view->GetPos().y,view->GetPos().z,1.0f };
		//cb.light_view_projection = light->LightViewProjectionMatrix();
		//cb.position = XMFLOAT4{ light->Position().x,light->Position().y,light->Position().z,1.0f };

		//cbuffer_light->data = cb;
		//cbuffer_light->Activate(immediate_context, 2);

		light->ActivateCBuffer(immediate_context, true, true);
	}

	// Set of primitives topology
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set of DepthStencilState
	depth_stencil->Activate(immediate_context, actor->GetDepthStencilState());

	// Set of texture
	if (ent->GetComponent<SpriteComponent>())
	{//Set of Texture
		auto* texture = ent->GetComponent<SpriteComponent>()->GetTexture();
		immediate_context->PSSetShaderResources(0, 1, texture->GetTextureData()->texture_view.GetAddressOf());
		samplers.at(actor->GetSamplerState())->Activate(immediate_context, 0);
	}
	else
	{// Set of dummy texture
		auto* dx_instance = Locator::GetDx11Configurator();
		immediate_context->PSSetShaderResources(0, 1, dummy_texture->dummy_texture.GetAddressOf());
		samplers.at(actor->GetSamplerState())->Activate(immediate_context, 0);
	}

	{// Transform
		shader::CB_CoordinateTransformation cb{};
		cb.mat_view_projection = view->GetView() * view->GetProjection();

		cb.bone_transforms[0] = ent->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();
		cbuffer_transformation->data = cb;
		cbuffer_transformation->Activate(immediate_context, 0);
	}

	{// Set constant buffer (Material)
		shader::CB_Material cb{};
		cb.color	= geomComponent->GetMaterial().color;
		cb.Ka		= geomComponent->GetMaterial().Ka;
		cb.Kd		= geomComponent->GetMaterial().Kd;
		cb.Ks		= geomComponent->GetMaterial().Ks;
		cbuffer_material->data = cb;
		cbuffer_material->Activate(immediate_context, 1);
	}

	RenderGeomPrimMesh(immediate_context, geomComponent->GetMesh());
	light->DeactivateCBuffer(immediate_context);
}

void MeshRenderer::RenderParticle(ID3D11DeviceContext* immediate_context,
	MeshObject* actor, MeshParticle* mesh_particle,
	std::unique_ptr<View>const& view)
{
	if (mesh_particle->GetParticleDatas()->empty())
		return;
	const auto* ent = actor->GetEntity();

	{// Transform
		shader::CB_CoordinateTransformation cb{};
		cb.mat_view_projection = view->GetView() * view->GetProjection();

		cb.bone_transforms[0] = ent->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();
		cbuffer_transformation->data = cb;
		cbuffer_transformation->Activate(immediate_context, 0);
	}

	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	mesh_particle->GetVertexShader()->Activate(immediate_context);
	mesh_particle->GetPixelShader()->Activate(immediate_context);

	UINT stride = sizeof(MeshParticle::Vb_Fixed_S0);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, mesh_particle->GetVertexBuffer_S0()->GetVertexBufferAddress(), &stride, &offset);

	static float up = 0;
	if(Locator::GetInput()->Keyboard().GetState(Keycode::Up) == ButtonState::Held)
		up += 0.1f;
	if (Locator::GetInput()->Keyboard().GetState(Keycode::Down) == ButtonState::Held)
		up -= 0.1f;
	DirectX::SimpleMath::Matrix mat = DirectX::SimpleMath::Matrix::Identity;
	mat = DirectX::SimpleMath::Matrix::CreateTranslation({ 0,up,0 });

	MeshParticle::Cb_Particle cb{};
	cb.color			= mesh_particle->GetParticleDatas()->at(0).color;
	cb.position			= DirectX::SimpleMath::Vector4{ mesh_particle->GetParticleDatas()->at(0).position.x,mesh_particle->GetParticleDatas()->at(0).position.y,mesh_particle->GetParticleDatas()->at(0).position.z,1.f };
	//cb.transform_matrix = mesh_particle->GetParticleDatas()->at(0).transform.GetWorld4x4() * mat;
	cb.transform_matrix = actor->GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();
	mesh_particle->GetConstantBuffer()->data = cb;
	mesh_particle->GetConstantBuffer()->Activate(immediate_context, 6);

	UINT vertex_count = static_cast<UINT>(mesh_particle->GetParticleDatas()->size());
	immediate_context->Draw(vertex_count, 0);

	mesh_particle->GetConstantBuffer()->Deactivate(immediate_context);
	mesh_particle->GetVertexShader()->Deactivate(immediate_context);
	mesh_particle->GetPixelShader()->Deactivate(immediate_context);
}

void MeshRenderer::RenderSphereCollision(ID3D11DeviceContext* immediate_context,
	MeshObject* actor, Spherecollision_component* collision_comp,
	std::unique_ptr<View>const& view)
{
	auto ent = collision_comp->GetEntity();

	const auto& datas = collision_comp->GetSphereData();

	if (!collision_comp->IsJudge())
		return;

	for (const auto& data : datas)
	{
		//-- Set of Constant Buffer --//
		{// Transform
			shader::CB_CoordinateTransformation cb{};
			cb.mat_view_projection = view->GetView() * view->GetProjection();

			cb.bone_transforms[0] = data.world_matrix;
			cbuffer_transformation->data = cb;
			cbuffer_transformation->Activate(immediate_context, 0);
		}
		{// Material
			shader::CB_Material cb{};
			cb.color = { data.color_RGB.x,data.color_RGB.y,data.color_RGB.z,1.f };
			cb.Ka = {1.f,1.f,1.f,1.f};
			cb.Kd = {1.f,1.f,1.f,1.f};
			cb.Ks = {1.f,1.f,1.f,1.f};
			cbuffer_material->data = cb;
			cbuffer_material->Activate(immediate_context, 1);
		}
		{// Light
			shader::CB_Light cb{};
			cb.light_dir = {};
			cb.color	= {};
			cb.ambient	= {};
			cb.eye_pos   = {};

			cbuffer_light->data = cb;
			cbuffer_light->Activate(immediate_context, 2);
		}


		// Set of primitives topology
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set of DepthStencilState
		depth_stencil->Activate(immediate_context, actor->GetDepthStencilState());

		auto* dx_instance = Locator::GetDx11Configurator();
		immediate_context->PSSetShaderResources(0, 1, dummy_texture->dummy_texture.GetAddressOf());
		samplers.at(actor->GetSamplerState())->Activate(immediate_context, 0);

		RenderGeomPrimMesh(immediate_context, collision_comp->GetMesh());
	}
}

void MeshRenderer::RenderInnerSphereCollision(ID3D11DeviceContext* immediate_context,
	MeshObject* actor, InnerSpherecollision_component* collision_comp,
	std::unique_ptr<View>const& view)
{
	auto ent = collision_comp->GetEntity();

	const auto& datas = collision_comp->GetSphereData();

	if (!collision_comp->IsJudge())
		return;

	for (const auto& data : datas)
	{
		//-- Set of Constant Buffer --//
		{// Transform
			shader::CB_CoordinateTransformation cb{};
			cb.mat_view_projection = view->GetView() * view->GetProjection();

			cb.bone_transforms[0] = data.world_matrix;
			cbuffer_transformation->data = cb;
			cbuffer_transformation->Activate(immediate_context, 0);
		}
		{// Material
			shader::CB_Material cb;
			cb.color = { data.color_RGB.x,data.color_RGB.y,data.color_RGB.z,1.f };
			cb.Ka = { 1.f,1.f,1.f,1.f };
			cb.Kd = { 1.f,1.f,1.f,1.f };
			cb.Ks = { 1.f,1.f,1.f,1.f };
			cbuffer_material->data = cb;
			cbuffer_material->Activate(immediate_context, 1);
		}
		{// Light
			shader::CB_Light cb;
			cb.light_dir = {};
			cb.color = {};
			cb.ambient = {};
			cb.eye_pos = {};

			cbuffer_light->data = cb;
			cbuffer_light->Activate(immediate_context, 2);
		}


		// Set of primitives topology
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set of DepthStencilState
		depth_stencil->Activate(immediate_context, actor->GetDepthStencilState());

		auto* dx_instance = Locator::GetDx11Configurator();
		immediate_context->PSSetShaderResources(0, 1, dummy_texture->dummy_texture.GetAddressOf());
		samplers.at(actor->GetSamplerState())->Activate(immediate_context, 0);

		RenderGeomPrimMesh(immediate_context, collision_comp->GetMesh());
	}
}

void MeshRenderer::RenderOBJ(ID3D11DeviceContext* immediate_context,
	MeshObject* actor, ObjModelComponent* objComponent,
	std::unique_ptr<View>const& view, std::unique_ptr<Light>const& light)
{
	auto ent = objComponent->GetEntity();

	//-- Set of Constant Buffer --//
	{ // Transform

		shader::CB_CoordinateTransformation cb{};
		cb.mat_view_projection = view->GetView() * view->GetProjection();

		cb.bone_transforms[0] = ent->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();
		cbuffer_transformation->data = cb;
		cbuffer_transformation->Activate(immediate_context, 0);

	}

	{// Light
		//shader::CB_Light cb;
		//cb.light_dir = light->GetLightDir();
		//cb.color = light->GetColor();
		//cb.ambient = light->GetAmbient();
		//cb.eye_pos = XMFLOAT4{ view->GetPos().x,view->GetPos().y,view->GetPos().z,1.0f };
		//cb.light_view_projection = light->LightViewProjectionMatrix();
		//cb.position = XMFLOAT4{ light->Position().x,light->Position().y,light->Position().z,1.0f };

		//cbuffer_light->data = cb;
		//cbuffer_light->Activate(immediate_context, 2);

		light->ActivateCBuffer(immediate_context, true, true);
	}


	// Set of primitives topology
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set of DepthStencilState
	depth_stencil->Activate(immediate_context, actor->GetDepthStencilState());

	u_int stride = sizeof(shader::Vertex);
	u_int offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, objComponent->GetVertexBufferAddress(), &stride, &offset);
	immediate_context->IASetIndexBuffer(objComponent->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	for (auto& material : objComponent->GetMaterials())
	{
		shader::CB_Material data;
		data.color = material.data.color;
		data.Ka = XMFLOAT4(material.data.Ka.x, material.data.Ka.y, material.data.Ka.z, 1.0f);
		data.Kd = XMFLOAT4(material.data.Kd.x, material.data.Kd.y, material.data.Kd.z, 1.0f);
		//data.Kd =
		cbuffer_material->data = data;
		cbuffer_material->Activate(immediate_context, 1);

		if (material.texture)
		{// Set of Texture
			immediate_context->PSSetShaderResources(0, 1, material.texture->GetShaderResourceViewAddress());
			samplers.at(actor->GetSamplerState())->Activate(immediate_context, 0);
		}
		else
		{// Set of Dummy texture
			immediate_context->PSSetShaderResources(0, 1, dummy_texture->dummy_texture.GetAddressOf());
			samplers.at(actor->GetSamplerState())->Activate(immediate_context, 0);
		}

		for (auto& subset : objComponent->GetSubsets())
		{
			if (material.newmtl == subset.usemtl)
			{
				immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}

	light->DeactivateCBuffer(immediate_context);
}

void MeshRenderer::RenderFBX(ID3D11DeviceContext* immediate_context,
	MeshObject* actor, const FbxModelComponent* model,
	std::unique_ptr<View>const& view,
	std::unique_ptr<Light>const& light)
{
	const FbxModelResource* resource = model->GetResource();
	const std::vector<FbxModelComponent::Node>& nodes = model->GetNodes();

	// Set of DepthStencilState
	//immediate_context->OMSetDepthStencilState(Dx11Configurator::GetInstance().GetDepthStencilState(actor_comp->GetDepthStencilState()), 1);

	{// Light
		//shader::CB_Light cb;
		//cb.light_dir	= light->GetLightDir();
		//cb.color		= light->GetColor();
		//cb.ambient		= light->GetAmbient();
		//cb.eye_pos		= XMFLOAT4{ view->GetPos().x,view->GetPos().y,view->GetPos().z,1.0f };
		//cb.light_view_projection = light->LightViewProjectionMatrix();
		//cb.position = XMFLOAT4{ light->Position().x,light->Position().y,light->Position().z,1.0f };

		//cbuffer_light->data = cb;
		//cbuffer_light->Activate(immediate_context, 2);

		light->ActivateCBuffer(immediate_context, true, true);
	}

	for (const ModelData::Mesh& mesh : resource->GetModelData().meshes)
	{
		// メッシュ用定数バッファ更新
		shader::CB_CoordinateTransformation cb_mesh;
		::memset(&cb_mesh, 0, sizeof(cb_mesh));
		if (mesh.node_indices.size() > 0)
		{
			for (size_t i = 0; i < mesh.node_indices.size(); ++i)
			{
				DirectX::SimpleMath::Matrix reverse;
				reverse.Right(DirectX::SimpleMath::Vector3(-1, 0, 0));

				DirectX::SimpleMath::Matrix world_transform   = DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
				DirectX::SimpleMath::Matrix inverse_transform = DirectX::XMLoadFloat4x4(&mesh.inverse_transforms.at(i));
				DirectX::SimpleMath::Matrix bone_transform	= inverse_transform * world_transform;
				XMStoreFloat4x4(&cb_mesh.bone_transforms[i], bone_transform);
				cb_mesh.mat_view_projection = view->GetView() * view->GetProjection();
				//XMStoreFloat4x4(&cb_mesh.mat_view_projection, XMLoadFloat4x4(&view->GetView()) * XMLoadFloat4x4(&view->GetProjection()));
			}
		}
		else
		{
			cb_mesh.bone_transforms[0] = model->GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4();
			cb_mesh.mat_view_projection = view->GetView() * view->GetProjection();
			//XMStoreFloat4x4(&cb_mesh.mat_view_projection, XMLoadFloat4x4(&view->GetView()) * XMLoadFloat4x4(&view->GetProjection()));
		}
		cbuffer_transformation->data = cb_mesh;
		cbuffer_transformation->Activate(immediate_context, 0);

		UINT stride = sizeof(shader::Vertex);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		for (const ModelData::Subset& subset : mesh.subsets)
		{
			shader::CB_Material cb_subset;
			cb_subset.color = subset.material != NULL ? subset.material->color : XMFLOAT4{ 0.8f, 0.8f, 0.8f, 1.0f };
			cb_subset.color.w = model->color.w;
			cbuffer_material->data = cb_subset;
			cbuffer_material->Activate(immediate_context, 1);
			if (subset.material && subset.material->shader_resource_view)
			{
				immediate_context->PSSetShaderResources(0, 1, subset.material->shader_resource_view.GetAddressOf());
			}
			else
			{
				immediate_context->PSSetShaderResources(0, 1, dummy_texture->dummy_texture.GetAddressOf());
			}
			samplers.at(actor->GetSamplerState())->Activate(immediate_context, 0);
			immediate_context->DrawIndexed(subset.index_count, subset.start_index, 0);
		}
	}

	light->DeactivateCBuffer(immediate_context);
}

void MeshRenderer::RenderImgui()
{

}