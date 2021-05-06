#pragma once

#include <array>
#include <bitset>
#include <memory>

#include <d3d11.h>

#include "frame_buffer.h"

enum class RenderingBuffer
{
	BackBuffer,
	Bloom,
	SorbelFilter,

	End,
};

struct RenderingBufferBitset
{
	std::bitset<static_cast<int>(RenderingBuffer::End)> rendering_buffer_bitset;

	RenderingBufferBitset()
	{
		for (int i = 0; i < rendering_buffer_bitset.size(); ++i)
		{
			rendering_buffer_bitset.reset(i);
		}

		rendering_buffer_bitset.set(0);
	}

	void Set(const RenderingBuffer& index) { rendering_buffer_bitset.set(static_cast<int>(index)); }
	void Reset(const RenderingBuffer& index) { rendering_buffer_bitset.reset(static_cast<int>(index)); }
};

class FrameBufferManager
{
private:
	std::array<std::unique_ptr<FrameBuffer>, static_cast<int>(RenderingBuffer::End)> frame_buffers;

public:
	FrameBufferManager(ID3D11Device* device);

	void Clear(ID3D11DeviceContext* immediate_context);
	void Clear(ID3D11DeviceContext* immediate_context, RenderingBuffer index);

	FrameBuffer* UsingBuffer(int index) { return frame_buffers.at(index).get(); }
};