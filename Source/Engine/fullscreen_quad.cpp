#include "fullscreen_quad.h"

#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler")
#endif

#include <cassert>
#include <string>

#include "locator.h"
#include "shader.h"
#include "shader_filename_mapping.h"

FullscreenQuad::FullscreenQuad(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	const std::string fullscreen_quad_vs =
		"struct VS_OUT\n"
		"{\n"
		"	float4 position : SV_POSITION;\n"
		"	float2 texcoord : TEXCOORD;\n"
		"};\n"
		"VS_OUT VS(in uint vertex_id : SV_VERTEXID)\n"
		"{\n"
		"	VS_OUT vout;\n"
		"	vout.texcoord = float2((vertex_id << 1) & 2, vertex_id & 2);\n"
		"	vout.position = float4(vout.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);\n"
		"	return vout;\n"
		"}\n";
	{

		DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> compiled_shader_blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error_message_blob;
		hr = D3DCompile(fullscreen_quad_vs.c_str(), fullscreen_quad_vs.length(), 0, 0, 0, "VS", "vs_5_0", flags, 0, compiled_shader_blob.GetAddressOf(), error_message_blob.GetAddressOf());
		assert(SUCCEEDED(hr));
		hr = device->CreateVertexShader(compiled_shader_blob->GetBufferPointer(), compiled_shader_blob->GetBufferSize(), 0, embedded_vertex_shader.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	const std::string fullscreen_quad_ps =
		"Texture2D texture_map : register( t20 );\n"
		"SamplerState texture_map_sampler_state : register( s0 );\n"
		"struct VS_OUT\n"
		"{\n"
		"	float4 position : SV_POSITION;\n"
		"	float2 texcoord : TEXCOORD;\n"
		"};\n"
		"float4 PS(VS_OUT pin) : SV_TARGET\n"
		"{\n"
		"	return texture_map.Sample(texture_map_sampler_state, pin.texcoord);\n"
		"}\n";
	{
		HRESULT hr = S_OK;
		DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> compiled_shader_blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error_message_blob;
		hr = D3DCompile(fullscreen_quad_ps.c_str(), fullscreen_quad_ps.length(), 0, 0, 0, "PS", "ps_5_0", flags, 0, compiled_shader_blob.GetAddressOf(), error_message_blob.GetAddressOf());
		assert(SUCCEEDED(hr));
		hr = device->CreatePixelShader(compiled_shader_blob->GetBufferPointer(), compiled_shader_blob->GetBufferSize(), 0, embedded_pixel_shaders[0].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	const std::string fullscreen_quad_ps_ms =
		"Texture2DMS<float4> texture_map : register(t20);\n"
		"struct VS_OUT\n"
		"{\n"
		"	float4 position : SV_POSITION;\n"
		"	float2 texcoord : TEXCOORD;\n"
		"};\n"
		"float4 PS(VS_OUT pin, uint sample_index : SV_SAMPLEINDEX) : SV_TARGET\n"
		"{\n"
		"	uint width, height, number_of_samples;\n"
		"	texture_map.GetDimensions(width, height, number_of_samples);\n"
		"	return texture_map.Load(uint2(pin.texcoord.x * width, pin.texcoord.y * height), sample_index);\n"
		"}\n";
	{
		HRESULT hr = S_OK;
		DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> compiled_shader_blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error_message_blob;
		hr = D3DCompile(fullscreen_quad_ps_ms.c_str(), fullscreen_quad_ps_ms.length(), 0, 0, 0, "PS", "ps_5_0", flags, 0, compiled_shader_blob.GetAddressOf(), error_message_blob.GetAddressOf());
		assert(SUCCEEDED(hr));
		hr = device->CreatePixelShader(compiled_shader_blob->GetBufferPointer(), compiled_shader_blob->GetBufferSize(), 0, embedded_pixel_shaders[1].ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = FALSE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, embedded_rasterizer_state.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateRasterizerState error");

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, embedded_depth_stencil_state.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateDepthStencilState error");

	// vertex_shader�̍쐬
	//cumulonimbus::shader_system::
	image_vs = std::make_unique<cumulonimbus::shader_system::VertexShader>(cumulonimbus::mapping::shader_filename::vs::Image_VS().c_str());
}
void FullscreenQuad::Blit(ID3D11DeviceContext* immediate_context,
	bool use_embedded_rasterizer_state	, bool use_embedded_depth_stencil_state,
	bool use_embedded_pixel_shader		, bool enable_msaa)
{
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> default_rasterizer_state;
	if (use_embedded_rasterizer_state)
	{
		immediate_context->RSGetState(default_rasterizer_state.ReleaseAndGetAddressOf());
		immediate_context->RSSetState(embedded_rasterizer_state.Get());
	}

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_depth_stencil_state;
	if (use_embedded_depth_stencil_state)
	{
		immediate_context->OMGetDepthStencilState(default_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
		immediate_context->OMSetDepthStencilState(embedded_depth_stencil_state.Get(), 1);
	}
	if (use_embedded_pixel_shader)
	{
		immediate_context->PSSetShader(enable_msaa ? embedded_pixel_shaders[1].Get() : embedded_pixel_shaders[0].Get(), 0, 0);
	}

	immediate_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(nullptr);

	//immediate_context->VSSetShader(embedded_vertex_shader.Get(), 0, 0);
	image_vs->BindVS(nullptr);

	immediate_context->Draw(4, 0);

	immediate_context->IASetInputLayout(nullptr);
	immediate_context->VSSetShader(nullptr, nullptr, 0);
	immediate_context->PSSetShader(nullptr, nullptr, 0);

	if (default_rasterizer_state)
	{
		immediate_context->RSSetState(default_rasterizer_state.Get());
	}
	if (default_depth_stencil_state)
	{
		immediate_context->OMSetDepthStencilState(default_depth_stencil_state.Get(), 1);
	}
}

void FullscreenQuad::Convolution(ID3D11DeviceContext* immediate_context,
	bool use_embedded_rasterizer_state, bool use_embedded_depth_stencil_state,
	bool use_embedded_pixel_shader, bool enable_msaa)
{
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> default_rasterizer_state;
	if (use_embedded_rasterizer_state)
	{
		immediate_context->RSGetState(default_rasterizer_state.ReleaseAndGetAddressOf());
		immediate_context->RSSetState(embedded_rasterizer_state.Get());
	}

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_depth_stencil_state;
	if (use_embedded_depth_stencil_state)
	{
		immediate_context->OMGetDepthStencilState(default_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
		immediate_context->OMSetDepthStencilState(embedded_depth_stencil_state.Get(), 1);
	}
	if (use_embedded_pixel_shader)
	{
		immediate_context->PSSetShader(enable_msaa ? embedded_pixel_shaders[1].Get() : embedded_pixel_shaders[0].Get(), 0, 0);
	}

	immediate_context->IASetVertexBuffers(0, 0, 0, 0, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(0);

	immediate_context->VSSetShader(embedded_vertex_shader.Get(), 0, 0);
	//image_vs->BindVS();

	immediate_context->Draw(4, 0);

	immediate_context->IASetInputLayout(0);
	immediate_context->VSSetShader(0, 0, 0);
	immediate_context->PSSetShader(0, 0, 0);

	if (default_rasterizer_state)
	{
		immediate_context->RSSetState(default_rasterizer_state.Get());
	}
	if (default_depth_stencil_state)
	{
		immediate_context->OMSetDepthStencilState(default_depth_stencil_state.Get(), 1);
	}
}