#include <vector>
#include "dx_manager.h"
#include "imgui_manager.h"

//***********************************
//
//	General Initialization
//
//***********************************
bool DxManager::Initialize(HWND hwnd,ID3D11Device** device,ID3D11DeviceContext** immediate_context, int width, int height)
{
	CreateDevice(hwnd,device,immediate_context);
	InitializeRenderTarget();

	return true;
}

//************************************
//
//	Create Device
//
//************************************
HRESULT DxManager::CreateDevice(HWND hwnd,ID3D11Device** device, ID3D11DeviceContext** immediate_context)
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(hwnd, &rc);
	screen_width =  rc.right - rc.left;
	screen_height = rc.bottom - rc.top;

	UINT create_device_flags = 0; // D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS
#if defined( DEBUG ) || defined( _DEBUG )
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	D3D_FEATURE_LEVEL featureLels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	D3D_FEATURE_LEVEL feature_level{};
	for (D3D_DRIVER_TYPE driver_type : driver_types)
	{
		//-- Create Device --//
		hr = D3D11CreateDevice(
			nullptr, driver_type,
			nullptr, create_device_flags, featureLels,
			ARRAYSIZE(featureLels),
			D3D11_SDK_VERSION,
			this->device.GetAddressOf(),
			&feature_level,
			this->immediate_context.GetAddressOf());
		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	bool is_window = true;
#ifndef _DEBUG
	is_window = false;
#endif


	//-- Create SwapChain --//
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width					= screen_width;
	scd.BufferDesc.Height					= screen_height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //DXGI_FORMAT_R8G8B8A8_UNORM_SRGB DXGI_FORMAT_R8G8B8A8_UNORM
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	UINT msaa_quality_level;
	UINT sample_count = 1;
	hr = this->device->CheckMultisampleQualityLevels(scd.BufferDesc.Format, sample_count, &msaa_quality_level);
	if (FAILED(hr))
		assert(!"CheckMultisampleQualityLevels error");
	scd.SampleDesc.Count					= 1;
	scd.SampleDesc.Quality					= 0;
	scd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount							= 1;
	scd.OutputWindow						= hwnd;
	scd.Windowed							= is_window;
	scd.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags								= 0;
	Microsoft::WRL::ComPtr<IDXGIFactory>  dxgi_factory;
	Microsoft::WRL::ComPtr < IDXGIAdapter> dxgi_adapter;
	hr = this->device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);
	if (FAILED(hr))
		assert(!"QueryInterface error");

	hr = dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf());
	if (FAILED(hr))
		assert(!"GetAdapter error");

	dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgi_factory);
	hr = dxgi_factory->CreateSwapChain(this->device.Get(), &scd, swap_chain.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateSwapChain error");

	*device = this->device.Get();
	//(*device)->AddRef();
	this->device->AddRef();

	*immediate_context = this->immediate_context.Get();
	//(*immediate_context)->AddRef();
	this->immediate_context->AddRef();

	return S_OK;
}


//**************************************
//
//	Render Target Relationship
//
//**************************************
//------------------------------
//	Initialize
//------------------------------
bool DxManager::InitializeRenderTarget()
{
	// Get BackBuffer
	ID3D11Texture2D* back_buffer = nullptr;
	HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	if (FAILED(hr))
		assert(!"GetBuffer(ID3D11Texture2D) error");

	// Create RenderTargetView
	hr = device->CreateRenderTargetView(back_buffer, NULL, render_target_view.GetAddressOf());
	back_buffer->Release();
	back_buffer = nullptr;
	if (FAILED(hr))
		assert(!"CreateRenderTargetView error");

	// Create DepthStencil Buffer
	CreateDepthStencil();

	// Set RenderTargetView
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	// Set Viewport
	SetViewPort(screen_width, screen_height);

	return true;
}

//------------------------------
// Set up Viewport
//------------------------------
void DxManager::SetViewPort(int width, int height)
{
	D3D11_VIEWPORT vp;
	vp.Width	= (FLOAT)width;
	vp.Height	= (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediate_context->RSSetViewports(1, &vp);
}

//--------------------------------
// Create DepthStencil Buffer
//--------------------------------
bool DxManager::CreateDepthStencil()
{
	// Set up DepthStencil (Required to create a DepthStencilView)
	D3D11_TEXTURE2D_DESC td = {};
	td.Width				= screen_width;
	td.Height				= screen_height;
	td.MipLevels			= 1;
	td.ArraySize			= 1;
	td.Format				= DXGI_FORMAT_R24G8_TYPELESS;
	td.SampleDesc.Count		= 1;
	td.SampleDesc.Quality	= 0;
	td.Usage				= D3D11_USAGE_DEFAULT;
	td.BindFlags			= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags		= 0;
	td.MiscFlags			= 0;

	// Genarate DepthStencil Texture
	HRESULT hr = device->CreateTexture2D(&td, NULL, depth_stencil_texture.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateTexture2D error");

	// Set up DepthStencilViewDesc and create DepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd	= {};
	dsvd.Format							= DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice				= 0;
	hr = device->CreateDepthStencilView(depth_stencil_texture.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateDepthStencilView error");

	//-- Set up and create ShaderResourceView --//
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd	= {};
	srvd.Format								= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip			= 0;
	srvd.Texture2D.MipLevels				= 1;
	hr = device->CreateShaderResourceView(depth_stencil_texture.Get(), &srvd, shader_resource_view.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateShaderResourceView error");
	return true;
}


//------------------------------------
//	Clear
//------------------------------------
void DxManager::Clear(DWORD color)
{
	float clear_color[] = { 0.0f, 1.0f, 1.0f, 1.0f };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), clear_color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//-------------------------------------
//	Flip
//-------------------------------------
void DxManager::Flip(int n)
{
	//imgui::Render();

	// Set RenderTargetView
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
	//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<u_int>(DS_Type::DS_TRUE)].Get(), 1);
	swap_chain->Present(n, 0);
}