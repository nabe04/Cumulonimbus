#include "rendering_buffer.h"

#include "framework.h"
#include "locator.h"

FrameBufferManager::FrameBufferManager(ID3D11Device* device)
{
	frame_buffers.at(static_cast<int>(RenderingBuffer::BackBuffer))   = std::make_unique<FrameBuffer>(device, Locator::GetWindow()->Width(), Locator::GetWindow()->Height(), false/*enable_msaa*/, 8, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	frame_buffers.at(static_cast<int>(RenderingBuffer::Bloom))		  = std::make_unique<FrameBuffer>(device, Locator::GetWindow()->Width(), Locator::GetWindow()->Height(), false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	frame_buffers.at(static_cast<int>(RenderingBuffer::SorbelFilter)) = std::make_unique<FrameBuffer>(device, Locator::GetWindow()->Width(), Locator::GetWindow()->Height(), false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
}

void FrameBufferManager::Clear(ID3D11DeviceContext* immediate_context)
{
	for (auto& it : frame_buffers)
	{
		it->Clear(immediate_context);
	}
}

void FrameBufferManager::Clear(ID3D11DeviceContext* immediate_context, RenderingBuffer index)
{
	frame_buffers.at(static_cast<int>(index))->Clear(immediate_context);
}