#include "light_manager.h"

namespace cumulonimbus::light
{
	LightManager::LightManager(ID3D11Device* device)
	{
		CreateCBuffer(device);
		cb_point_lights.CreateCBuffer(device);
		cb_spot_lights.CreateCBuffer(device);
		cb_light_info.CreateCBuffer(device);
	}

	void LightManager::SetDirectionalLight(const DirectionalLightCB& directional_light)
	{
		cb_directional_light = directional_light;
	}

	void LightManager::SetPointLight(const PointLight& point_light, const u_int index)
	{
		if (index >= MAX_POINT_LIGHT)
			assert(!"Overflow point light cbuffer(LightManager::SetPointLight)");

		cb_point_lights.GetData().point_lights[index] = point_light;
	}

	void LightManager::SetSpotLight(const SpotLight& spot_light, const u_int index)
	{
		if (index >= MAX_POINT_LIGHT)
			assert(!"Overflow spot light cbuffer(LightManager::SetSpotLight)");

		cb_spot_lights.GetData().spot_lights[index] = spot_light;
	}

	 void LightManager::SetLightInfo(const LightInfoCB& light_info)
	{
		 cb_light_info.SetData(light_info);
	}

	void LightManager::SetNumPointLights(const uint num_lights)
	{
		LightInfoCB& light_info  = cb_light_info.GetData();
		light_info.p_light_count = num_lights;
	}

	void LightManager::SetNumSpotLights(const uint num_lights)
	{
		LightInfoCB& light_info  = cb_light_info.GetData();
		light_info.s_light_count = num_lights;
	}

	void LightManager::CreateCBuffer(ID3D11Device* device)
	{
		HRESULT hr = E_FAIL;

		// コンスタントバッファの作成
		{// ディレクショナルライト
			const int size = sizeof(DirectionalLightCB);
			if constexpr (sizeof(DirectionalLightCB) % 16 != 0)
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
		immediate_context->UpdateSubresource(directional_light_buffer.Get(), 0, NULL, &cb_directional_light, 0, 0);
		immediate_context->PSSetConstantBuffers(CBSlot_DirectionalLight, 1, directional_light_buffer.GetAddressOf());
	}

	void LightManager::UnbindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context) const
	{
		ID3D11Buffer* const null_buffer = nullptr;
		immediate_context->PSSetConstantBuffers(CBSlot_DirectionalLight, 1, &null_buffer);
	}

	void LightManager::BindPointLightCBuffer(ID3D11DeviceContext* immediate_context)
	{
		cb_point_lights.Activate(immediate_context, CBSlot_PointLight, false);
	}

	void LightManager::UnbindPointLightCBuffer(ID3D11DeviceContext* immediate_context) const
	{
		cb_point_lights.Deactivate(immediate_context);
	}

	void LightManager::BindSpotLightCBuffer(ID3D11DeviceContext* immediate_context)
	{
		cb_spot_lights.Activate(immediate_context, CBSlot_SpotLight);
	}

	void LightManager::UnbindSpotLightCBuffer(ID3D11DeviceContext* immediate_context) const
	{
		cb_spot_lights.Deactivate(immediate_context);
	}

	void LightManager::BindLightInfoCBuffer(ID3D11DeviceContext* immediate_context)
	{
		cb_light_info.Activate(immediate_context, CBSlot_LightInfo, false);
	}

	void LightManager::UnbindLightInfoCBuffer(ID3D11DeviceContext* immediate_context) const
	{
		cb_light_info.Deactivate(immediate_context);
	}

} // cumulonimbus::light
