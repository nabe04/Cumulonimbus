#pragma once
#include "frame_buffer.h"
#include "shader.h"

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
		void BindShaderAndRTV(ID3D11DepthStencilView* depth_stencil_view);
		void UnbindShaderAndRTV();

		[[nodiscard]] ID3D11ShaderResourceView** GetAlbedoBufferSRV_Address()		const { return albedo_buffer->GetRenderTargetSRV(); }
		[[nodiscard]] ID3D11ShaderResourceView** GetPositionBufferSRB_Address()		const { return position_buffer->GetRenderTargetSRV(); }
		[[nodiscard]] ID3D11ShaderResourceView** GetNormalBufferSRV_Address()		const { return normal_buffer->GetRenderTargetSRV(); }
		
		/*
		 * brief : RenderPath�N���X��GBuffer���m��Blit�����ɉ�����̂�
		 */
		[[nodiscard]] bool GetIsUsedGBuffer() const { return is_used_gbuffer; }
	
	private:
		/*
		 * brief	 : GBuffer�Ƃ��Ďg�p����e�N�X�`���Q
		 * ��caution : FrameBuffer�N���X��Activate�n�͎g��Ȃ�
		 *			   FrameBuffer��Activate�ł�OMSetRenderTarget��
		 *			   FrameBuffer�N���X�݂̂�RenderTargetView�̃Z�b�g������
		 */
		std::unique_ptr<FrameBuffer> albedo_buffer	{ nullptr };
		std::unique_ptr<FrameBuffer> position_buffer{ nullptr };
		std::unique_ptr<FrameBuffer> normal_buffer	{ nullptr };

		static constexpr u_int num_rtv = 3; // GBuffer�Ŏg�p����render_target_view�̑���

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> default_render_target_view{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> default_depth_stencil_view{ nullptr };

		// GBuffer�p�V�F�[�_�[
		std::unique_ptr<shader_system::VertexShader> gbuffer_vs{ nullptr };
		std::unique_ptr<shader_system::PixelShader>  gbuffer_ps{ nullptr };

		// RenderPath�N���X��GBuffer���m��Blit��������
		// ���̕ϐ���false�Ȃ�e�N�X�`�����g�p���Ȃ�
		bool is_used_gbuffer = false;

		/*
		 * brief     : GBuffer�p��render_target_view�̃Z�b�g
		 * ��caution : depth_stencil_view�͑S�Ă�GBuffer�ŋ��ʂ̂��̂��g�p����
		 */
		void BindRTV(ID3D11DepthStencilView* depth_stencil_view);
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
	};
} // cumulonimbus::graphics::buffer