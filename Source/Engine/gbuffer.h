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
		void Clear(float r = 0, float g = 0, float b = 0, float a = 1) const;

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
	};
} // cumulonimbus::graphics::buffer