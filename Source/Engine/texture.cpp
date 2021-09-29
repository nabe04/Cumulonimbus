#include "texture.h"

#include <cassert>
#include <filesystem>

#include <WICTextureLoader.h>

#include "string_helper.h"
#include "locator.h"

namespace cumulonimbus::asset
{
	Texture::Texture(ID3D11Device* device, const char* tex_filename)
	{
		CreateTexture(device, tex_filename);
	}

	Texture::Texture(ID3D11Device* device, const DirectX::SimpleMath::Vector4& color)
	{
		CreateDummyTexture(device, color);
	}

	void Texture::CreateTexture(ID3D11Device* device, const char* tex_filename)
	{
		char name[MAX_PATH]{};
		char exe[MAX_PATH]{};
		_splitpath_s(tex_filename, nullptr, NULL, nullptr, NULL, name, sizeof(name), exe, sizeof(exe));

		//-- Newly loaded --//
		HRESULT hr = E_FAIL;
		if (strcmp(exe, ".dds") == 0)
		{// Loading DDS file
			hr = DirectX::LoadFromDDSFile(string_helper::stringToWString(tex_filename).c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &metadata, scratch);
		}
		else if (strcmp(exe, ".tga") == 0 || strcmp(exe, ".TGA") == 0)
		{// Loading TGA file
			hr = DirectX::LoadFromTGAFile(string_helper::stringToWString(tex_filename).c_str(), &metadata, scratch);
		}
		else
		{// Loading WIC file
			hr = DirectX::LoadFromWICFile(string_helper::stringToWString(tex_filename).c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metadata, scratch);
		}

		if (FAILED(hr))
			assert(!"Load texture error");

		hr = DirectX::CreateShaderResourceView(device, scratch.GetImages(), scratch.GetImageCount(), metadata, texture_view.GetAddressOf());
		if (FAILED(hr))
			assert(!"Create shader resource view error");

		format		= metadata.format;
		width		= static_cast<u_int>(metadata.width);
		height		= static_cast<u_int>(metadata.height);
		file_path	= tex_filename;
		filename	= name;
	}

	void Texture::CreateDummyTexture(ID3D11Device* device, const DirectX::XMFLOAT4& color)
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

			format = tex2d_desc.Format;
			width  = 1;
			height = 1;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = tex2d_desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2DArray.ArraySize = 1;
		srvDesc.Texture2DArray.MipLevels = 1;

		hr = device->CreateShaderResourceView(tex2d.Get(), &srvDesc, texture_view.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateShaderResourceView error(dummy texture)");
	}


	void Texture::BindTexture(
		const mapping::graphics::ShaderStage shader_stage,
		const u_int tex_slot)
	{
		// shader_resource_viewのバインド
		using namespace mapping::graphics;
		locator::Locator::GetDx11Device()->BindShaderResource(shader_stage, texture_view.GetAddressOf(), tex_slot);
	}
	void Texture::UnbindTexture(
		const mapping::graphics::ShaderStage shader_stage,
		const u_int tex_slot)
	{
		// shader_resource_viewのアンバインド
		using namespace mapping::graphics;
		locator::Locator::GetDx11Device()->UnbindShaderResource(shader_stage, tex_slot);
	}

} // cumulonimbus::asset

// アセットシートができ次第消す
TextureResource::TextureResource(ID3D11Device* device, const char* tex_filename)
{
	CreateTexture(device, tex_filename);
	//LoadTextureInfo();
}


void TextureResource::CreateTexture(ID3D11Device* device, const char* tex_filename)
{
	texture_data = std::make_shared<TextureData>();
	char name[MAX_PATH]{};
	char exe[MAX_PATH]{};
	_splitpath_s(tex_filename, nullptr, NULL, nullptr, NULL, name, sizeof(name), exe, sizeof(exe));

	//-- Newly loaded --//
	HRESULT hr = E_FAIL;
	if (strcmp(exe, ".dds") == 0)
	{// Loading DDS file
		hr = DirectX::LoadFromDDSFile(string_helper::stringToWString(tex_filename).c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &texture_data->metadata, texture_data->scratch);
	}
	else if (strcmp(exe, ".tga") == 0 || strcmp(exe, ".TGA") == 0)
	{// Loading TGA file
		hr = DirectX::LoadFromTGAFile(string_helper::stringToWString(tex_filename).c_str(), &texture_data->metadata, texture_data->scratch);
	}
	else
	{// Loading WIC file
		hr = DirectX::LoadFromWICFile(string_helper::stringToWString(tex_filename).c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texture_data->metadata, texture_data->scratch);
	}

	if (FAILED(hr))
		assert(!"Load texture error");

	hr = DirectX::CreateShaderResourceView(device, texture_data->scratch.GetImages(), texture_data->scratch.GetImageCount(), texture_data->metadata, texture_data->texture_view.GetAddressOf());
	if (FAILED(hr))
		assert(!"Create shader resource view error");

	texture_data->format	= texture_data->metadata.format;
	texture_data->width		= static_cast<u_int>(texture_data->metadata.width);
	texture_data->height	= static_cast<u_int>(texture_data->metadata.height);
	texture_data->file_path = tex_filename;
	texture_data->filename  = name;

	//const std::string strFilename = string_helper::toString(name);
	//const auto tex = texture_resources.insert(std::make_pair(strFilename, texture_data));
}

//-----------------------------------------
//
//	TextureManager class
//
//-----------------------------------------

void TextureManager::Initialize(ID3D11Device* device)
{
	dummy_texture_white = std::make_unique<cumulonimbus::asset::DummyTexture>(device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f });
}

//-- Creating a texture from a file name --//
TextureResource* TextureManager::CreateTexture(ID3D11Device* device, const std::string_view tex_filename)
{
	const auto find = textures.find(tex_filename.data());
	if (find != textures.end())
	{// Already loaded
		IncrementRefCount(tex_filename);
		return find->second.first.get();
	}

	const std::string strFilename = string_helper::toString(tex_filename);
	const auto tex = textures.insert(std::make_pair(strFilename, std::make_pair(std::make_unique<TextureResource>(device, strFilename.c_str()), 1)));
	return tex.first->second.first.get();
}

//-- Increase the reference count --//
void TextureManager::IncrementRefCount(const std::string_view tex_filename)
{
	++textures.find(tex_filename.data())->second.second;
}

//-- Reduce the reference count --//
void TextureManager::DecrementRefCount(const std::string_view tex_filename)
{
	if (--textures.find(tex_filename.data())->second.second <= 0)
	{
		// Release texture as reference count reaches 0
		RemoveTexture(tex_filename);
	}
}

// Release texture
void TextureManager::RemoveTexture(const std::string_view tex_filename)
{
	textures.erase(tex_filename.data());
}