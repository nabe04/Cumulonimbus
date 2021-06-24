#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <cereal/cereal.hpp>


class TextureResource final
{
private:
	struct TextureData
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view;
		std::string file_path;
		std::string filename;
		DXGI_FORMAT format;
		u_int		width;
		u_int		height;
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
			tex2d_desc.Format			= DXGI_FORMAT_R32G32B32A32_FLOAT;
			tex2d_desc.Width			= 1;
			tex2d_desc.Height			= 1;
			tex2d_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
			tex2d_desc.Usage			= D3D11_USAGE_DEFAULT;
			tex2d_desc.ArraySize		= 1;
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


class TextureManager final
{
private:
	std::map<std::string, std::pair<std::unique_ptr<TextureResource>, int>> textures = {};
	std::unique_ptr<DummyTexture> dummy_texture_white = {};

private:
	TextureManager()
	{
		//dummy_texture_white = std::make_unique<DummyTexture>(std::string{}, nullptr);
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

	// Reference count
	void IncrementRefCount(const std::string_view tex_filename);
	void DecrementRefCount(const std::string_view tex_filename);
};