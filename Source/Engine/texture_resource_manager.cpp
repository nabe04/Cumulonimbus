#include "texture_resource_manager.h"

#include "string_helper.h"
#include "texture_filename_mapping.h"

namespace cumulonimbus::manager::texture
{
	TextureResourceManager::TextureResourceManager(ID3D11Device* device)
	{
		dummy_texture_white  = std::make_unique<DummyTexture>(device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f });
		dummy_texture_normal = std::make_unique<DummyTexture>(device, DirectX::XMFLOAT4{ 0.5f,0.5f,1.0f,1.0f });

		InitialLoad(device);
	}

	void TextureResourceManager::InitialLoad(ID3D11Device* device)
	{
		using namespace mapping;
		CreateTexture(device, texture_filename::NoImage256().c_str());
		CreateTexture(device, texture_filename::Coffee256().c_str());
		CreateTexture(device, texture_filename::UvChecker().c_str());
	}


	TextureResource* TextureResourceManager::CreateTexture(ID3D11Device* device, const std::string_view tex_filename)
	{
		const auto find = textures.find(tex_filename.data());
		if (find != textures.end())
		{// Already loaded
			return find->second.get();
		}

		const std::string str_filename = string_helper::toString(tex_filename);
		const auto tex = textures.insert(std::make_pair(str_filename, std::make_unique<TextureResource>(device, str_filename.c_str())));
		return tex.first->second.get();
	}

	void TextureResourceManager::RemoveTexture(const std::string_view tex_filename)
	{
		textures.erase(tex_filename.data());
	}
} // cumulonimbus::manager::texture
