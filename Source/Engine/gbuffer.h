#pragma once
#include <d3d11.h>
#include <wrl.h>

class FrameBuffer;

namespace cumulonimbus::shader_system
{
	class VertexShader;
	class PixelShader;
} // cumulonimbus::shader_system

namespace cumulonimbus::graphics::buffer
{
	class GBuffer final
	{
	public:
		explicit GBuffer();

		/*
		 * brief : render_target_view(frame_buffer)��depth_stencil_view�̉�ʃN���A����
		 */
		void Clear(float r = 0, float g = 0, float b = 0, float a = 1);

		/*
		 * brief : GBuffer�p��RTV�ƃV�F�[�_�[�̃o�C���h
		 */
		void BindShaderAndRTV();
		void UnbindShaderAndRTV();

		/*
		 * brief     : GBuffer�p��render_target_view�̃Z�b�g
		 * ��caution : depth_stencil_view�͑S�Ă�GBuffer�ŋ��ʂ̂��̂��g�p����
		 */
		void BindRTV();
		/*
		 * brief     : �Z�b�g����Ă���render_target_view��depth_stencil_view��
		 *			   "default" render_target_view�ɃZ�b�g
		 */
		void UnbindRTV();

		/*
		 * brief : GBuffer�p�V�F�[�_�[�̃Z�b�g
		 */
		void BindShader() const;
		/*
		 * brief : GBuffer�p�V�F�[�_�[�̃��Z�b�g
		 */
		void UnbindShader() const;

		/*
		 * brief	 : GBuffer�̃��C�e�B���O�p�V�F�[�_�[�̃Z�b�g
		 * ��caution : �s�N�Z���V�F�[�_�[�݂̂̃Z�b�g
		 */
		void BindGBuffLightingShader() const;

		/*
		 * brief : �S�Ă�GBuffer�e�N�X�`���̃o�C���h
		 * �o�C���h�����e�N�X�`���̃X���b�g�ԍ�
		 * slot TexSlot_BaseColorMap(20) : AlbedoBuffer�e�N�X�`��
		 * slot TexSlot_NormalMap(21)	 : NormalBuffer�e�N�X�`��
		 * slot TexSlot_Position(30)	 : PositionBuffer�e�N�X�`��
		 * slot TexSlot_ShaderSlot(31)	 : ShaderSlotBuffer�e�N�X�`��
		 */
		void BindGBufferTextures() const;

		/*
		 * brief : �S�Ă�GBuffer�e�N�X�`���̃A���o�C���h
		 * �o�C���h�����e�N�X�`���̃X���b�g�ԍ�
		 * slot TexSlot_BaseColorMap(20) : AlbedoBuffer�e�N�X�`��
		 * slot TexSlot_NormalMap(21)	 : NormalBuffer�e�N�X�`��
		 * slot TexSlot_Position(30)	 : PositionBuffer�e�N�X�`��
		 * slot TexSlot_ShaderSlot(31)	 : ShaderSlotBuffer�e�N�X�`��
		 */
		void UnbindGBufferTextures() const;


		/*
		 * brief : GBuffer�e�N�X�`���̌ʂ�SRV�̃o�C���h
		 */
		/*
		 * brief	: albedo_buffer�e�N�X�`���̃o�C���h
		 * slot�ԍ� : TexSlot_BaseColorMap(20)
		 */
		void BindColorTexture() const;
		/*
		 * brief	: normal_buffer�e�N�X�`���̃o�C���h
		 * slot�ԍ� : TexSlot_NormalMap(21)
		 */
		void BindNormalMapTexture() const;
		/*
		 * brief	: position_buffer�e�N�X�`���̃o�C���h
		 * slot�ԍ� : TexSlot_Position(30)
		 */
		void BindPositionTexture() const;

		/*
		 * brief : GBuffer�e�N�X�`���̌ʂ�SRV�̃A���o�C���h
		*/
		/*
		 * brief	 : albedo_buffer�e�N�X�`���̃A���o�C���h
		* slot�ԍ� : TexSlot_BaseColorMap(20)
		 */
		void UnbindColorTexture() const;
		/*
		 * brief	: normal_buffer�e�N�X�`���̃A���o�C���h
		 * slot�ԍ� : TexSlot_NormalMap(21)
		 */
		void UnbindNormalMapTexture() const;
		/*
		 * brief	: position_buffer�e�N�X�`���̃A���o�C���h
		 * slot�ԍ� : TexSlot_Position(30)
		 */
		void UnbindPositionTexture() const;


		/*
		 * brief : GBuffer�e�N�X�`����SRV�̃Q�b�g
		 */
		[[nodiscard]] ID3D11ShaderResourceView** GetColorBufferSRV_Address()		const;
		[[nodiscard]] ID3D11ShaderResourceView** GetPositionBufferSRV_Address()		const;
		[[nodiscard]] ID3D11ShaderResourceView** GetNormalBufferSRV_Address()		const;

	private:
		/*
		 * brief	 : GBuffer�Ƃ��Ďg�p����e�N�X�`���Q
		 * ��caution : FrameBuffer�N���X��Activate�n�͎g��Ȃ�
		 *			   FrameBuffer��Activate�ł�OMSetRenderTarget��
		 *			   FrameBuffer�N���X�݂̂�RenderTargetView�̃Z�b�g������
		 */
		// �e�N�X�`���{���̐F���o�b�t�@
		std::unique_ptr<FrameBuffer> color_buffer	{ nullptr };
		// ���[���h���W�o�b�t�@
		std::unique_ptr<FrameBuffer> position_buffer{ nullptr };
		// �@�����o�b�t�@
		std::unique_ptr<FrameBuffer> normal_buffer	{ nullptr };


		static constexpr u_int num_rtv = 3; // GBuffer�Ŏg�p����render_target_view�̑���

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> default_render_target_view{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> default_depth_stencil_view{ nullptr };

		// GBuffer�p��depth_stencil_view,shader_resource_view
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		dsv_for_g_buffer{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	srv_for_g_buffer{ nullptr };

		// GBuffer�p�V�F�[�_�[
		std::unique_ptr<shader_system::VertexShader> g_buffer_vs{ nullptr };
		std::unique_ptr<shader_system::PixelShader>  g_buffer_ps{ nullptr };
		// GBuffer�̃��C�e�B���O�p�V�F�[�_�[
		std::unique_ptr<shader_system::PixelShader> g_buff_lighting_ps{ nullptr };
	};
} // cumulonimbus::graphics::buffer