#pragma once
#include <memory>
#include <unordered_map>

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
#include "light_manager.h"
#include "local_shader_manager.h"
#include "rasterizer.h"
#include "rename_type_mapping.h"
#include "sampler.h"
#include "shader_manager.h"
#include "texture.h"

enum class CollisionType;

namespace cumulonimbus
{
	namespace system
	{
		class EffekseerManager;
	} // system

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
		class SpriteComponent;
		class BillboardComponent;
	} // component

	namespace graphics::buffer
	{
		class GBuffer;
	} // graphics::buffer

} // cumulonimbus

namespace cumulonimbus::renderer
{
	class RenderPath
	{
	public:
		explicit RenderPath(ID3D11Device* device);

		void RenderScene(
			ID3D11DeviceContext* immediate_context,
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& scenes,
			const camera::Camera& camera);
		void RenderGame(
			ID3D11DeviceContext* immediate_context,
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& scenes);

		/**
		 * @brief : Scene View�̕`��
		 */
		void RenderScene(
			ID3D11DeviceContext* immediate_context, ecs::Registry* registry,
			const camera::Camera* camera, const Light* light);
		/**
		 * @brief : Game View�̕`��
		 */
		void RenderGame(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry, const Light* light);

	private:
		// ���ׂẴV�F�[�_�[�̐����ƃZ�b�g
		std::unique_ptr<shader_system::ShaderManager> shader_manager{ nullptr };

		//-- DirectX States --//
		std::unique_ptr<Blend>			blend			{ nullptr };
		std::unique_ptr<Rasterizer>		rasterizer		{ nullptr };
		std::unique_ptr<DepthStencil>	depth_stencil	{ nullptr };
		std::unique_ptr<Sampler>		sampler			{ nullptr };

