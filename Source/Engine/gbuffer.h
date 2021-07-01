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
		 * brief	 : GBufferとして使用するテクスチャ群
		 * ※caution : FrameBufferクラスのActivate系は使わない
		 *			   FrameBufferのActivateでのOMSetRenderTargetは
		 *			   FrameBufferクラスのみのRenderTargetViewのセットだから
		 */
		std::unique_ptr<FrameBuffer> albedo_buffer{nullptr};
		std::unique_ptr<FrameBuffer> position_buffer{nullptr};
		std::unique_ptr<FrameBuffer> normal_buffer{nullptr};
	};
} // cumulonimbus::graphics::buffer