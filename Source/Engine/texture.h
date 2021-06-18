#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

class TextureResource final
{
private:
	struct TextureData
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view;
		std::string file_path;
		std::string filename;
		DXGI_FORMAT format;
		u_int width;
		u_int height;
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage scratch;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(file_path),
				CEREAL_NVP(filename)
			);
		}
	};

	std::shared_ptr<TextureData> texture_data;
public:
	TextureResource(ID3D11Device* device, const char* tex_filename);
	TextureResource() = default; // for cereal
	[[nodiscard]] const TextureData* GetTextureData() const { return texture_data.get(); }
	
	template<class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(texture_data)
		);
	}
private:
	void CreateTexture(ID3D11Device* device, const char* tex_filename);
};

class DummyTexture
{
public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dummy_texture;

private:
	void CreateDummyTexture(ID3D11Device* device, const DirectX::XMFLOAT4& color)
	{
		HRESULT hr;

		// Create Texture2D
		D3D11_TEXTURE2D_DESC					tex2d_desc = {};
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d = {};
		{
			tex2d_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			tex2d_desc.Width = 1;
			tex2d_desc.Height = 1;
			tex2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tex2d_desc.Usage = D3D11_USAGE_DEFAULT;
			tex2d_desc.ArraySize = 1;
			tex2d_desc.SampleDesc.Count = 1;

			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = &color;
			initData.SysMemPitch = sizeof(color);

			hr = device->CreateTexture2D(&tex2d_desc, &initData, tex2d.GetAddressOf());
			if (FAILED(hr))
				assert(!"CreateTexture2D error");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = tex2d_desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2DArray.ArraySize = 1;
		srvDesc.Texture2DArray.MipLevels = 1;

		hr = device->CreateShaderResourceView(tex2d.Get(), &srvDesc, dummy_texture.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateShaderResourceView error(dummy texture)");
	}

public:
	DummyTexture(ID3D11Device* device, const DirectX::XMFLOAT4& color)
	{
		CreateDummyTexture(device, color);
	}
};

class OldTextureResource final
{
private:
	std::string tex_filename = {};

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = {};
	D3D11_TEXTURE2D_DESC							 texture2D_Desc		  = {};

public:
	OldTextureResource(const std::string& tex_filename, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
		: tex_filename(tex_filename)
		, shader_resource_view(std::move(srv)){}
	~OldTextureResource() = default;

	void   SetShaderResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv) { shader_resource_view = srv; }

	auto&  GetTextureFilename() const		{ return tex_filename; }
	auto*  GetShaderResourceView()			{ return shader_resource_view.Get(); }
	auto** GetShaderResourceViewAddress()	{ return shader_resource_view.GetAddressOf(); }

};

class TextureManager final
{
private:
	std::map<std::string, std::pair<std::unique_ptr<TextureResource>, int>> textures = {};
	std::unique_ptr<OldTextureResource> dummy_texture = {};
	//Microsoft::WRL::ComPtr<ID3D11Device> device = {};

private:
	TextureManager()
	{
		dummy_texture = std::make_unique<OldTextureResource>(std::string{}, nullptr);
	}

	void RemoveTexture(const std::string_view tex_filename);

public:
	~TextureManager() = default;

	// Singleton used
	static TextureManager* GetInstance()
	{
		static TextureManager instance{};
		return &instance;
	}

	void Initialize(ID3D11Device* device);

	TextureResource* CreateTexture(ID3D11Device* device,const std::string_view tex_filename);
	OldTextureResource* CreateDummyTexture(ID3D11Device* device, const DirectX::XMFLOAT4& color);

	// Reference count
	void IncrementRefCount(const std::string_view tex_filename);
	void DecrementRefCount(const std::string_view tex_filename);

	OldTextureResource* GetDummyTexture() { return dummy_texture.get(); }
};