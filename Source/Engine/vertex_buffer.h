#pragma once
#include <assert.h>
#include <vector>

#include <d3d11.h>
#include <wrl.h>

namespace buffer
{
	template<class T>
	class VertexBuffer final
	{
	public:

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		std::vector<T> vertex_datas;

	public:
		VertexBuffer(ID3D11Device* device, const std::vector<T>& datas)
		{
			vertex_datas = datas;

			CreateVertexBuffer(device);
		}

		[[nodiscard]] ID3D11Buffer*  GetVertexBuffer()		{ return vertex_buffer.Get(); }
		[[nodiscard]] ID3D11Buffer** GetVertexBufferAddress() { return vertex_buffer.GetAddressOf(); }
		[[nodiscard]] const std::vector<T>& GetVertexDatas()  { return vertex_datas; }

		 void SetVertexDatas(const std::vector<T>& datas) { vertex_datas = datas; }
	private:
		void CreateVertexBuffer(ID3D11Device* device)
		{
			D3D11_BUFFER_DESC vertex_buffer_desc{};
			vertex_buffer_desc.Usage				= D3D11_USAGE_DEFAULT;
			vertex_buffer_desc.ByteWidth			= sizeof(T) * static_cast<u_int>(vertex_datas.size());
			vertex_buffer_desc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
			vertex_buffer_desc.CPUAccessFlags		= 0;
			vertex_buffer_desc.MiscFlags			= 0;
			vertex_buffer_desc.StructureByteStride	= 0;

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem			= vertex_datas.data();
			init_data.SysMemPitch		= 0;
			init_data.SysMemSlicePitch	= 0;

			if (FAILED(device->CreateBuffer(&vertex_buffer_desc, &init_data, vertex_buffer.GetAddressOf())))
				assert(!"Create vertex buffer error(class : VertexBuffer)");
		}
	};
}