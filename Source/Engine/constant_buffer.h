#pragma once
#include <cassert>

#include <d3d11.h>
#include <wrl.h>
#include <cereal/cereal.hpp>

namespace cumulonimbus::buffer
{
	template <class T>
	class ConstantBuffer
	{
	public:
		explicit ConstantBuffer(ID3D11Device* const device)
		{
			CreateCBuffer(device);
		}

		explicit ConstantBuffer() = default; // for cereal
		virtual ~ConstantBuffer() = default;
		//ConstantBuffer(ConstantBuffer&) = delete;
		//ConstantBuffer(ConstantBuffer&&) = delete;
		//ConstantBuffer(const ConstantBuffer&) = delete;
		//ConstantBuffer(const ConstantBuffer&&) = delete;
		//ConstantBuffer& operator= (ConstantBuffer&&) = delete;
		//ConstantBuffer& operator= (ConstantBuffer&) = delete;
		//ConstantBuffer& operator= (const ConstantBuffer&) = delete;
		//ConstantBuffer& operator= (const ConstantBuffer&&) = delete;

		void Load(ID3D11Device* const device)
		{
			CreateCBuffer(device);
		}

		void CreateCBuffer(ID3D11Device* const device)
		{
			int size = sizeof(T);
			if (sizeof(T) % 16 != 0)
				assert(!"constant buffer's need to be 16 byte aligned");

			D3D11_BUFFER_DESC buffer_desc = {};
			buffer_desc.ByteWidth			= sizeof(T);
			buffer_desc.Usage				= D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags		= 0;
			buffer_desc.MiscFlags			= 0;
			buffer_desc.StructureByteStride = 0;
			const HRESULT hr = device->CreateBuffer(&buffer_desc, 0, buffer_object.GetAddressOf());
			if (FAILED(hr))
				assert(!"CreateBuffer error(Constant buffer)");
		}

		void Activate(ID3D11DeviceContext* const immediate_context, int slot, bool set_in_vs = true, bool set_in_ps = true)
		{
			if (dirty_flg)
			{
				dirty_flg = false;
				immediate_context->UpdateSubresource(buffer_object.Get(), 0, NULL, &data, 0, 0);
			}

			using_slot = slot;

			ID3D11Buffer* null_buffer = nullptr;
			set_in_vs ? immediate_context->VSSetConstantBuffers(using_slot, 1, buffer_object.GetAddressOf()) : immediate_context->VSSetConstantBuffers(slot, 1, &null_buffer);
			set_in_ps ? immediate_context->PSSetConstantBuffers(using_slot, 1, buffer_object.GetAddressOf()) : immediate_context->PSSetConstantBuffers(slot, 1, &null_buffer);
		}

		void Deactivate(ID3D11DeviceContext* const immediate_context) const
		{
			ID3D11Buffer* const null_buffer = nullptr;
			immediate_context->VSSetConstantBuffers(using_slot, 1, &null_buffer);
			immediate_context->PSSetConstantBuffers(using_slot, 1, &null_buffer);
		}

		void SetData(const T& data)
		{
			dirty_flg = true;
			this->data = data;
		}

		T& GetData()
		{
			dirty_flg = true;
			return data;
		}
		const T& GetData() const { return data; }

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(data),
				CEREAL_NVP(using_slot));
		}
	private:
		T data;
		unsigned int using_slot{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_object{};
		bool dirty_flg{ true };
	};

} // buffer
