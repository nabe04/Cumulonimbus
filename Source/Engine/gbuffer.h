#pragma once
#include "frame_buffer.h"

namespace cumulonimbus::graphics::buffer
{
	class GBuffer final
	{
	public:
		explicit GBuffer(u_int width, u_int height);

	private:

		/*
		 * brief	 : GBuffer�Ƃ��Ďg�p����e�N�X�`���Q
		 * ��caution : FrameBuffer�N���X��Activate�n�͎g��Ȃ�
		 *			   FrameBuffer��Activate�ł�OMSetRenderTarget��
		 *			   FrameBuffer�N���X�݂̂�RenderTargetView�̃Z�b�g������
		 */
		std::unique_ptr<FrameBuffer> albedo_buffer{nullptr};
		std::unique_ptr<FrameBuffer> position_buffer{nullptr};
		std::unique_ptr<FrameBuffer> normal_buffer{nullptr};
	};
} // cumulonimbus::graphics::buffer