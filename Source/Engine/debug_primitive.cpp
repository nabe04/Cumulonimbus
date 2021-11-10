#include "debug_primitive.h"

#include "locator.h"

namespace cumulonimbus::graphics
{
	DebugPrimitiveBase::DebugPrimitiveBase(ID3D11Device* device)
	{
		cbuffer = std::make_unique<buffer::ConstantBuffer<DebugPrimitiveCB>>(device);
	}

	PrimitiveSphere::PrimitiveSphere(ID3D11Device* device)
		:DebugPrimitiveBase{device}
	{

	}

	void PrimitiveSphere::Render(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		UINT stride = sizeof(primitive::Vertex);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	}

	void PrimitiveSphere::BindCBuffer()
	{

	}

	void PrimitiveSphere::UnBindCBuffer()
	{

	}

	void PrimitiveSphere::CreateVertexBuffer(ID3D11Device* device)
	{
		HRESULT hr = E_FAIL;
		const int segment_count = 36;

		{// Vertex Buffer�̍쐬
			std::vector<primitive::Vertex> vertices;
			primitive::Vertex vertex;
			// XY
			for (int i = 0; i <= segment_count; ++i)
			{
				const float angle0 = static_cast<float>(i) / static_cast<float>(segment_count) * DirectX::XM_2PI;
				vertex.position = DirectX::SimpleMath::Vector4(sinf(angle0), cosf(angle0), 0, 1);
				vertices.push_back(vertex);
			}
			// XZ
			for (int i = 0; i <= segment_count; ++i)
			{
				const float angle0 = static_cast<float>(i) / static_cast<float>(segment_count) * DirectX::XM_2PI;
				vertex.position = DirectX::SimpleMath::Vector4(sinf(angle0), 0, cosf(angle0), 1);
				vertices.push_back(vertex);
			}
			// YZ
			for (int i = 0; i <= segment_count; ++i)
			{
				const float angle0 = static_cast<float>(i) / static_cast<float>(segment_count) * DirectX::XM_2PI;
				vertex.position = DirectX::SimpleMath::Vector4(0, sinf(angle0), cosf(angle0), 1);
				vertices.push_back(vertex);
			}

			D3D11_BUFFER_DESC vertex_buffer_desc{};
			vertex_buffer_desc.Usage				= D3D11_USAGE_DEFAULT;
			vertex_buffer_desc.ByteWidth			= sizeof(primitive::Vertex) * static_cast<u_int>(vertices.size());
			vertex_buffer_desc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
			vertex_buffer_desc.CPUAccessFlags		= 0;
			vertex_buffer_desc.MiscFlags			= 0;
			vertex_buffer_desc.StructureByteStride	= 0;

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem			= vertices.data();
			init_data.SysMemPitch		= 0;
			init_data.SysMemSlicePitch	= 0;

			hr = device->CreateBuffer(&vertex_buffer_desc, &init_data, vertex_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Create Vertex buffer error(sky box)");
		}

		{// Index Buffer�̍쐬
			std::vector<u_int> indices;
			for (int i = 0; i < segment_count; ++i)
			{
				indices.push_back(static_cast<u_int>(i));
				indices.push_back(static_cast<u_int>(i + 1));
			}
			for (int i = 0; i < segment_count; ++i)
			{
				indices.push_back(static_cast<u_int>(i + segment_count + 1));
				indices.push_back(static_cast<u_int>(i + segment_count + 1 + 1));
			}
			for (int i = 0; i < segment_count; ++i)
			{
				indices.push_back(static_cast<u_int>(i + (segment_count + 1) * 2));
				indices.push_back(static_cast<u_int>(i + (segment_count + 1) * 2 + 1));
			}
			D3D11_BUFFER_DESC index_buffer_desc{};
			index_buffer_desc.Usage					= D3D11_USAGE_DEFAULT;
			index_buffer_desc.ByteWidth				= indices.size() * sizeof(u_int);
			index_buffer_desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
			index_buffer_desc.CPUAccessFlags		= 0;
			index_buffer_desc.MiscFlags				= 0;
			index_buffer_desc.StructureByteStride	= 0;

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem			= indices.data();
			init_data.SysMemPitch		= 0;
			init_data.SysMemSlicePitch	= 0;
			hr = device->CreateBuffer(&index_buffer_desc, &init_data, index_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Create Index buffer error(sky box)");
		}
	}

} // cumulonimbus::graphics
