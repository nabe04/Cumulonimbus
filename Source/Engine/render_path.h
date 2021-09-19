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
		class CameraComponent;
		class MeshObjectComponent;
		class SpriteObjectComponent;
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

		void Render(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const Light* light);

	private:
		// ���ׂẴV�F�[�_�[�̐����ƃZ�b�g
		std::unique_ptr<shader_system::ShaderManager> shader_manager;
		std::unique_ptr<shader::SpriteShaderManager> shader_manager_2d;

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

		// Todo : BindDirectXStates�֐��͏���
		/*
		 * brief : "MeshObjectComponent"������DirectX state�̃Z�b�g
		 *         (states : rasterizer , sampler , depth_stencil , blend)
		 */
		void BindDirectXStates(
				ID3D11DeviceContext* immediate_context,
				const component::SpriteObjectComponent* sprite_object,
			    bool set_rasterizer = true	, bool set_sampler = true,
				bool set_depth_stencil = true, bool set_blend = true);
		void BindGraphicsState(
				ID3D11DeviceContext* immediate_context,
				const cumulonimbus::graphics::GraphicsState& graphics_state);

		/*
		 * brief : "back_buffer"(�����o�ϐ�)�ɏ������܂�Ă�����̂�
		 *		   �o�b�N�o�b�t�@�ɏ�������
		 */
		void Blit(ID3D11DeviceContext* immediate_context) const;

		/**
		 * @brief : �S�̂̕`��O����
		 */
		void RenderBegin(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : �S�̂̕`��㏈��
		 */
		void RenderEnd(ID3D11DeviceContext* immediate_context);

		/*
		 * brief     : �[�x�e�N�X�`���̍쐬
		 *             �쐬���ꂽ�e�N�X�`����
		 *		       Render3D()�ł�ShadowMap�̃��\�[�X�Ƃ��Ďg�p
		 * ��caution : "FBXModelComponent", "ObjModelComponent", "GeometricPrimitiveComponent"
		 *			   �����G���e�B�e�B�̂ݕ`�悳���
		 */
		void RenderShadow_Begin(ID3D11DeviceContext* immediate_context) const;
		void RenderShadow(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const component::CameraComponent* view, const Light* light);
		void RenderShadow_End(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : SkyBox�̕`��
		 *		   off_screen�o�b�t�@���g�p���ď�������
		 */
		void RenderSkyBox_Begin(
				ID3D11DeviceContext*				immediate_context,
				const component::CameraComponent*	camera_comp);
		void RenderSkyBox(
				ID3D11DeviceContext*				immediate_context,
				ecs::Registry*						registry,
				const component::CameraComponent*	view,
				const Light*						light);
		void RenderSkyBox_End(
				ID3D11DeviceContext* immediate_context,
				const component::CameraComponent* camera_comp);

		/*
		 * brief     : 3D���f����GBuffer�ւ̕`��
		 */
		/*
		 * brief     : GBuffer�p��RTV�̃N���A�A�Z�b�g�A�V�F�[�_�[�̃Z�b�g
		 * ��caution : �V�F�[�_��Begin�̒i�K�ŃZ�b�g����̂�GBuffer�̕`�撆��
		 *			   �V�F�[�_���Z�b�g���Ȃ��悤�ɂ���
		 */
		void Render3DToGBuffer_Begin(ID3D11DeviceContext* immediate_context) const;
		/*
		 * brief : GBuffer��`���ɂ������f���̕`��
		 */
		void Render3DToGBuffer(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, const component::CameraComponent* view, const Light* light);
		/*
		 * brier     : �e�X�̃V�F�[�_�[�ɉ����č쐬����GBuffer��off_screen�ɂ܂Ƃ߂�
		 * ��caution : Render3DToGBuffer_Begin�֐��ŃZ�b�g����GBuffer�p�̃V�F�[�_�A���o�C���h����
		 */
		void Render3DToGBuffer_End(ID3D11DeviceContext* immediate_context, const component::CameraComponent* camera_comp) const;

		/*
		 * brief : �|�X�g�v���Z�X����
		 *		   (Bloom)
		 */
		void RenderPostProcess_Begin(ID3D11DeviceContext* immediate_context);
		void RenderPostProcess(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		void RenderPostProcess_End(ID3D11DeviceContext* immediate_context);

		/*
		 * brief     : 3D���f���̕`��
		 * ��caution : "FBXModelComponent", "ObjModelComponent", "GeometricPrimitiveComponent"
		 *			   �����G���e�B�e�B�̂ݕ`�悳���
		 */
		void Render3D_Begin(ID3D11DeviceContext* immediate_context);
		void Render3D(
				ID3D11DeviceContext* immediate_context,
				ecs::Registry* registry,
				const component::CameraComponent* view,
				const Light* light);
		void Render3D_End(
				ID3D11DeviceContext* immediate_context,
				const component::CameraComponent* camera_comp);

		void Render2D(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);

		/**
		 * @brief : "ModelComponent"�̎����f���̕`��
		 */
		void RenderModel(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry, mapping::rename_type::Entity entity,
			const component::ModelComponent* model_comp,
			const component::CameraComponent* view, const Light* light);

		/**
		 * @brief :   "SkyBoxComponent"�������f���̕`��
		 */
		void RenderSkyBox(
				ID3D11DeviceContext* immediate_context,
				ecs::Registry* registry, mapping::rename_type::Entity entity,
				const component::CameraComponent* view, const Light* light);

		/*
		 * brief : �����蔻��̕`��
		 */
		void RenderCollision_Begin(
				ID3D11DeviceContext* immediate_context,
				const component::CameraComponent* camera_comp);
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
				const component::CameraComponent* camera_comp);

		/*
		 * brief        : 2D�X�v���C�g�̕`��
		 * ��caution(1) : "SpriteObjectComponent", "SpriteComponent", "AnimSpriteComponent"
		 *                �����G���e�B�e�B�̂ݕ`�悳���
		 * ��caution(2) : "�o�b�N�o�b�t�@�ɒ��ڏ������ނ��߁uBegin�v��uEnd�v�͂Ȃ�"
		 */
		// 2D�X�v���C�g�`��(Animation�Ȃ�)
		void RenderSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, mapping::rename_type::Entity entity);
		// 2D�X�v���C�g�`��(Animation����)
		void RenderAnimSprite(ID3D11DeviceContext* immediate_context, ecs::Registry* registry, mapping::rename_type::Entity entity);

		/*
		 * brief     : GBuffer��off_screen�Ɍ���
		 * ��caution : FrameBuffer�̃Z�b�g�͂��̊֐��ł͍s��Ȃ�
		 */
		void CombinationGBuffer() const;
	};
} //  cumulonimbus::renderer
