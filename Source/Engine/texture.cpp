#include "texture.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <WICTextureLoader.h>

#include "string_helper.h"

TextureResource::TextureResource(ID3D11Device* device, const char* tex_filename)
{
	CreateTexture(device, tex_filename);
	//LoadTextureInfo();
}


void TextureResource::CreateTexture(ID3D11Device* device, const char* tex_filename)
{
	////const auto find = texture_resources.find(texture_filename);
	//const auto find = texture_resources.find(tex_filename);

	//if (find != texture_resources.end())
	//{// Allready loaded
	//	this->texture_data = find->second;
	//	return;
	//}

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
	else if (strcmp(exe, ".tga") == 0)
	{// Loading TGA file
		hr = DirectX::LoadFromTGAFile(string_helper::stringToWString(tex_filename).c_str(), &texture_data->metadata, texture_data->scratch);
	}
	else
	{// Loading WIC file
		hr = DirectX::LoadFromWICFile(string_helper::stringToWString(tex_filename).c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texture_data->metadata, texture_data->scratch);
	}

	//const HRESULT hr = DirectX::CreateWICTextureFromFile(device, string_helper::stringToWString(tex_filename).c_str(), nullptr, shader_resource_view.GetAddressOf());
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
	//this->device = device;
	CreateDummyTexture(device, DirectX::XMFLOAT4(1, 1, 1, 1));
}

//-- Creating a texture from a file name --//
OldTextureResource* TextureManager::CreateTexture(ID3D11Device* device, const std::string_view tex_filename)
{
	const auto find = textures.find(tex_filename.data());
	if (find != textures.end())
	{// Already loaded
		IncrementRefCount(tex_filename);
		return find->second.first.get();
	}

	//-- Newly loaded --//
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv{};
	const HRESULT hr = DirectX::CreateWICTextureFromFile(device, string_helper::stringToWString(tex_filename).c_str(), nullptr, srv.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateWICTextureFromFile error");

	const std::string strFilename = string_helper::toString(tex_filename);
	const auto tex = textures.insert(std::make_pair(strFilename, std::make_pair(std::make_unique<OldTextureResource>(strFilename, srv), 1)));
	return tex.first->second.first.get();
}

//-- Creating a dummy texture --//
OldTextureResource* TextureManager::CreateDummyTexture(ID3D11Device* device, const DirectX::XMFLOAT4& color)
{
	HRESULT hr;

	// Create Texture2D
	D3D11_TEXTURE2D_DESC					tex2d_desc	= {};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d		= {};
	{
		tex2d_desc.Format			= DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex2d_desc.Width			= 1;
		tex2d_desc.Height			= 1;
		tex2d_desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
		tex2d_desc.Usage			= D3D11_USAGE_DEFAULT;
		tex2d_desc.ArraySize		= 1;
		tex2d_desc.SampleDesc.Count	= 1;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem		= &color;
		initData.SysMemPitch	= sizeof(color);

		hr = device->CreateTexture2D(&tex2d_desc, &initData, tex2d.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateTexture2D error");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format						= tex2d_desc.Format;
	srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.ArraySize	= 1;
	srvDesc.Texture2DArray.MipLevels	= 1;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = {};
	hr = device->CreateShaderResourceView(tex2d.Get(), &srvDesc, srv.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateShaderResourceView error");

	dummy_texture->SetShaderResourceView(srv.Get());

	return dummy_texture.get();
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