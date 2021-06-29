#include "render_path.h"

#include <cereal/types/array.hpp>
#include <cereal/types/bitset.hpp>

#include "sampler_mapping.h"
#include "scene.h"
#include "shader_interop_renderer.h"
#include "texture_resource_mapping.h"
#include "view.h"

// Components
#include "anim_sprite.h"
#include "fbx_model_component.h"
#include "geometric_primitive_component.h"
#include "material_component.h"
#include "mesh_object.h"
#include "obj_model_component.h"
#include "sky_box.h"
#include "sprite.h"
#include "sprite_object.h"
#include "transform_component.h"
#include "material_instance_component.h"
#include "shader_assets_component.h"

namespace cumulonimbus::renderer
{
	RenderPath::RenderPath(ID3D11Device* device)
	{
		off_screen		= std::make_unique<FrameBuffer>(device, locator::Locator::GetWindow()->Width(), locator::Locator::GetWindow()->Height());
		fullscreen_quad = std::make_unique<FullscreenQuad>(device);
		depth_map		= std::make_unique<DepthMap>(device);

		shader_manager		= std::make_unique<shader_system::ShaderManager>();
		old_shader_manager	= std::make_unique<shader::ShaderManager>( device );
		shader_manager_2d	= std::make_unique<shader::SpriteShaderManager>(device);
		blend				= std::make_unique<Blend>(device);
		depth_stencil		= std::make_unique<DepthStencil>(device);
		rasterizer			= std::make_unique<Rasterizer>(device);

		samplers.at(Linear_Border)	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR	, D3D11_TEXTURE_ADDRESS_BORDER	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplers.at(Point_Wrap)		= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_POINT	, D3D11_TEXTURE_ADDRESS_WRAP	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplers.at(Linear_Wrap)	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR	, D3D11_TEXTURE_ADDRESS_WRAP	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplers.at(Anistropic)		= std::make_unique<Sampler>(device, D3D11_FILTER_ANISOTROPIC			, D3D11_TEXTURE_ADDRESS_WRAP	, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

		dummy_texture = std::make_unique<DummyTexture>( device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f } );
	}

	void RenderPath::BindDirectXStates(ID3D11DeviceContext* immediate_context,
										   const component::MeshObjectComponent* const mesh_object,
										   bool set_rasterizer   , bool set_sampler,
										   bool set_depth_stencil, bool set_blend)
	{
		if(set_rasterizer)
			rasterizer->Activate(immediate_context, mesh_object->GetRasterizerState());
		if(set_sampler)
			samplers.at(mesh_object->GetSamplerState())->Activate(immediate_context, SSlot_OnDemand0);
		if(set_depth_stencil)
			depth_stencil->Activate(immediate_context, mesh_object->GetDepthStencilState());
		if(set_blend)
			blend->Activate(immediate_context, mesh_object->GetBlendState());
	}

	void RenderPath::BindDirectXStates(ID3D11DeviceContext* immediate_context,
									   const component::SpriteObjectComponent* sprite_object,
									   const bool set_rasterizer   , const bool set_sampler,
									   const bool set_depth_stencil, const bool set_blend)
	{
		if (set_rasterizer)
			rasterizer->Activate(immediate_context, sprite_object->GetRasterizerState());
		if (set_sampler)
			samplers.at(static_cast<int>(sprite_object->GetSamplerState()))->Activate(immediate_context, SSlot_OnDemand0);
		if (set_depth_stencil)
			depth_stencil->Activate(immediate_context, sprite_object->GetDepthStencilState());
		if (set_blend)
			blend->Activate(immediate_context, sprite_object->GetBlendState());
	}

	void RenderPath::Render(ID3D11DeviceContext* const immediate_context,
							ecs::Registry* registry,
							const View* view, const Light* light)
	{
		RenderShadow_Begin(immediate_context);
		RenderShadow(immediate_context, registry, view, light);
		RenderShadow_End(immediate_context);

		Render3D_Begin(immediate_context);
		Render3D(immediate_context, registry, view, light);
		Render3D_End(immediate_context);

		Render2D(immediate_context, registry);
	}

