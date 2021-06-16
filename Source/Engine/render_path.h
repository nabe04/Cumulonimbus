#pragma once
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "sampler.h"
#include "ecs.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	}

	namespace renderer
	{
		class DepthMap;
	}

	namespace component
	{
		class MeshObjectComponent;
		class GeomPrimComponent;
	}

}

namespace shader
{
	class  ShaderManager;
	struct CB_CoordinateTransformation;
	struct CB_Material;
	struct CB_Light;
}

class Blend;
class DepthStencil;
class Sampler;
class Rasterizer;
class View;
class Light;
class DummyTexture;
class FrameBuffer;
class FullscreenQuad;
class Locator;

namespace cumulonimbus::renderer
{
	class RenderPath final
	{
	public:
		explicit RenderPath(ID3D11Device* device, Locator* locator);

		void Render(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const View* view, const Light* light);

	private:
		// すべてのシェーダーの生成とセット
		std::unique_ptr<shader::ShaderManager> shader_manager;

		//-- DirectX States --//
		std::unique_ptr<Blend>			blend;
		std::unique_ptr<Rasterizer>		rasterizer;
		std::unique_ptr<DepthStencil>	depth_stencil;
		std::array< std::unique_ptr<Sampler>, static_cast<int>(RenderingSampleState::End)> samplers;

		// TODO : コンスタントバッファ関連を使用するコンポーネントに持っていく
		std::unique_ptr<buffer::ConstantBuffer<shader::CB_CoordinateTransformation>> cbuffer_transformation;
		std::unique_ptr<buffer::ConstantBuffer<shader::CB_Material>>					cbuffer_material;
		std::unique_ptr<buffer::ConstantBuffer<shader::CB_Light>>					cbuffer_light;

		std::unique_ptr<DummyTexture>		dummy_texture;
		std::unique_ptr<FrameBuffer>		back_buffer;
		std::unique_ptr<DepthMap>			depth_map;
		std::unique_ptr<FullscreenQuad>		fullscreen_quad;

		Locator* const locator;

		/*
		 * brief : "MeshObjectComponent"がもつDirectX stateのセット
		 *         (states : rasterizer , sampler , depth_stencil , blend)
		 */
		void ActivateDirectXStates(ID3D11DeviceContext* immediate_context,
							       const component::MeshObjectComponent* mesh_object,
								   bool set_rasterizer = true, bool set_sampler = true,
								   bool set_depth_stencil = true, bool set_blend = true);

		/*
		 * brief     : 深度テクスチャの作成
		 *             作成されたテクスチャは
		 *		       Render3D()でのShadowMapのリソースとして使用
		 * ※caution : "FBXModelComponent", "ObjModelComponent", "GeometricPrimitiveComponent"
		 *			   を持つエンティティのみ描画される
		 */
		void RenderShadow_Begin(ID3D11DeviceContext* immediate_context) const;
		void RenderShadow(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const View* view, const Light* light);
		void RenderShadow_End(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief     : 3Dモデルの描画
		 * ※caution : "FBXModelComponent", "ObjModelComponent", "GeometricPrimitiveComponent"
		 *			   を持つエンティティのみ描画される
		 */
		void Render3D_Begin(ID3D11DeviceContext* immediate_context);
		void Render3D(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const View* view, const Light* light);
		void Render3D_End(ID3D11DeviceContext* immediate_context);

		/*
		 * brief        : 2Dスプライトの描画
		 * ※caution(1) : "SpriteObjectComponent", "SpriteComponent", "AnimSpriteComponent"
		 *                を持つエンティティのみ描画される
		 * ※caution(2) : "バックバッファに直接書き込むため「Begin」や「End」はない"
		 */
		void Render2D(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		// 2Dスプライト描画(Animationなし)
		void RenderSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		// 2Dスプライト描画(Animationあり)
		void RenderAnimSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);


		//--------< モデルの種類に応じての描画 >--------//

		/*
		 * brief : "GeomPrimComponent"が持つモデルの描画
		 */
		// TODO: RenderGeomPrimを今後当たり判定用のモデルの描画も行う
		void RenderGeomPrim(ID3D11DeviceContext* immediate_context,
							ecs::Registry* registry ,ecs::Entity entity,
							const component::MeshObjectComponent* mesh_object,
							const View* view, const Light* light);

		/*
		 * brief : "ObjModelComponent"が持つモデルの描画
		 */
		void RenderOBJ(ID3D11DeviceContext* immediate_context,
					   ecs::Registry* registry, ecs::Entity entity,
					   const component::MeshObjectComponent* mesh_object,
					   const View* view, const Light* light);

		/*
		 * brief : "FbxModelComponent"が持つモデルの描画
		 */
		void RenderFBX(ID3D11DeviceContext* immediate_context,
					   ecs::Registry* registry, ecs::Entity entity,
					   const component::MeshObjectComponent* mesh_object,
					   const View* view, const Light* light);

		/*
		 * brief : "SkyBoxComponent"が持つモデルの描画
		 */
		void RenderSkyBox(ID3D11DeviceContext* immediate_context,
						  ecs::Registry* registry, ecs::Entity entity,
						  const component::MeshObjectComponent* mesh_object,
						  const View* view, const Light* light);
	};
}
