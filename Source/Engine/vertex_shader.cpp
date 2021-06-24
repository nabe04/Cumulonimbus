#include "vertex_shader.h"

#include <memory>
#include <cassert>
#include <map>
#include <string>

#include "locator.h"

namespace cumulonimbus::shader_system
{
	HRESULT CreateVertexShaderFromCSO(ID3D11Device* device,
									  const char* cso_name,
									  ID3D11VertexShader** vertex_shader,
									  ID3D11InputLayout** input_layout,
									  D3D11_INPUT_ELEMENT_DESC* input_element_desc,
									  UINT num_elements)
	{
		struct MapContent
		{
			MapContent(ID3D11VertexShader* vertex_shader, ID3D11InputLayout* input_layout) :vertex_shader(vertex_shader), input_layout(input_layout) {}
			Microsoft::WRL::ComPtr<ID3D11VertexShader>vertex_shader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>input_layout;
		};

		static std::map<std::string, MapContent>checkShader;
		auto it = checkShader.find(cso_name);
		if (it != checkShader.end())
		{

			*vertex_shader = it->second.vertex_shader.Get();
			(*vertex_shader)->AddRef();
			*input_layout = it->second.input_layout.Get();
			(*input_layout)->AddRef();

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

		HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
		if (FAILED(hr))
			assert(!"CreateVertexShader error");

		hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
		if (FAILED(hr))
			assert(!"CreateInputLayout error");

		checkShader.insert(std::make_pair(cso_name, MapContent(*vertex_shader, *input_layout)));

		return hr;

	}

	VertexShader::VertexShader(const char* cso, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
	{
		if (input_element_desc)
		{
			HRESULT hr = CreateVertexShaderFromCSO(locator::Locator::GetDx11Device()->device.Get(), cso, shader_object.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, num_elements);
			if (FAILED(hr))
				assert(!"Create vertex shader error!");

			return;
		}

		// Setting the default InputLayout
		std::array<D3D11_INPUT_ELEMENT_DESC, 5> layout =
		{
			{
			{ "POSITION",	0 , DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0 , DXGI_FORMAT_R32G32B32_FLOAT ,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0 , DXGI_FORMAT_R32G32_FLOAT ,		0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",	0 ,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES",		0 ,	DXGI_FORMAT_R32G32B32A32_UINT,  0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			}
		};

		HRESULT hr = CreateVertexShaderFromCSO(locator::Locator::GetDx11Device()->device.Get(), cso, shader_object.GetAddressOf(), input_layout.GetAddressOf(), layout.data(), static_cast<u_int>(layout.size()));
		if (FAILED(hr))
			assert(!"Create vertex shader error!");

		return;
	}

	void VertexShader::BindVS()
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();

		immediate_context->IAGetInputLayout(default_input_layout.ReleaseAndGetAddressOf());
		immediate_context->VSGetShader(default_shader_object.ReleaseAndGetAddressOf(), 0, 0);

		immediate_context->IASetInputLayout(input_layout.Get());
		immediate_context->VSSetShader(shader_object.Get(), 0, 0);
	}

	void VertexShader::UnbindVS()
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();

		immediate_context->IASetInputLayout(default_input_layout.Get());
		immediate_context->VSSetShader(default_shader_object.Get(), 0, 0);
	}
}