	void RenderPath::RenderShadow_Begin(ID3D11DeviceContext* const immediate_context) const
	{
		depth_map->Begin(immediate_context);
	}

	void RenderPath::RenderShadow(ID3D11DeviceContext* immediate_context,
								  ecs::Registry* const registry,
								  const View* view, const Light* light)
	{
		depth_map->Activate(immediate_context);
		light->BindCBuffer();

		for(auto& mesh_object : registry->GetArray<component::MeshObjectComponent>().GetComponents())
		{
			const mapping::rename_type::Entity ent = mesh_object.GetEntity();

			// MeshObjectComponentが持つstate類の実行
			// sampler stateはメッシュに関係なくLinearBorderを使用
			BindDirectXStates(immediate_context, &mesh_object,
									/*rasterizer*/   true, /*sampler*/false,
									/*depth stencil*/true, /*blend*/  true);
			samplers.at(RenderingSampleState::Linear_Border)->Activate(immediate_context, 0);

			if(auto* geom = registry->TryGetComponent<component::GeomPrimComponent>(ent))
			{
				RenderGeomPrim(immediate_context, registry, ent, &mesh_object, view, light);
			}
			else if(auto* obj_model = registry->TryGetComponent<component::ObjModelComponent>(ent))
			{
				RenderOBJ(immediate_context, registry, ent, &mesh_object, view, light);
			}
			else if(auto* fbx_model = registry->TryGetComponent<component::FbxModelComponent>(ent))
			{
				RenderFBX(immediate_context, registry, ent, &mesh_object, view, light);
			}
		}

		light->UnbindCBuffer();
		depth_map->Deactivate(immediate_context);
	}

	void RenderPath::RenderShadow_End(ID3D11DeviceContext* immediate_context) const
	{
		depth_map->End(immediate_context);
	}
	void RenderPath::Render3D_Begin(ID3D11DeviceContext* immediate_context)
	{
		off_screen->Clear(immediate_context);
		off_screen->Activate(immediate_context);
	}

	void RenderPath::Render3D(ID3D11DeviceContext* immediate_context,
							  ecs::Registry* registry,
							  const View* view, const Light* light)
	{
		light->BindCBuffer();

		auto& components = registry->GetArray<component::SkyBoxComponent>().GetComponents();
		for(auto& sky_box : registry->GetArray<component::SkyBoxComponent>().GetComponents())
		{
			mapping::rename_type::Entity ent = sky_box.GetEntity();
			RenderSkyBox(immediate_context, registry, sky_box.GetEntity(), view, light);
		}

		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, sky_box_srv.GetAddressOf(), TexSlot_SkyMap);

		//off_screen->Activate(immediate_context);
		for (auto& mesh_object : registry->GetArray<component::MeshObjectComponent>().GetComponents())
		{
			const mapping::rename_type::Entity ent = mesh_object.GetEntity();

			// MeshObjectComponentが持つstate類の実行
			// sampler stateはメッシュに関係なくLinearBorderを使用
			BindDirectXStates(immediate_context, &mesh_object);

			locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, depth_map->GetDepthExtractionSRV(), TexSlot_Depth);

			const auto asset = registry->GetComponent<component::MaterialInstance3DComponent>(ent).GetCurrentAsset();
			shader_manager->BindShader(asset);
			registry->GetComponent<component::ShaderAssets3DComponent>(ent).BindCBuffer(asset);
			//registry->GetComponent<component::>()
			//old_shader_manager->Activate(immediate_context, registry->GetComponent<component::MeshObjectComponent>(ent).GetShaderState());

