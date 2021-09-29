#include "render_path.h"

#include "debug_collision.h"
#include "gbuffer.h"
#include "graphics_mapping.h"
#include "sampler_mapping.h"
#include "scene.h"
#include "shader_interop_renderer.h"
#include "texture_resource_mapping.h"
#include "cum_imgui_helper.h"
#include "model_loader.h"
#include "material_loader.h"
// Components
#include "anim_sprite.h"
#include "camera_component.h"
#include "capsule_collison_component.h"
#include "fbx_model_component.h"
#include "mesh_object.h"
#include "sky_box.h"
#include "sphere_collision_component.h"
#include "sprite.h"
#include "transform_component.h"
#include "model_component.h"


namespace cumulonimbus::renderer
{
	RenderPath::RenderPath(ID3D11Device* device)
	{
		off_screen			= std::make_shared<FrameBuffer>(device, locator::Locator::GetWindow()->Width(), locator::Locator::GetWindow()->Height());
		fullscreen_quad		= std::make_unique<FullscreenQuad>(device);
		depth_map			= std::make_unique<DepthMap>(device);

		shader_manager				= std::make_unique<shader_system::ShaderManager>();
		shader_manager_2d			= std::make_unique<shader::SpriteShaderManager>(device);
		g_buffer					= std::make_unique<graphics::buffer::GBuffer>();
		local_shader_asset_manager	= std::make_unique<shader_asset::LocalShaderAssetManager>();

		blend				= std::make_unique<Blend>(device);
		depth_stencil		= std::make_unique<DepthStencil>(device);
		rasterizer			= std::make_unique<Rasterizer>(device);
		sampler				= std::make_unique<Sampler>(device);
		dummy_texture = std::make_unique<cumulonimbus::asset::DummyTexture>( device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f } );
	}

	void RenderPath::BindGraphicsState(
			ID3D11DeviceContext* immediate_context,
			const graphics::GraphicsState& graphics_state)
	{
		rasterizer->Activate(immediate_context, graphics_state.GetRasterizerState());
		sampler->Activate(immediate_context, graphics_state.GetSamplerState(), SSlot_OnDemand0);
		depth_stencil->Activate(immediate_context, graphics_state.GetDepthStencilState());
		blend->Activate(immediate_context, graphics_state.GetBlendState());
	}

	void RenderPath::RenderScene(
		ID3D11DeviceContext* immediate_context,ecs::Registry* registry,
		const camera::Camera* camera, const Light* light)
	{
		RenderBegin(immediate_context);
		camera->ClearFrameBuffer();

		if (registry->GetArray<component::ModelComponent>().GetComponents().size() > 0)
		{
			// ShadowMap作成
			RenderShadow_Begin(immediate_context);
			RenderShadow(immediate_context, registry, camera, light);
			RenderShadow_End(immediate_context);
		}

		// SkyBoxの描画
		RenderSkyBox_Begin(immediate_context, camera);
		RenderSkyBox(immediate_context, registry, camera, light);
		RenderSkyBox_End(immediate_context, camera);

		if (registry->GetArray<component::ModelComponent>().GetComponents().size() > 0)
		{
			// GBufferへの描画処理
			Render3DToGBuffer_Begin(immediate_context);
			Render3DToGBuffer(immediate_context, registry, camera, light);
			Render3DToGBuffer_End(immediate_context, camera);

			// 当たり判定の描画
			RenderCollision_Begin(immediate_context, camera);
			RenderCollision(immediate_context, registry);
			RenderCollision_End(immediate_context, camera);
		}

		// 2Dスプライトの描画
		Render2D(immediate_context, registry);

		RenderEnd(immediate_context);
	}


	void RenderPath::RenderGame(
		ID3D11DeviceContext* const immediate_context,
		ecs::Registry* registry,
		const Light* light)
	{
		RenderBegin(immediate_context);

		for (auto& camera_comp : registry->GetArray<component::CameraComponent>().GetComponents())
		{
			if (!camera_comp.GetIsActive())
				continue;
			camera_comp.GetCamera()->ClearFrameBuffer();

			if(registry->GetArray<component::ModelComponent>().GetComponents().size() > 0)
			{
				// ShadowMap作成
				RenderShadow_Begin(immediate_context);
				RenderShadow(immediate_context, registry, camera_comp.GetCamera(), light);
				RenderShadow_End(immediate_context);
			}

			// SkyBoxの描画
			RenderSkyBox_Begin(immediate_context, camera_comp.GetCamera());
			RenderSkyBox(immediate_context, registry, camera_comp.GetCamera(), light);
			RenderSkyBox_End(immediate_context, camera_comp.GetCamera());

			if (registry->GetArray<component::ModelComponent>().GetComponents().size() > 0)
			{
				// GBufferへの描画処理
				Render3DToGBuffer_Begin(immediate_context);
				Render3DToGBuffer(immediate_context, registry, camera_comp.GetCamera(), light);
				Render3DToGBuffer_End(immediate_context, camera_comp.GetCamera());

				// 当たり判定の描画
				RenderCollision_Begin(immediate_context, camera_comp.GetCamera());
				RenderCollision(immediate_context, registry);
				RenderCollision_End(immediate_context, camera_comp.GetCamera());
			}
		}

		//// ポストプロセス処理
		//RenderPostProcess_Begin(immediate_context);
		//RenderPostProcess(immediate_context, registry);
		//RenderPostProcess_End(immediate_context);

		// 2Dスプライトの描画
		Render2D(immediate_context, registry);

		RenderEnd(immediate_context);

		for (auto& camera_comp : registry->GetArray<component::CameraComponent>().GetComponents())
		{
			if (!camera_comp.GetIsMainCamera())
				continue;

			if (ImGui::Begin("RTV"))
			{
				if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					ImGui::Text("True");
				}
				const auto size = ImGui::GetWindowSize();
				helper::imgui::Image(*camera_comp.GetCamera()->GetFrameBufferSRV_Address(), { size.x,size.y });
			}
			ImGui::End();
		}
	}

	void RenderPath::RenderBegin(ID3D11DeviceContext* immediate_context)
	{
		//off_screen->Clear(immediate_context);
		//off_screen->Activate(immediate_context);
	}

	void RenderPath::RenderEnd(ID3D11DeviceContext* immediate_context)
	{

	}

	void RenderPath::RenderShadow_Begin(ID3D11DeviceContext* const immediate_context) const
	{
		depth_map->Begin(immediate_context);
	}

	void RenderPath::RenderShadow(
		ID3D11DeviceContext* immediate_context,
		ecs::Registry* const registry,
		const camera::Camera* camera,
		const Light* light)
	{
		depth_map->Activate(immediate_context);
		light->BindCBuffer();

		for (auto& model_comp : registry->GetArray<component::ModelComponent>().GetComponents())
		{
			const mapping::rename_type::Entity ent = model_comp.GetEntity();

			RenderModel(immediate_context, registry, ent, &model_comp, camera, light);
		}

		light->UnbindCBuffer();
		depth_map->Deactivate(immediate_context);
	}

	void RenderPath::RenderShadow_End(ID3D11DeviceContext* immediate_context) const
	{
		depth_map->End(immediate_context);
	}

	void RenderPath::RenderSkyBox_Begin(ID3D11DeviceContext* immediate_context,
										const camera::Camera* camera)
	{
		camera->BindFrameBuffer();
		//off_screen->Clear(immediate_context);
		//off_screen->Activate(immediate_context);
	}

	void RenderPath::RenderSkyBox(
		ID3D11DeviceContext* immediate_context,
		ecs::Registry* registry,
		const camera::Camera* camera,
		const Light* light)
	{
		// ライトパラメータをコンスタントバッファにバインド
		light->BindCBuffer();

		auto& components = registry->GetArray<component::SkyBoxComponent>().GetComponents();
		for (auto& sky_box : registry->GetArray<component::SkyBoxComponent>().GetComponents())
		{
			mapping::rename_type::Entity ent = sky_box.GetEntity();
			RenderSkyBox(immediate_context, registry, sky_box.GetEntity(), camera, light);
		}
	}

	void RenderPath::RenderSkyBox_End(
		ID3D11DeviceContext* immediate_context,
		const camera::Camera* camera)
	{
		// Skyマップをpixel shaderのshader resource viewにバインド
		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, sky_box_srv.GetAddressOf(), TexSlot_SkyMap);
		//off_screen->Deactivate(immediate_context);
		camera->UnbindFrameBuffer();
	}

	void RenderPath::Render3DToGBuffer_Begin(ID3D11DeviceContext* immediate_context) const
	{
		// GBuffer用RTVのクリア
		g_buffer->Clear(0, 0, 0, 0);
		// GBuffer用のシェーダーとRTVのセット
		g_buffer->BindShaderAndRTV();
	}

	void RenderPath::Render3DToGBuffer(
		ID3D11DeviceContext* immediate_context,
		ecs::Registry* registry,
		const camera::Camera* camera,
		const Light* light)
	{
		for(auto& model_comp : registry->GetArray<component::ModelComponent>().GetComponents())
		{
			const mapping::rename_type::Entity ent = model_comp.GetEntity();

			RenderModel(immediate_context, registry, ent, &model_comp, camera, light);
		}
	}

	void RenderPath::Render3DToGBuffer_End(
		ID3D11DeviceContext* immediate_context,
		const camera::Camera* camera) const
	{
		g_buffer->UnbindShaderAndRTV();

		camera->BindFrameBuffer();
		// GBufferに書き出したテクスチャのセット
		g_buffer->BindGBufferTextures();
		// GBufferライティング用シェーダー(PS)のセット
		g_buffer->BindGBuffLightingShader();
		fullscreen_quad->Blit(immediate_context);

		g_buffer->UnbindGBufferTextures();

		camera->UnbindFrameBuffer();

		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, 0);
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, 1);
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, 2);

		immediate_context->VSSetShader(nullptr, nullptr, 0);
		immediate_context->PSSetShader(nullptr, nullptr, 0);
	}

	void RenderPath::RenderPostProcess_Begin(ID3D11DeviceContext* immediate_context)
	{

	}

	void RenderPath::RenderPostProcess(ID3D11DeviceContext* immediate_context, ecs::Registry* registry)
	{
		for(auto& camera_comp : registry->GetArray<component::CameraComponent>().GetComponents())
		{
			if(!camera_comp.GetIsMainCamera())
				continue;

			locator::Locator::GetDx11Device()->BindShaderResource(
				mapping::graphics::ShaderStage::PS,
				camera_comp.GetCamera()->GetFrameBufferSRV_Address(),
				TexSlot_BaseColorMap);
			fullscreen_quad->Blit(immediate_context, true, true, true);
			ID3D11ShaderResourceView* const pSRV[1] = { nullptr };
		}
	}

	void RenderPath::RenderPostProcess_End(ID3D11DeviceContext* immediate_context)
	{

	}

	void RenderPath::Render3D_Begin(ID3D11DeviceContext* immediate_context)
	{
		g_buffer->Clear();
		g_buffer->BindRTV();
	}

	void RenderPath::Render3D(
		ID3D11DeviceContext* immediate_context,
		ecs::Registry* registry,
		const camera::Camera* camera,
		const Light* light)
	{
		// ライトパラメータをコンスタントバッファにバインド
		light->BindCBuffer();

		auto& components = registry->GetArray<component::SkyBoxComponent>().GetComponents();
		for(auto& sky_box : registry->GetArray<component::SkyBoxComponent>().GetComponents())
		{
			mapping::rename_type::Entity ent = sky_box.GetEntity();
			RenderSkyBox(immediate_context, registry, sky_box.GetEntity(), camera, light);
		}

		// Skyマップをpixel shaderのshader resource viewにバインド
		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, sky_box_srv.GetAddressOf(), TexSlot_SkyMap);

		light->UnbindCBuffer();
	}

	void RenderPath::Render3D_End(
			ID3D11DeviceContext* immediate_context,
			const camera::Camera* camera)
	{
		g_buffer->UnbindRTV();
		//off_screen->Activate(immediate_context);
		camera->BindFrameBuffer();
		g_buffer->BindColorTexture();
		fullscreen_quad->Blit(immediate_context, true, true, true);
		//off_screen->Deactivate(immediate_context);
		camera->UnbindFrameBuffer();

		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, 0);
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, 1);
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, 2);

		immediate_context->VSSetShader(nullptr, nullptr, 0);
		immediate_context->PSSetShader(nullptr, nullptr, 0);
	}

	void RenderPath::Render2D(ID3D11DeviceContext* immediate_context,
							  ecs::Registry* registry)
	{
		for(auto& sprite : registry->GetArray<component::OldSpriteComponent>().GetComponents())
		{
			BindGraphicsState(immediate_context, sprite.GetGraphicsState());
			shader_manager_2d->Activate(immediate_context, sprite.GetShaderState());
			RenderSprite(immediate_context, registry, sprite.GetEntity());
			shader_manager_2d->Deactivate(immediate_context, sprite.GetShaderState());
		}

		for(auto& anim_sprite : registry->GetArray<component::OldAnimSpriteComponent>().GetComponents())
		{
			BindGraphicsState(immediate_context, anim_sprite.GetGraphicsState());
			shader_manager_2d->Activate(immediate_context, anim_sprite.GetShaderState());
			RenderSprite(immediate_context, registry, anim_sprite.GetEntity());
			RenderAnimSprite(immediate_context, registry, anim_sprite.GetEntity());
			shader_manager_2d->Deactivate(immediate_context, anim_sprite.GetShaderState());
		}
	}

	void RenderPath::Blit(ID3D11DeviceContext* immediate_context) const
	{
		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS,
															  off_screen->GetRenderTargetSRV(),
															  TexSlot_BaseColorMap);
		fullscreen_quad->Blit(immediate_context, true, true, true);
	}

	void RenderPath::RenderModel(
		ID3D11DeviceContext* immediate_context,
		ecs::Registry* registry, mapping::rename_type::Entity entity,
		const component::ModelComponent* model_comp,
		const camera::Camera* camera, const Light* light)
	{
		if (!locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->HasModel(model_comp->GetModelID()))
			return;

		const asset::ModelData& model_data =
			locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_comp->GetModelID()).GetModelData();

		BindGraphicsState(immediate_context, model_comp->GetGraphicsState());

		for (const auto& mesh : model_data.meshes)
		{
			// メッシュ単位コンスタントバッファ更新
			TransformCB transform{};

			if (mesh.node_indices.size() > 0)
			{
				for (size_t i = 0; i < mesh.node_indices.size(); ++i)
				{
					DirectX::SimpleMath::Matrix reverse;
					reverse.Right(DirectX::SimpleMath::Vector3(-1, 0, 0));

					DirectX::SimpleMath::Matrix world_transform		= DirectX::XMLoadFloat4x4(&model_comp->GetNodes().at(mesh.node_indices.at(i)).world_transform);
					DirectX::SimpleMath::Matrix inverse_transform	= DirectX::XMLoadFloat4x4(&mesh.inverse_transforms.at(i));
					DirectX::SimpleMath::Matrix bone_transform		= inverse_transform * world_transform;
					XMStoreFloat4x4(&transform.bone_transforms[i], bone_transform);
				}
			}
			else
			{
				transform.bone_transforms[0] = registry->GetComponent<component::TransformComponent>(entity).GetWorld4x4();
			}

			registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);
			camera->BindCBuffer();

			UINT stride = sizeof(shader::Vertex);
			UINT offset = 0;
			immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
			immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			locator::Locator::GetDx11Device()->BindPrimitiveTopology(mapping::graphics::PrimitiveTopology::TriangleList);

			for (const asset::ModelData::Subset& subset : mesh.subsets)
			{
				const auto material_id = model_comp->GetMaterialID(subset.material_index);
				// マテリアルのバインド
				locator::Locator::GetAssetManager()->GetLoader<asset::MaterialLoader>()->GetMaterial(material_id).BindMaterial(mapping::graphics::ShaderStage::PS);
				immediate_context->DrawIndexed(subset.index_count, subset.start_index, 0);
				// マテリアルのアンバインド
				locator::Locator::GetAssetManager()->GetLoader<asset::MaterialLoader>()->GetMaterial(material_id).UnbindMaterial(mapping::graphics::ShaderStage::PS);

			}
		}
	}


	void RenderPath::RenderSkyBox(
		ID3D11DeviceContext* immediate_context,
		ecs::Registry* registry, mapping::rename_type::Entity entity,
		const camera::Camera* camera, const Light* light)
	{
		auto& sky_box = registry->GetComponent<component::SkyBoxComponent>(entity);
		sky_box.ActivateShader(immediate_context);

		sampler->Activate(immediate_context, SamplerState::Linear_Border, 0);
		depth_stencil->Activate(immediate_context, DepthStencilState::Depth_First);
		rasterizer->Activate(immediate_context, RasterizeState::Cull_None);

		{// Transform
			//shader::CB_CoordinateTransformation cb{};
			//cb.mat_view_projection = view->GetViewMat() * view->GetProjectionMat();
			TransformCB transform{};

			transform.bone_transforms[0] = registry->GetComponent<component::TransformComponent>(entity).GetWorld4x4();
			registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);
		}

		locator::Locator::GetDx11Device()->BindPrimitiveTopology(mapping::graphics::PrimitiveTopology::TriangleList);

		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												sky_box.GetShaderResoueceViewAddress(),
												TexSlot_SkyMap);
		camera->BindCBuffer();
		// Set of Vertex Buffers
		UINT stride = sizeof(shader::Vertex);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, sky_box.GetVertexBufferAddress(), &stride, &offset);
		immediate_context->IASetIndexBuffer(sky_box.GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		immediate_context->DrawIndexed(36, 0, 0);

		sky_box_srv = sky_box.GetShaderResoueceView();
		sky_box.DeactivateShader(immediate_context);
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_SkyMap);
		camera->UnbindCBuffer();
		//off_screen->Deactivate(immediate_context);

		//Blit(immediate_context);
	}

	void RenderPath::RenderCollision_Begin(
		ID3D11DeviceContext* immediate_context,
		const camera::Camera* camera)
	{
		// off_screen->Activate(immediate_context);
		camera->BindFrameBuffer();
		// DirectX graphics stateのバインド
		rasterizer->Activate(immediate_context, RasterizeState::Wireframe);
		depth_stencil->Activate(immediate_context, DepthStencilState::DepthTest_True_Write_True);
		blend->Activate(immediate_context, BlendState::Alpha);
		// シェーダーのバインド
		shader_manager->BindLocalShader(mapping::shader_assets::LocalShaderAsset::Collision);
		local_shader_asset_manager->BindCBuffer(mapping::shader_assets::LocalShaderAsset::Collision);
		// ビュー行列のバインド
		camera->BindCBuffer();

	}

	void RenderPath::RenderCollision(ID3D11DeviceContext* immediate_context, ecs::Registry* registry)
	{
		for (auto& sphere_collision : registry->GetArray<component::SphereCollisionComponent>().GetComponents())
		{
			const mapping::rename_type::Entity ent = sphere_collision.GetEntity();
			const auto& model_resource = locator::Locator::GetResourceManager()->FbxModelResouece("sphere");
			RenderSphereCollisionModel(immediate_context, registry, ent, model_resource.get());
		}
		for(auto& capsule_collision : registry->GetArray<component::CapsuleCollisionComponent>().GetComponents())
		{
			const mapping::rename_type::Entity ent = capsule_collision.GetEntity();
			const auto& model_resource = locator::Locator::GetResourceManager()->FbxModelResouece("capsule");
			RenderCapsuleCollisionModel(immediate_context, registry, ent, model_resource.get());
		}
	}

	void RenderPath::RenderSphereCollisionModel(ID3D11DeviceContext* immediate_context,
										  ecs::Registry* registry, mapping::rename_type::Entity entity,
										  const FbxModelResource* model_resource)
	{
		auto& sphere_collision = registry->GetComponent<component::SphereCollisionComponent>(entity);
		for (const auto& sphere : sphere_collision.GetSpheres())
		{
			for (const auto& mesh : model_resource->GetModelData().meshes)
			{
				// メッシュ単位コンスタントバッファ更新
				TransformCB transform{};
				transform.bone_transforms[0] = sphere.second.world_transform_matrix;

				registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);

				UINT stride = sizeof(shader::Vertex);
				UINT offset = 0;
				immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
				immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				locator::Locator::GetDx11Device()->BindPrimitiveTopology(mapping::graphics::PrimitiveTopology::TriangleList);

				for (const ModelData::Subset& subset : mesh.subsets)
				{

					//DebugCollisionCB cb_collision;
					//if (sphere.second.is_hit)
					//{
					//	cb_collision.collider_color = { 1.f,.0f,.0f,1.f };
					//}
					//else
					//{
					//	cb_collision.collider_color = { .0f,.0f,1.f,1.f };
					//}
					immediate_context->DrawIndexed(subset.index_count, subset.start_index, 0);
				}
			}
		}
	}

	void RenderPath::RenderCapsuleCollisionModel(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, mapping::rename_type::Entity entity, const FbxModelResource* model_resource)
	{
		auto& capsule_collision = registry->GetComponent<component::CapsuleCollisionComponent>(entity);
		for (auto& capsule : capsule_collision.GetCapsules())
		{
			for (const auto& mesh : model_resource->GetModelData().meshes)
			{
				// メッシュ単位コンスタントバッファ更新
				TransformCB transform{};
				transform.bone_transforms[0] = capsule.second.world_transform_matrix;

				registry->GetComponent<component::TransformComponent>(entity).SetAndBindCBuffer(transform);

				UINT stride = sizeof(shader::Vertex);
				UINT offset = 0;
				immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
				immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				locator::Locator::GetDx11Device()->BindPrimitiveTopology(mapping::graphics::PrimitiveTopology::TriangleList);

				for (const ModelData::Subset& subset : mesh.subsets)
				{

					//DebugCollisionCB cb_collision;
					//if (sphere.second.is_hit)
					//{
					//	cb_collision.collider_color = { 1.f,.0f,.0f,1.f };
					//}
					//else
					//{
					//	cb_collision.collider_color = { .0f,.0f,1.f,1.f };
					//}
					immediate_context->DrawIndexed(subset.index_count, subset.start_index, 0);
				}
			}
		}
	}


	void RenderPath::RenderCollision_End(
		ID3D11DeviceContext* immediate_context,
		const camera::Camera* camera)
	{
		// ビュー行列のバインド
		camera->UnbindCBuffer();
		// シェーダーのバインド
		shader_manager->UnbindLocalShader(mapping::shader_assets::LocalShaderAsset::Collision);
		local_shader_asset_manager->UnbindCBuffer(mapping::shader_assets::LocalShaderAsset::Collision);
		//off_screen->Deactivate(immediate_context);
		camera->UnbindFrameBuffer();
	}

	void RenderPath::RenderSprite(ID3D11DeviceContext* immediate_context,
								  ecs::Registry* const registry , const mapping::rename_type::Entity entity)
	{
		component::OldSpriteComponent&		sprite		= registry->GetComponent<component::OldSpriteComponent>(entity);
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
		component::OldAnimSpriteComponent& anim_sprite = registry->GetComponent<component::OldAnimSpriteComponent>(entity);
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
		vertex.at(0).texcoord	= anim_sprite.GetAnimTexcoord().at(component::OldAnimSpriteComponent::TexSide::LeftTop);
		vertex.at(1).texcoord	= anim_sprite.GetAnimTexcoord().at(component::OldAnimSpriteComponent::TexSide::RightTop);
		vertex.at(2).texcoord	= anim_sprite.GetAnimTexcoord().at(component::OldAnimSpriteComponent::TexSide::LeftBottom);
		vertex.at(3).texcoord	= anim_sprite.GetAnimTexcoord().at(component::OldAnimSpriteComponent::TexSide::RightBottom);

		immediate_context->UpdateSubresource(anim_sprite.GetVertexBuffer(), 0, NULL, vertex.data(), 0, 0);

		// Set of Vertex Buffers
		UINT stride = sizeof(shader::VertexSprite);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, anim_sprite.GetVertexBufferAddress(), &stride, &offset);

		immediate_context->Draw(4, 0);
	}

	void RenderPath::CombinationGBuffer() const
	{
		//g_buffer->

		//blend->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), BlendState::Alpha);
		//for(const auto& gbuffer : shader_manager->GetGBufferMap())
		//{
		//	if(gbuffer.second->GetIsUsedGBuffer())
		//	{
		//		shader_manager->BindShader(mapping::shader_assets::ShaderAsset3D::SampleShader);
		//		using namespace mapping::graphics;
		//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, gbuffer.second->GetColorBufferSRV_Address()	 , TexSlot_BaseColorMap);
		//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, gbuffer.second->GetNormalBufferSRV_Address()	 , TexSlot_NormalMap);
		//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, gbuffer.second->GetPositionBufferSRV_Address(), TexSlot_Position);
		//		fullscreen_quad->Blit(locator::Locator::GetDx11Device()->immediate_context.Get());
		//		shader_manager->UnbindShader(mapping::shader_assets::ShaderAsset3D::SampleShader);
		//	}
		//}
	}


}
