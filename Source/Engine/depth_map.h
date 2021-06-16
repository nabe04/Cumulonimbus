#pragma once
#include <memory>

#include <d3d11.h>

#include "frame_buffer.h"

namespace shader
{
	class VertexShader;
	class PixelShader;
}
class GaussianBlur;

namespace cumulonimbus::renderer
{
	/*
	 * brief : �`��Ώۂ̐[�x�l�𒊏o
	 *         ���U�V���h�E�}�b�v�p�ɂ��ׂĂ̒��o���I����
	 *         �u���[(GaussianBlur)��������
	 */
	class DepthMap final
	{
	public:
		/*
		 * width  : �[�x�}�b�v�e�N�X�`���̕�
		 * height : �[�x�}�b�v�e�N�X�`���̍���
		 */
		explicit DepthMap(ID3D11Device* device, u_int width = default_depth_map_width, u_int height = default_depth_map_height);

		/*
		 * brief : �t���[���o�b�t�@�̃N���A����
		 *         FB,VS,PS�̃Z�b�g
		 */
		void Begin(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : default_fb,vs,ps�ɃZ�b�g���Ȃ���
		 *	       ���o�����[�x�e�N�X�`���̃u���[�K�p
		 */
		void End(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : FB,VS,PS�̎��s
		 *		   ���ݕێ����Ă���FB,VS,PS��default_fb,vs,ps
		 *		   �ɕێ�����(Deactivate���Ɏg�p)
		 */
		void Activate(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : default_fb,vs,ps�ɂ��ꂼ��Z�b�g���Ȃ���
		 */
		void Deactivate(ID3D11DeviceContext* immediate_context) const;

		// �u���[�K�p��e�N�X�`����SRV
		[[nodiscard]] ID3D11ShaderResourceView** GetDepthExtractionSRV() const { return depth_extraction_fb.get()->render_target_shader_resource_view.GetAddressOf(); }

	private:
		static const u_int default_depth_map_width  = 2048;
		static const u_int default_depth_map_height = 2048;
		
		// �[�x�ۑ��p�o�b�t�@
		std::unique_ptr<FrameBuffer> depth_extraction_fb{};

		// �u���[�p(End()�֐����Ƀu���[��������)
		std::unique_ptr<GaussianBlur> gaussian_blur{};

		// vertex & pixel shader
		std::unique_ptr<shader::VertexShader> depth_extraction_vs{};
		std::unique_ptr<shader::PixelShader>  depth_extraction_ps{};
	};
}