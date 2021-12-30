#include "light_manager.h"

namespace cumulonimbus::light
{
	LightManager::LightManager(ID3D11Device* device)
	{
		CreateCBuffer(device);
	}

	void LightManager::SetDirectionalLight(const DirectionalLightCB& directional_light)
	{
		cb_directional_light = directional_light;
	}

	void LightManager::CreateCBuffer(ID3D11Device* device)
	{
		HRESULT hr = E_FAIL;

		// コンスタントバッファの作成
		{// ディレクショナルライト
			const int size = sizeof(DirectionalLightCB);
			if (sizeof(DirectionalLightCB) % 16 != 0)
				assert(!"constant buffer's need to be 16 byte aligned");

			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth			= sizeof(DirectionalLightCB);
			buffer_desc.Usage				= D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags		= 0;
			buffer_desc.MiscFlags			= 0;
			buffer_desc.StructureByteStride = 0;
			hr = device->CreateBuffer(&buffer_desc, 0, directional_light_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"CreateBuffer error(Constant buffer)");
		}
	}

	void LightManager::BindCBuffers(ID3D11DeviceContext* immediate_context)
	{
		BindDirectionalLightCBuffer(immediate_context);
	}

	void LightManager::UnbindCBuffers(ID3D11DeviceContext* immediate_context)
	{
		UnbindDirectionalLightCBuffer(immediate_context);
	}

	void LightManager::BindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context)
	{
		//DirectionalLightCB cb{};
		//cb.d_light_intensity = 1.0f;
		//cb.d_light_direction = DirectX::SimpleMath::Vector3{ 0,0,1 };
		//cb_directional_light = cb;

		immediate_context->UpdateSubresource(directional_light_buffer.Get(), 0, NULL, &cb_directional_light, 0, 0);
		immediate_context->PSSetConstantBuffers(CBSlot_DirectionalLight, 1, directional_light_buffer.GetAddressOf());
	}

	void LightManager::UnbindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context)
	{
		ID3D11Buffer* const null_buffer = nullptr;
		immediate_context->PSSetConstantBuffers(CBSlot_DirectionalLight, 1, &null_buffer);
	}
} // cumulonimbus::light
