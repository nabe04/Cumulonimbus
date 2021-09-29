#pragma once
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "blend.h"
#include "constant_buffer.h"
#include "depth_map.h"
#include "depth_stencil.h"
#include "frame_buffer.h"
#include "fullscreen_quad.h"
#include "graphics_state.h"
#include "light.h"
#include "local_shader_manager.h"
#include "rasterizer.h"
#include "rename_type_mapping.h"
#include "sampler.h"
#include "shader_manager.h"
#include "texture.h"

enum class CollisionType;
class FbxModelResource;

namespace cumulonimbus
{
	namespace camera
	{
		class Camera;
	} // camera

	namespace ecs
	{
		class Registry;
	} // ecs

	namespace renderer
	{
		class DepthMap;
	} // renderer

	namespace component
	{
		class MeshObjectComponent;
		class GeomPrimComponent;
		class ModelComponent;
	} // component

	namespace graphics::buffer
	{
		class GBuffer;
	} // graphics::buffer

} // cumulonimbus

namespace cumulonimbus::renderer
{
	class RenderPath final
	{
	public:
		explicit RenderPath(ID3D11Device* device);

		/**
		 * @brief : Scene Viewの描画
		 */
		void RenderScene(
			ID3D11DeviceContext* immediate_context, ecs::Registry* registry,
			const camera::Camera* camera, const Light* light);
		/**
		 * @brief : Game Viewの描画
		 */
		void RenderGame(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry, const Light* light);

	private:
		// すべてのシェーダーの生成とセット
		std::unique_ptr<shader_system::ShaderManager> shader_manager;
		std::unique_ptr<shader::SpriteShaderManager>  shader_manager_2d;

		//-- DirectX States --//
		std::unique_ptr<Blend>			blend;
		std::unique_ptr<Rasterizer>		rasterizer;
		std::unique_ptr<DepthStencil>	depth_stencil;
		std::unique_ptr<Sampler>		sampler;