			if (auto* geom = registry->TryGetComponent<component::GeomPrimComponent>(ent))
			{
				RenderGeomPrim(immediate_context, registry, ent, &mesh_object, view, light);
			}
			else if (auto* obj_model = registry->TryGetComponent<component::ObjModelComponent>(ent))
			{
				RenderOBJ(immediate_context, registry, ent, &mesh_object, view, light);
			}
			else if (auto* fbx_model = registry->TryGetComponent<component::FbxModelComponent>(ent))
			{
				RenderFBX(immediate_context, registry, ent, &mesh_object, view, light);
			}
		}

		light->UnbindCBuffer();
		off_screen->Deactivate(immediate_context);
		Blit(immediate_context);
	}

	void RenderPath::Render3D_End(ID3D11DeviceContext* immediate_context)
	{
		using namespace cumulonimbus::mapping;
		locator::Locator::GetDx11Device()->UnbindShaderResource(graphics::ShaderStage::PS, 0);
		locator::Locator::GetDx11Device()->UnbindShaderResource(graphics::ShaderStage::PS, 1);
		locator::Locator::GetDx11Device()->UnbindShaderResource(graphics::ShaderStage::PS, 2);

		immediate_context->VSSetShader(nullptr, nullptr, 0);
		immediate_context->PSSetShader(nullptr, nullptr, 0);
	}

	void RenderPath::Render2D(ID3D11DeviceContext* immediate_context,
							  ecs::Registry* registry)
	{
		for(auto& sprite : registry->GetArray<component::SpriteComponent>().GetComponents())
		{
			component::SpriteObjectComponent& sprite_obj = registry->GetComponent<component::SpriteObjectComponent>(sprite.GetEntity());

			BindDirectXStates(immediate_context, &sprite_obj);
			shader_manager_2d->Activate(immediate_context, sprite_obj.GetShaderState());
			RenderSprite(immediate_context, registry, sprite.GetEntity());
			shader_manager_2d->Deactivate(immediate_context, sprite_obj.GetShaderState());
		}

		for(auto& anim_sprite : registry->GetArray<component::AnimSpriteComponent>().GetComponents())
		{
			component::SpriteObjectComponent& sprite_obj = registry->GetComponent<component::SpriteObjectComponent>(anim_sprite.GetEntity());

			BindDirectXStates(immediate_context, &sprite_obj);
			shader_manager_2d->Activate(immediate_context, sprite_obj.GetShaderState());
			RenderSprite(immediate_context, registry, anim_sprite.GetEntity());
			RenderAnimSprite(immediate_context, registry, anim_sprite.GetEntity());
			shader_manager_2d->Deactivate(immediate_context, sprite_obj.GetShaderState());
		}
	}

	void RenderPath::Blit(ID3D11DeviceContext* immediate_context) const
	{
		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
													 off_screen->GetRenderTargetSRV(),
													 TexSlot_BaseColorMap);
		fullscreen_quad->Blit(immediate_context, true, true, true);
		ID3D11ShaderResourceView* const pSRV[1] = { nullptr };
	}

	void RenderPath::RenderGeomPrim(ID3D11DeviceContext* immediate_context,
									ecs::Registry* registry, mapping::rename_type::Entity entity,
									const component::MeshObjectComponent* const mesh_object,
									const View* view, const Light* light)
	{
		component::GeomPrimComponent& geom_prim = registry->GetComponent<component::GeomPrimComponent>(entity);

		BindDirectXStates(immediate_context, &registry->GetComponent<component::MeshObjectComponent>(entity));
		locator::Locator::GetDx11Device()->BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// テクスチャのセット
		if(auto* texture = registry->TryGetComponent<component::SpriteComponent>(entity))
		{// テクスチャを保持している場合、そのテクスチャをセット
			locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
														 texture->GetTexture(),
														 TexSlot_BaseColorMap);
		}
		else
		{
			locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
													     dummy_texture->dummy_texture.GetAddressOf(),
														 TexSlot_BaseColorMap);
		}

		{// Transform
			TransformCB transform{};
			transform.bone_transforms[0] = registry->GetComponent<component::TransformComponent>(entity).GetWorld4x4();
			registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);
		}

		{// Set constant buffer (Material)
			MaterialCB material{};
			material.material.base_color = geom_prim.GetMaterial().color;;
			registry->GetComponent<component::MaterialComponent>(entity).SetAndBindCBuffer(material);
		}

		{// 描画処理
			// Set of Vertex Buffers
			UINT stride = sizeof(shader::Vertex);
			UINT offset = 0;

			immediate_context->IASetVertexBuffers(0, 1, geom_prim.GetMesh().vertex_buffer.GetAddressOf(), &stride, &offset);
			// Set of Index Buffer
			immediate_context->IASetIndexBuffer(geom_prim.GetMesh().index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			// Rendering the index buffer
			immediate_context->DrawIndexed(geom_prim.GetMesh().num_indices, 0, 0);
		}

		registry->GetComponent<component::TransformComponent>(entity).UnbindCBuffer();
		registry->GetComponent<component::MaterialComponent>(entity).UnbindCBuffer();
	}

	void RenderPath::RenderOBJ(ID3D11DeviceContext* immediate_context,
							   ecs::Registry* registry, mapping::rename_type::Entity entity,
							   const component::MeshObjectComponent* mesh_object,
							   const View* view, const Light* light)
	{
		//-- Set of Constant Buffer --//
		{// Transform
			TransformCB transform{};

			transform.bone_transforms[0] = registry->GetComponent<component::TransformComponent>(entity).GetWorld4x4();
			registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);
		}

		locator::Locator::GetDx11Device()->BindPrimitiveTopology(mapping::graphics::PrimitiveTopology::TriangleList);
		BindDirectXStates(immediate_context, &registry->GetComponent<component::MeshObjectComponent>(entity));

		component::ObjModelComponent& obj_model = registry->GetComponent<component::ObjModelComponent>(entity);
		u_int stride = sizeof(shader::Vertex);
		u_int offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, obj_model.GetVertexBufferAddress(), &stride, &offset);
		immediate_context->IASetIndexBuffer(obj_model.GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		for(auto& material : obj_model.GetMaterials())
		{
			MaterialCB cb_material{};
			cb_material.material.base_color = material.data.color;
			registry->GetComponent<component::MaterialComponent>(entity).SetAndBindCBuffer(cb_material);

			if (material.texture)
			{// Set of Texture
				locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
														     material.texture,
														     TexSlot_BaseColorMap);
			}
			else
			{// Set of Dummy texture
				locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
															 dummy_texture->dummy_texture.GetAddressOf(),
															 TexSlot_BaseColorMap);
			}

			for (auto& subset : obj_model.GetSubsets())
			{
				if (material.newmtl == subset.usemtl)
				{
					immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
				}
			}

			registry->GetComponent<component::MaterialComponent>(entity).UnbindCBuffer();
		}

		registry->GetComponent<component::TransformComponent>(entity).UnbindCBuffer();
	}

	void RenderPath::RenderFBX(ID3D11DeviceContext* immediate_context,
							   ecs::Registry* registry, mapping::rename_type::Entity entity,
							   const component::MeshObjectComponent* mesh_object,
							   const View* view, const Light* light)
	{
		const component::FbxModelComponent&	 model		= registry->GetComponent<component::FbxModelComponent>(entity);
		const FbxModelResource*				 resource	= model.GetResource();

		for(const auto& mesh : resource->GetModelData().meshes)
		{
			// メッシュ用定数バッファ更新
			TransformCB transform{};

			if (mesh.node_indices.size() > 0)
			{
				for (size_t i = 0; i < mesh.node_indices.size(); ++i)
				{
					DirectX::SimpleMath::Matrix reverse;
					reverse.Right(DirectX::SimpleMath::Vector3(-1, 0, 0));

					DirectX::SimpleMath::Matrix world_transform   = DirectX::XMLoadFloat4x4(&model.GetNodes().at(mesh.node_indices.at(i)).world_transform);
					DirectX::SimpleMath::Matrix inverse_transform = DirectX::XMLoadFloat4x4(&mesh.inverse_transforms.at(i));
					DirectX::SimpleMath::Matrix bone_transform    = inverse_transform * world_transform;
					XMStoreFloat4x4(&transform.bone_transforms[i], bone_transform);
				}
			}
			else
			{
				transform.bone_transforms[0] = registry->GetComponent<component::TransformComponent>(entity).GetWorld4x4();
			}

			registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);
			view->BindCBuffer();

			UINT stride = sizeof(shader::Vertex);
			UINT offset = 0;
			immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
			immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			locator::Locator::GetDx11Device()->BindPrimitiveTopology(mapping::graphics::PrimitiveTopology::TriangleList);

			for (const ModelData::Subset& subset : mesh.subsets)
			{
				MaterialCB cb_material;
				cb_material.material.base_color   = subset.material != nullptr ? subset.material->color : XMFLOAT4{ 0.8f, 0.8f, 0.8f, 1.0f };
				cb_material.material.base_color.w = model.GetColor().w;
				registry->GetComponent<component::MaterialComponent>(entity).SetAndBindCBuffer(cb_material);

				if (subset.material && subset.material->shader_resource_view)
				{
					locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
																 subset.material->shader_resource_view.GetAddressOf(),
																 TexSlot_BaseColorMap);
				}
				else
				{
					locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
																 dummy_texture->dummy_texture.GetAddressOf(),
																 TexSlot_BaseColorMap);
				}
				immediate_context->DrawIndexed(subset.index_count, subset.start_index, 0);

				registry->GetComponent<component::MaterialComponent>(entity).UnbindCBuffer();
			}
		}
	}

	void RenderPath::RenderSkyBox(ID3D11DeviceContext* immediate_context,
								  ecs::Registry* registry, mapping::rename_type::Entity entity,
								  const View* view, const Light* light)
	{
		auto& sky_box = registry->GetComponent<component::SkyBoxComponent>(entity);
		sky_box.ActivateShader(immediate_context);

		samplers.at(RenderingSampleState::Linear_Border)->Activate(immediate_context, 0);
		depth_stencil->Activate(immediate_context, DepthStencilState::Depth_First);
		rasterizer->Activate(immediate_context, RasterizeState::Cull_None);

		{// Transform
			//shader::CB_CoordinateTransformation cb{};
			//cb.mat_view_projection = view->GetView() * view->GetProjection();
			TransformCB transform{};

			transform.bone_transforms[0] = registry->GetComponent<component::TransformComponent>(entity).GetWorld4x4();
			registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);
		}

		locator::Locator::GetDx11Device()->BindPrimitiveTopology(mapping::graphics::PrimitiveTopology::TriangleList);

		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
													 sky_box.GetShaderResoueceViewAddress(),
													 TexSlot_SkyMap);

		// Set of Vertex Buffers
		UINT stride = sizeof(shader::Vertex);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, sky_box.GetVertexBufferAddress(), &stride, &offset);
		immediate_context->IASetIndexBuffer(sky_box.GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		immediate_context->DrawIndexed(36, 0, 0);

		sky_box_srv = sky_box.GetShaderResoueceView();
		sky_box.DeactivateShader(immediate_context);
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_SkyMap);;
		//off_screen->Deactivate(immediate_context);

		//Blit(immediate_context);
	}

	void RenderPath::RenderSprite(ID3D11DeviceContext* immediate_context,
								  ecs::Registry* const registry , const mapping::rename_type::Entity entity)
	{
		component::SpriteComponent&		sprite		= registry->GetComponent<component::SpriteComponent>(entity);
		component::TransformComponent&	transform	= registry->GetComponent<component::TransformComponent>(entity);

		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
													 sprite.GetTexture(),
													 TexSlot_BaseColorMap);

		// 2Dに必要ない値のリセット
		transform.SetPositionZ(0.0f);
		transform.SetScaleZ(1.f);
		transform.SetWorldRotation_X(.0f);
		transform.SetWorldRotation_Y(.0f);

		std::array<XMFLOAT2, 4> pos{};
		// Left top
		pos[0].x = 0.0f;
		pos[0].y = 0.0f;
		// Right top
		pos[1].x = static_cast<float>(sprite.VariableWidth());
		pos[1].y = 0.0f;
		// Left bottom
		pos[2].x = 0.0f;
		pos[2].y = static_cast<float>(sprite.VariableHeight());
		// Right bottom
		pos[3].x = static_cast<float>(sprite.VariableWidth());
		pos[3].y = static_cast<float>(sprite.VariableHeight());

		// Rotation
		{
			const float sin_val = sinf(DirectX::XMConvertToRadians(transform.GetWorldRotation().z));
			const float cos_val = cosf(DirectX::XMConvertToRadians(transform.GetWorldRotation().z));

			for (auto& p : pos)
			{// Rotate with the pivot as the origin

				// Pivot adjustment
				p.x -= sprite.GetSrcPivot().x;
				p.y -= sprite.GetSrcPivot().y;

				// Scale ajustment
				p.x *= (transform.GetScale().x);
				p.y *= (transform.GetScale().y);

				// Rotate
				const float oldX = p.x;
				p.x = p.x * cos_val - p.y * sin_val;
				p.y = oldX * sin_val + p.y * cos_val;

				// Move back in position
				p.x += transform.GetPosition().x;
				p.y += transform.GetPosition().y;
			}
		}

		{
			RECT rec{};
			const HWND desc_wnd_handle = registry->GetScene()->GetFramework()->GetWindow()->GetHWND();

			GetClientRect(desc_wnd_handle, &rec);

			// No off-screen sprites are drawn
			for (int i = 0; i < 4; ++i)
			{
				if ((pos[i].x >= 0 && pos[i].x <= rec.right) && (pos[i].y >= 0 && pos[i].y <= rec.bottom))
				{
					break;
				}
				if (i == 4)
				{
					return;
				}
			}

			// NDC transform
			{
				for (auto& p : pos)
				{
					p.x = p.x * 2.0f / rec.right - 1.0f;
					p.y = -p.y * 2.0f / rec.bottom + 1.0f;
				}
			}
		}

		// Update vertex buffer
		std::array<shader::VertexSprite, 4> vertex{};
		vertex.at(0).position = DirectX::XMFLOAT4{ pos[0].x,pos[0].y,.0f,1.f };
		vertex.at(1).position = DirectX::XMFLOAT4{ pos[1].x,pos[1].y,.0f,1.f };
		vertex.at(2).position = DirectX::XMFLOAT4{ pos[2].x,pos[2].y,.0f,1.f };
		vertex.at(3).position = DirectX::XMFLOAT4{ pos[3].x,pos[3].y,.0f,1.f };
		vertex.at(0).color	  = sprite.Color();
		vertex.at(1).color	  = sprite.Color();
		vertex.at(2).color	  = sprite.Color();
		vertex.at(3).color	  = sprite.Color();
		vertex.at(0).texcoord = sprite.GetVariableTexcoords().at(0);
		vertex.at(1).texcoord = sprite.GetVariableTexcoords().at(1);
		vertex.at(2).texcoord = sprite.GetVariableTexcoords().at(2);
		vertex.at(3).texcoord = sprite.GetVariableTexcoords().at(3);

		immediate_context->UpdateSubresource(sprite.GetVertexBuffer(), 0, NULL, vertex.data(), 0, 0);

		// Set of Vertex Buffers
		UINT stride = sizeof(shader::VertexSprite);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, sprite.GetVertexBufferAddress(), &stride, &offset);

		immediate_context->Draw(4, 0);
	}

	void RenderPath::RenderAnimSprite(ID3D11DeviceContext* immediate_context,
									  ecs::Registry* const registry, const mapping::rename_type::Entity entity)
	{
		component::AnimSpriteComponent& anim_sprite = registry->GetComponent<component::AnimSpriteComponent>(entity);
		component::TransformComponent&  transform	= registry->GetComponent<component::TransformComponent>(entity);

		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
													 anim_sprite.GetTexture(),
													 TexSlot_BaseColorMap);

		// 2Dに必要ない値のリセット
		transform.SetPositionZ(0.0f);
		transform.SetScaleZ(1.f);
		transform.SetWorldRotation_X(.0f);
		transform.SetWorldRotation_Y(.0f);

		std::array<XMFLOAT2, 4> pos{};
		// Left top
		pos[0].x = 0.0f;
		pos[0].y = 0.0f;
		// Right top
		pos[1].x = static_cast<float>(anim_sprite.VariableWidth() / anim_sprite.GetNumClip().x);
		pos[1].y = 0.0f;
		// Left bottom
		pos[2].x = 0.0f;
		pos[2].y = static_cast<float>(anim_sprite.VariableHeight() / anim_sprite.GetNumClip().y);
		// Right bottom
		pos[3].x = static_cast<float>(anim_sprite.VariableWidth() / anim_sprite.GetNumClip().x);
		pos[3].y = static_cast<float>(anim_sprite.VariableHeight() / anim_sprite.GetNumClip().y);


		const float sin_val = sinf(DirectX::XMConvertToRadians(transform.GetWorldRotation().z));
		const float cos_val = cosf(DirectX::XMConvertToRadians(transform.GetWorldRotation().z));

		for (auto& p : pos)
		{// Rotate with the pivot as the origin

			// Pivot adjustment
			p.x -=  anim_sprite.GetAnimPivot().x;
			p.y -=  anim_sprite.GetAnimPivot().y;

			// Scale ajustment
			p.x *= (transform.GetScale().x);
			p.y *= (transform.GetScale().y);

			// Rotate
			const float oldX = p.x;
			p.x = p.x * cos_val - p.y * sin_val;
			p.y = oldX * sin_val + p.y * cos_val;

			// Move back in position
			p.x += transform.GetPosition().x;
			p.y += transform.GetPosition().y;
		}

		{
			RECT rec{};
			const HWND desc_wnd_handle = registry->GetScene()->GetFramework()->GetWindow()->GetHWND();

			GetClientRect(desc_wnd_handle, &rec);

			// No off-screen sprites are drawn
			for (int i = 0; i < 4; ++i)
			{
				if ((pos[i].x >= 0 && pos[i].x <= rec.right) && (pos[i].y >= 0 && pos[i].y <= rec.bottom))
				{
					break;
				}
				if (i == 4)
				{
					return;
				}
			}

			// NDC transform
			{
				for (auto& p : pos)
				{
					p.x = p.x * 2.0f / rec.right - 1.0f;
					p.y = -p.y * 2.0f / rec.bottom + 1.0f;
				}
			}
		}

		// Update vertex buffer
		std::array<shader::VertexSprite, 4> vertex{};
		vertex.at(0).position	= DirectX::XMFLOAT4{ pos[0].x,pos[0].y,.0f,1.f };
		vertex.at(1).position	= DirectX::XMFLOAT4{ pos[1].x,pos[1].y,.0f,1.f };
		vertex.at(2).position	= DirectX::XMFLOAT4{ pos[2].x,pos[2].y,.0f,1.f };
		vertex.at(3).position	= DirectX::XMFLOAT4{ pos[3].x,pos[3].y,.0f,1.f };
		vertex.at(0).color		= anim_sprite.Color();
		vertex.at(1).color		= anim_sprite.Color();
		vertex.at(2).color		= anim_sprite.Color();
		vertex.at(3).color		= anim_sprite.Color();
		vertex.at(0).texcoord	= anim_sprite.GetAnimTexcoord().at(component::AnimSpriteComponent::TexSide::LeftTop);
		vertex.at(1).texcoord	= anim_sprite.GetAnimTexcoord().at(component::AnimSpriteComponent::TexSide::RightTop);
		vertex.at(2).texcoord	= anim_sprite.GetAnimTexcoord().at(component::AnimSpriteComponent::TexSide::LeftBottom);
		vertex.at(3).texcoord	= anim_sprite.GetAnimTexcoord().at(component::AnimSpriteComponent::TexSide::RightBottom);

		immediate_context->UpdateSubresource(anim_sprite.GetVertexBuffer(), 0, NULL, vertex.data(), 0, 0);

		// Set of Vertex Buffers
		UINT stride = sizeof(shader::VertexSprite);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, anim_sprite.GetVertexBufferAddress(), &stride, &offset);

		immediate_context->Draw(4, 0);
	}
}