		std::shared_ptr<FrameBuffer>							off_screen					{ nullptr };
		std::unique_ptr<graphics::buffer::GBuffer>				g_buffer					{ nullptr };
		std::unique_ptr<DepthMap>								depth_map					{ nullptr };
		std::unique_ptr<light::LightManager>					light_manager				{ nullptr };
		std::unique_ptr<FullscreenQuad>							fullscreen_quad				{ nullptr };
		std::unique_ptr<shader_asset::LocalShaderAssetManager>	local_shader_asset_manager	{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		sky_box_srv					{ nullptr };

		/**
		 * @brief : "MeshObjectComponent"������DirectX state�̃Z�b�g
		 *          (states : rasterizer , sampler , depth_stencil , blend)
		 */
		void BindGraphicsState(
				ID3D11DeviceContext* immediate_context,
				const cumulonimbus::graphics::GraphicsState& graphics_state);

		/**
		 * @brief : "back_buffer"(�����o�ϐ�)�ɏ������܂�Ă�����̂�
		 *		    �o�b�N�o�b�t�@�ɏ�������
		 */
		void Blit(ID3D11DeviceContext* immediate_context) const;

		//-- ���C�g�֘A --//
		/**
		 * @brief : �V�[�����ɂ��郉�C�g���R���X�^���g�o�b�t�@
		 *			�p�����[�^�Ƃ��Ă܂Ƃ߂�
		 * @remark : 1���[�v�Ɉ�x�����ŗǂ�
		 */
		void SetLightParam(std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& scenes);
		void BindCBufferLight(ID3D11DeviceContext* immediate_context);
		void UnbindCBufferLight(ID3D11DeviceContext* immediate_context);

		/**
		 * @brief : �S�̂̕`��O����
		 */
		void RenderBegin(ID3D11DeviceContext* immediate_context, std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& scenes);
		/**
		 * @brief : �S�̂̕`��㏈��
		 */
		void RenderEnd(ID3D11DeviceContext* immediate_context);

		/**
		 * @brief  : �[�x�e�N�X�`���̍쐬
		 *           �쐬���ꂽ�e�N�X�`����
		 *		     Render3D()�ł�ShadowMap�̃��\�[�X�Ƃ��Ďg�p
		 * @remaek : "FBXModelComponent"
		 *			   �����G���e�B�e�B�̂ݕ`�悳���
		 */
		void RenderShadow_Begin(ID3D11DeviceContext* immediate_context) const;
		void RenderShadow(
			ID3D11DeviceContext*  immediate_context,
			ecs::Registry*		  registry,
			const camera::Camera* camera,
			const Light*		  light);
		void RenderShadow_End(ID3D11DeviceContext* immediate_context) const;

		/**
		 * @brief : SkyBox�̕`��
		 *		    off_screen�o�b�t�@���g�p���ď�������
		 */
		void RenderSkyBox_Begin(
				ID3D11DeviceContext*	immediate_context,
				const camera::Camera*	camera);
		void RenderSkyBox(
				ID3D11DeviceContext*  immediate_context,
				const camera::Camera* camera,
				const Light*		 light);
		void RenderSkyBox_End(
				ID3D11DeviceContext* immediate_context,
				const camera::Camera* camera);

		/**
		 * @brief : Effekseer�̕`��
		 */
		void RenderEffekseerBegin(const system::EffekseerManager* effekseer_manager,const camera::Camera* camera);
		void RenderEffekseer(const system::EffekseerManager* effekseer_manager);
		void RenderEffekseerEnd(const system::EffekseerManager* effekseer_manager, const camera::Camera* camera);

		/**
		 * @brief     : 3D���f����GBuffer�ւ̕`��
		 */
		/**
		 * @brief  : GBuffer�p��RTV�̃N���A�A�Z�b�g�A�V�F�[�_�[�̃Z�b�g
		 * @remark : �V�F�[�_��Begin�̒i�K�ŃZ�b�g����̂�GBuffer�̕`�撆��
		 *			   �V�F�[�_���Z�b�g���Ȃ��悤�ɂ���
		 */
		void Render3DToGBuffer_Begin(ID3D11DeviceContext* immediate_context) const;
		/**
		 * @brief : GBuffer��`���ɂ������f���̕`��
		 */
		void Render3DToGBuffer(
			ID3D11DeviceContext*  immediate_context,
			ecs::Registry*		  registry,
			const camera::Camera* camera,
			const Light*		  light,
			bool				  is_game);
		/**
		 * @brier  : �e�X�̃V�F�[�_�[�ɉ����č쐬����GBuffer��off_screen�ɂ܂Ƃ߂�
		 * @remark : Render3DToGBuffer_Begin�֐��ŃZ�b�g����GBuffer�p�̃V�F�[�_�A���o�C���h����
		 */
		void Render3DToGBuffer_End(
			ID3D11DeviceContext* immediate_context,
			const camera::Camera* camera) const;

		/**
		 * @brief : �|�X�g�v���Z�X����
		 *		   (Bloom)
		 */
		void RenderPostProcess_Begin(ID3D11DeviceContext* immediate_context);
		void RenderPostProcess(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		void RenderPostProcess_End(ID3D11DeviceContext* immediate_context);

		/**
		 * @brief  : 3D���f���̕`��
		 * @remark : "FBXModelComponent"
		 *			   �����G���e�B�e�B�̂ݕ`�悳���
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

		void Render2D(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry,
			const camera::Camera* camera,
			bool is_scene);

		/**
		 * @brief : "ModelComponent"�̎����f���̕`��
		 */
		void RenderModel(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry, mapping::rename_type::Entity entity,
			const component::ModelComponent* model_comp,
			const camera::Camera* camera, const Light* light);

		/**
		 * @brief : �S�Ă̓����蔻��̕`��
		 */
		void RenderCollisions(
			ID3D11DeviceContext* immediate_context, ecs::Registry* registry, bool is_game);
		void RenderSphereCollision(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry,
			const mapping::rename_type::Entity& entity,
			bool is_game);
		void RenderCapsuleCollision(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry,
			const mapping::rename_type::Entity& entity,
			bool is_game);

		///**
		// * @brief : �����蔻��̕`��
		// */
		//void RenderCollision_Begin(
		//		ID3D11DeviceContext* immediate_context,
		//		const camera::Camera* camera);
		//void RenderCollision(ID3D11DeviceContext* immediate_context, ecs::Registry* registry);
		//void RenderSphereCollisionModel(
		//		ID3D11DeviceContext* immediate_context,
		//		ecs::Registry* registry, mapping::rename_type::Entity entity,
		//		const FbxModelResource* model_resource);
		//void RenderCapsuleCollisionModel(
		//		ID3D11DeviceContext* immediate_context,
		//		ecs::Registry* registry, mapping::rename_type::Entity entity,
		//		const FbxModelResource* model_resource);
		//void RenderCollision_End(
		//		ID3D11DeviceContext* immediate_context,
		//		const camera::Camera* camera);

		/**
		 * @brief  : 2D�X�v���C�g�̕`��
		 * @remark : "OldSpriteComponent", "OldAnimSpriteComponent"
		 *                �����G���e�B�e�B�̂ݕ`�悳���
		 * @remark : "�o�b�N�o�b�t�@�ɒ��ڏ������ނ��߁uBegin�v��uEnd�v�͂Ȃ�"
		 */

		/**
		 * @brief : 2D�X�v���C�g�`��(Animation�Ȃ�)
		 */
		void RenderSprite(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry,
			mapping::rename_type::Entity entity,
			component::SpriteComponent* sprite_comp);
		/**
		 * @brief : 2D�X�v���C�g�`��(Animation����)
		 */
		void RenderAnimSprite(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry,
			mapping::rename_type::Entity entity);
		/**
		 * @brief : 2D�X�v���C�g�`��(�r���{�[�h)
		 */
		void RenderBillboard(
			ID3D11DeviceContext* immediate_context,
			ecs::Registry* registry,
			mapping::rename_type::Entity entity,
			component::BillboardComponent* billboard_comp,
			const camera::Camera* camera,
			bool is_scene /* �`��悪Scene View���ǂ��� */);

		/**
		 * @brief  : GBuffer��off_screen�Ɍ���
		 * @remark : FrameBuffer�̃Z�b�g�͂��̊֐��ł͍s��Ȃ�
		 */
		void CombinationGBuffer() const;
	};
} //  cumulonimbus::renderer
