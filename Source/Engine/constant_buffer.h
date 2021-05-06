#pragma once

#include <assert.h>

#include <d3d11.h>
#include <wrl.h>

namespace buffer
{
	template <class T>
	class ConstantBuffer
	{
	private:
		unsigned int using_slot;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_object;

	public:
		T data;

		ConstantBuffer(ID3D11Device* device)
		{
			int size = sizeof(T);
			if (sizeof(T) % 16 != 0)
				assert(!"constant buffer's need to be 16 byte aligned");

			D3D11_BUFFER_DESC buffer_desc = {};
			buffer_desc.ByteWidth = sizeof(T);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			HRESULT hr = device->CreateBuffer(&buffer_desc, 0, buffer_object.GetAddressOf());
			if (FAILED(hr))
				assert(!"CreateBuffer error(Constant buffer)");
		}

		virtual ~ConstantBuffer() = default;
		ConstantBuffer(ConstantBuffer&) = delete;
		ConstantBuffer& operator =(ConstantBuffer&) = delete;

		void Activate(ID3D11DeviceContext* immediate_context, int slot, bool set_in_vs = true, bool set_in_ps = true)
		{
			immediate_context->UpdateSubresource(buffer_object.Get(), 0, NULL, &data, 0, 0);

			using_slot = slot;

			ID3D11Buffer* null_buffer = 0;
			set_in_vs ? immediate_context->VSSetConstantBuffers(using_slot, 1, buffer_object.GetAddressOf()) : immediate_context->VSSetConstantBuffers(slot, 1, &null_buffer);
			set_in_ps ? immediate_context->PSSetConstantBuffers(using_slot, 1, buffer_object.GetAddressOf()) : immediate_context->PSSetConstantBuffers(slot, 1, &null_buffer);
		}

		void Deactivate(ID3D11DeviceContext* immediate_context)
		{
			ID3D11Buffer* null_buffer = 0;
			immediate_context->VSSetConstantBuffers(using_slot, 1, &null_buffer);
			immediate_context->PSSetConstantBuffers(using_slot, 1, &null_buffer);
		}

		void SetData(const T& data) { this->data = data; }

		void Edit()
		{

		}
	};

}
struct ConstantBuffer_Origine
{
	virtual void Edit() {};	// For Imgui
};