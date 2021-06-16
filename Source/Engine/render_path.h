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
		// ���ׂẴV�F�[�_�[�̐����ƃZ�b�g
		std::unique_ptr<shader::ShaderManager> shader_manager;

		//-- DirectX States --//
		std::unique_ptr<Blend>			blend;
		std::unique_ptr<Rasterizer>		rasterizer;
		std::unique_ptr<DepthStencil>	depth_stencil;
		std::array< std::unique_ptr<Sampler>, static_cast<int>(RenderingSampleState::End)> samplers;

		// TODO : �R���X�^���g�o�b�t�@�֘A���g�p����R���|�[�l���g�Ɏ����Ă���
		std::unique_ptr<buffer::ConstantBuffer<shader::CB_CoordinateTransformation>> cbuffer_transformation;
		std::unique_ptr<buffer::ConstantBuffer<shader::CB_Material>>					cbuffer_material;
		std::unique_ptr<buffer::ConstantBuffer<shader::CB_Light>>					cbuffer_light;

		std::unique_ptr<DummyTexture>		dummy_texture;
		std::unique_ptr<FrameBuffer>		back_buffer;
		std::unique_ptr<DepthMap>			depth_map;
		std::unique_ptr<FullscreenQuad>		fullscreen_quad;

		Locator* const locator;

		/*
		 * brief : "MeshObjectComponent"������DirectX state�̃Z�b�g
		 *         (states : rasterizer , sampler , depth_stencil , blend)
		 */
		void ActivateDirectXStates(ID3D11DeviceContext* immediate_context,
							       const component::MeshObjectComponent* mesh_object,
								   bool set_rasterizer = true, bool set_sampler = true,
								   bool set_depth_stencil = true, bool set_blend = true);

		/*
		 * brief     : �[�x�e�N�X�`���̍쐬
		 *             �쐬���ꂽ�e�N�X�`����
		 *		       Render3D()�ł�ShadowMap�̃��\�[�X�Ƃ��Ďg�p
		 * ��caution : "FBXModelComponent", "ObjModelComponent", "GeometricPrimitiveComponent"
		 *			   �����G���e�B�e�B�̂ݕ`�悳���
		 */
		void RenderShadow_Begin(ID3D11DeviceContext* immediate_context) const;
		void RenderShadow(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const View* view, const Light* light);
		void RenderShadow_End(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief     : 3D���f���̕`��
		 * ��caution : "FBXModelComponent", "ObjModelComponent", "GeometricPrimitiveComponent"
		 *			   �����G���e�B�e�B�̂ݕ`�悳���
		 */
		void Render3D_Begin(ID3D11DeviceContext* immediate_context);
		void Render3D(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const View* view, const Light* light);
		void Render3D_End(ID3D11DeviceContext* immediate_context);

		/*
		 * brief        : 2D�X�v���C�g�̕`��
		 * ��caution(1) : "SpriteObjectComponent", "SpriteComponent", "AnimSpriteComponent"
		 *                �����G���e�B�e�B�̂ݕ`�悳���
		 * ��caution(2) : "�o�b�N�o�b�t�@�ɒ��ڏ������ނ��߁uBegin�v��uEnd�v�͂Ȃ�"
		 */
		void Render2D(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		// 2D�X�v���C�g�`��(Animation�Ȃ�)
		void RenderSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		// 2D�X�v���C�g�`��(Animation����)
		void RenderAnimSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);


		//--------< ���f���̎�ނɉ����Ă̕`�� >--------//

		/*
		 * brief : "GeomPrimComponent"�������f���̕`��
		 */
		// TODO: RenderGeomPrim�����㓖���蔻��p�̃��f���̕`����s��
		void RenderGeomPrim(ID3D11DeviceContext* immediate_context,
							ecs::Registry* registry ,ecs::Entity entity,
							const component::MeshObjectComponent* mesh_object,
							const View* view, const Light* light);

		/*
		 * brief : "ObjModelComponent"�������f���̕`��
		 */
		void RenderOBJ(ID3D11DeviceContext* immediate_context,
					   ecs::Registry* registry, ecs::Entity entity,
					   const component::MeshObjectComponent* mesh_object,
					   const View* view, const Light* light);

		/*
		 * brief : "FbxModelComponent"�������f���̕`��
		 */
		void RenderFBX(ID3D11DeviceContext* immediate_context,
					   ecs::Registry* registry, ecs::Entity entity,
					   const component::MeshObjectComponent* mesh_object,
					   const View* view, const Light* light);

		/*
		 * brief : "SkyBoxComponent"�������f���̕`��
		 */
		void RenderSkyBox(ID3D11DeviceContext* immediate_context,
						  ecs::Registry* registry, ecs::Entity entity,
						  const component::MeshObjectComponent* mesh_object,
						  const View* view, const Light* light);
	};
}
