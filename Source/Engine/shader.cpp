#include <d3dcompiler.h>

#include <memory>
#include <cassert>
#include <wrl.h>
#include <map>
#include <array>


#include "shader.h"

#include "scene.h"
#include "component_list.h"

namespace shader
{

	HRESULT CreateVertexShaderFromCSO(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
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

	HRESULT CreatePixelShaderFromCSO(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
	{
		static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> checkShader;
		auto it = checkShader.find(cso_name);
		if (it != checkShader.end())
		{
			// ���݂��Ă����
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

	//*********************************************
	//
	// CB_Light
	//
	//*********************************************
	void CB_Light::Update(Scene* scene)
	{
		light_dir = scene->GetLight()->GetLightDir();
		eye_pos   = XMFLOAT4{ scene->GetView()->GetEyePosition().x, scene->GetView()->GetEyePosition().y, scene->GetView()->GetEyePosition().z, 1.0f };

		auto* light = scene->GetLight();
		//DirectX::XMMATRIX view_mat = DirectX::XMMatrixLookAtLH(light->)
	}

	//************************************************************
	//
	//	VertexShader class
	//
	//************************************************************
	VertexShader::VertexShader(ID3D11Device* device, const char* cso, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
	{
		if (input_element_desc)
		{
			HRESULT hr = CreateVertexShaderFromCSO(device, cso, shader_object.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, num_elements);
			if (FAILED(hr))
				assert(!"Create vertex shader error!");

			return;
		}

		// Setting the default InputLayout
		std::array<D3D11_INPUT_ELEMENT_DESC,5> layout =
		{
			{
			{ "POSITION",	0 , DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0 , DXGI_FORMAT_R32G32B32_FLOAT ,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0 , DXGI_FORMAT_R32G32_FLOAT ,		0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",	0 ,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES",		0 ,	DXGI_FORMAT_R32G32B32A32_UINT,  0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			}
		};

		HRESULT hr = CreateVertexShaderFromCSO(device, cso, shader_object.GetAddressOf(), input_layout.GetAddressOf(), layout.data(), static_cast<u_int>(layout.size()));
		if (FAILED(hr))
			assert(!"Create vertex shader error!");

		return;
	}

	void VertexShader::Activate(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->IAGetInputLayout(default_input_layout.ReleaseAndGetAddressOf());
		immediate_context->VSGetShader(default_shader_object.ReleaseAndGetAddressOf(), 0, 0);

		immediate_context->IASetInputLayout(input_layout.Get());
		immediate_context->VSSetShader(shader_object.Get(), 0, 0);
	}

	void VertexShader::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->IASetInputLayout(default_input_layout.Get());
		immediate_context->VSSetShader(default_shader_object.Get(),0,0);
	}

	//****************************************************************
	//
	//	PixelShader class
	//
	//****************************************************************
	PixelShader::PixelShader(ID3D11Device* device, const char* cso)
	{
		HRESULT hr = CreatePixelShaderFromCSO(device, cso, shader_object.GetAddressOf());
		if (FAILED(hr))
			assert(!"Create pixel shader error!");
	}

	void PixelShader::Activate(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->PSGetShader(default_shader_object.ReleaseAndGetAddressOf(), 0, 0);
		immediate_context->PSSetShader(shader_object.Get(), 0, 0);
	}

	void PixelShader::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->PSSetShader(default_shader_object.Get(), 0, 0);
	}
}
