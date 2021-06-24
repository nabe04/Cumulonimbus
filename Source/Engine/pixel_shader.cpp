#include "pixel_shader.h"

#include <memory>
#include <cassert>
#include <map>
#include <string>

#include "locator.h"

namespace cumulonimbus::shader_system
{
	HRESULT CreatePixelShaderFromCSO(ID3D11Device* device, const char* cso_name,
								     ID3D11PixelShader** pixel_shader)
	{
		static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> checkShader;
		auto it = checkShader.find(cso_name);
		if (it != checkShader.end())
		{
			*pixel_shader = it->second.Get();
			(*pixel_shader)->AddRef();
			return S_OK;
		}

		FILE* fp = nullptr;
		fopen_s(&fp, cso_name, "rb");
		if (fp == NULL)
			assert(!"file open error");

		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
		if (FAILED(hr))
			assert(!"CretePixelShader error");

		checkShader.insert(std::make_pair(cso_name, *pixel_shader));

		return hr;
	}

	PixelShader::PixelShader(const char* cso)
	{
		HRESULT hr = CreatePixelShaderFromCSO(locator::Locator::GetDx11Device()->device.Get(), cso, shader_object.GetAddressOf());
		if (FAILED(hr))
			assert(!"Create pixel shader error!");
	}

	void PixelShader::BindPS()
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();

		immediate_context->PSGetShader(default_shader_object.ReleaseAndGetAddressOf(), 0, 0);
		immediate_context->PSSetShader(shader_object.Get(), 0, 0);
	}

	void PixelShader::UnbindPS() const
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();

		immediate_context->PSSetShader(default_shader_object.Get(), 0, 0);
	}
}