		std::shared_ptr<FrameBuffer>							off_screen					{ nullptr };
		std::unique_ptr<graphics::buffer::GBuffer>				g_buffer					{ nullptr };
		std::unique_ptr<cumulonimbus::asset::DummyTexture>		dummy_texture				{ nullptr };
		std::unique_ptr<DepthMap>								depth_map					{ nullptr };
		std::unique_ptr<FullscreenQuad>							fullscreen_quad				{ nullptr };
		std::unique_ptr<shader_asset::LocalShaderAssetManager>	local_shader_asset_manager	{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		sky_box_srv					{ nullptr };
		//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>

		// Todo : BindDirectXStates関数は消す
		/*
		 * brief : "MeshObjectComponent"がもつDirectX stateのセット
		 *         (states : rasterizer , sampler , depth_stencil , blend)
		 */
		void BindGraphicsState(
				ID3D11DeviceContext* immediate_context,
				const cumulonimbus::graphics::GraphicsState& graphics_state);

		/*
		 * brief : "back_buffer"(メンバ変数)に書き込まれているものを
		 *		   バックバッファに書き込む
		 */
		void Blit(ID3D11DeviceContext* immediate_context) const;

		/**
		 * @brief : 全体の描画前処理
		 */
		void RenderBegin(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : 全体の描画後処理
		 */
		void RenderEnd(ID3D11DeviceContext* immediate_context);

		/*
		 * brief     : 深度テクスチャの作成
		 *             作成されたテクスチャは
		 *		       Render3D()でのShadowMapのリソースとして使用
		 * ※caution : "FBXModelComponent"
		 *			   を持つエンティティのみ描画される
		 */
		void RenderShadow_Begin(ID3D11DeviceContext* immediate_context) const;
		void RenderShadow(
			ID3D11DeviceContext*  immediate_context,
			ecs::Registry*		  registry,
			const camera::Camera* camera,
			const Light*		  light);
		void RenderShadow_End(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : SkyBoxの描画
		 *		   off_screenバッファを使用して書き込む
		 */
		void RenderSkyBox_Begin(
				ID3D11DeviceContext*	immediate_context,
				const camera::Camera*	camera);
		void RenderSkyBox(
				ID3D11DeviceContext*  immediate_context,
				ecs::Registry*		  registry,
				const camera::Camera* camera,
				const Light*		 light);
		void RenderSkyBox_End(
				ID3D11DeviceContext* immediate_context,
				const camera::Camera* camera);

		/*
		 * brief     : 3DモデルのGBufferへの描画
		 */
		/*
		 * brief     : GBuffer用のRTVのクリア、セット、シェーダーのセット
		 * ※caution : シェーダをBeginの段階でセットするのでGBufferの描画中は
		 *			   シェーダをセットしないようにする
		 */
		void Render3DToGBuffer_Begin(ID3D11DeviceContext* immediate_context) const;
		/*
		 * brief : GBufferを描画先にしたモデルの描画
		 */
		void Render3DToGBuffer(
			ID3D11DeviceContext*  immediate_context,
			ecs::Registry*		  registry,
			const camera::Camera* camera,
			const Light*		  light);
		/*
		 * brier     : 各々のシェーダーに応じて作成したGBufferをoff_screenにまとめる
		 * ※caution : Render3DToGBuffer_Begin関数でセットしたGBuffer用のシェーダアンバインドする
		 */
		void Render3DToGBuffer_End(
			ID3D11DeviceContext* immediate_context,
			const camera::Camera* camera) const;

		/*
		 * brief : ポストプロセス処理
		 *		   (Bloom)
		 */
		void RenderPostProcess_Begin(ID3D11DeviceContext* immediate_context);
		void RenderPostProcess(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		void RenderPostProcess_End(ID3D11DeviceContext* immediate_context);

		/*
		 * brief     : 3Dモデルの描画
		 * ※caution : "FBXModelComponent"
		 *			   を持つエンティティのみ描画される
		 */
		void Render3D_Begin(ID3D11DeviceContext* immediate_context);
		void Render3D(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry,
			const camera::Camera* camera,
			const Light* light);
		void Render3D_End(
				ID3D11DeviceContext* immediate_context,
				const camera::Camera* camera);

		void Render2D(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);

		/**
		 * @brief : "ModelComponent"の持つモデルの描画
		 */
		void RenderModel(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry, mapping::rename_type::Entity entity,
			const component::ModelComponent* model_comp,
			const camera::Camera* camera, const Light* light);

		/**
		 * @brief :   "SkyBoxComponent"が持つモデルの描画
		 */
		void RenderSkyBox(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry, mapping::rename_type::Entity entity,
			const camera::Camera* camera, const Light* light);

		/*
		 * brief : 当たり判定の描画
		 */
		void RenderCollision_Begin(
				ID3D11DeviceContext* immediate_context,
				const camera::Camera* camera);
		void RenderCollision(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		void RenderSphereCollisionModel(
				ID3D11DeviceContext* immediate_context,
				ecs::Registry* registry, mapping::rename_type::Entity entity,
				const FbxModelResource* model_resource);
		void RenderCapsuleCollisionModel(
				ID3D11DeviceContext* immediate_context,
				ecs::Registry* registry, mapping::rename_type::Entity entity,
				const FbxModelResource* model_resource);
		void RenderCollision_End(
				ID3D11DeviceContext* immediate_context,
				const camera::Camera* camera);

		/*
		 * brief        : 2Dスプライトの描画
		 * ※caution(1) : "OldSpriteComponent", "OldAnimSpriteComponent"
		 *                を持つエンティティのみ描画される
		 * ※caution(2) : "バックバッファに直接書き込むため「Begin」や「End」はない"
		 */
		// 2Dスプライト描画(Animationなし)
		void RenderSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, mapping::rename_type::Entity entity);
		// 2Dスプライト描画(Animationあり)
		void RenderAnimSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, mapping::rename_type::Entity entity);

		/*
		 * brief     : GBufferをoff_screenに結合
		 * ※caution : FrameBufferのセットはこの関数では行わない
		 */
		void CombinationGBuffer() const;
	};
} //  cumulonimbus::renderer
