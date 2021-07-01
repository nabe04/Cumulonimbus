#include "gbuffer.h"

#include "locator.h"

namespace cumulonimbus::graphics::buffer
{
	GBuffer::GBuffer()
	{
		ID3D11Device* device = locator::Locator::GetDx11Device()->device.Get();
		const float width    = locator::Locator::GetWindow()->Width();
		const float height   = locator::Locator::GetWindow()->Height();
		albedo_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R8G8B8A8_UNORM	, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		position_buffer      = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		normal_buffer	    = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
	}
} // cumulonimbus::graphics::buffer

