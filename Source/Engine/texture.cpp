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

		format = metadata.format;
		width = static_cast<u_int>(metadata.width);
		height = static_cast<u_int>(metadata.height);
		file_path = tex_filename;
		filename = name;
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
	dummy_texture_white = std::make_unique<DummyTexture>(device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f });
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