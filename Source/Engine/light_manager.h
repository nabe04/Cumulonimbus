#pragma once
#include <array>

#include "cbuffer_lights.h"
#include "constant_buffer.h"

namespace cumulonimbus::light
{
	class LightManager final
	{
	public:
		LightManager(ID3D11Device* device);

		void SetDirectionalLight(const DirectionalLightCB& directional_light);

		void BindCBuffers(ID3D11DeviceContext* immediate_context);
		void UnbindCBuffers(ID3D11DeviceContext* immediate_context);

		void BindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context);
		void UnbindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context);
	private:
		DirectionalLightCB cb_directional_light{};

		Microsoft::WRL::ComPtr<ID3D11Buffer> directional_light_buffer{};

		void CreateCBuffer(ID3D11Device* device);
	};
} // cumulonimbus::light
