#pragma once

#include <cassert>
#include <map>
#include <string>

#include "fbx_model_resource.h"
#include "texture.h"

class ResourceManager final
{
private:
	// FBX
	std::map<std::string, std::shared_ptr<FbxModelResource>> fbx_model_resources;
	// Texture
	std::map<std::string, std::shared_ptr<TextureResource>> texture_resources;

	// For ImGui
	std::vector<std::string> fbx_model_names;
	std::vector<std::string> texture_filenames;

public:
	explicit ResourceManager() = default;
	~ResourceManager() = default;

	[[noreturn]] void Initialize(ID3D11Device* device);

	template<class T>
	void Register(const std::shared_ptr<T>& resource)
	{
		if constexpr (typeid(T) == typeid(FbxModelResource))
		{
			AddFBXData(resource);
			return;
		}
		else if constexpr (typeid(T) == typeid(TextureResource))
		{
			AddTextureData(resource);
		}
		else
		{
			static_assert(false, "Not found type (class : ResourceManager::Resister)");
		}

		assert(!"Not found resource type");
	}

#pragma region FBX
	[[nodiscard]] const std::map<std::string, std::shared_ptr<FbxModelResource>>& FbxModelResources() { return fbx_model_resources; }

	[[nodiscard]] const std::shared_ptr<FbxModelResource>& FbxModelResouece(std::string filename)
	{
		if (fbx_model_resources.contains(filename))
			return fbx_model_resources.at(filename);

		assert(!"Not found filename (class : ResourceManager::FbxModelResouece)");
		static std::shared_ptr<FbxModelResource> p = nullptr;
		return p;
	}
#pragma endregion FBX

#pragma region Texture
	[[nodiscard]] const std::map<std::string, std::shared_ptr<TextureResource>>& GetTextureResources() { return texture_resources; }

	[[nodiscard]] const std::shared_ptr<TextureResource>& GetTextureResouece(std::string filename)
	{
		if (texture_resources.contains(filename))
			return texture_resources.at(filename);

		assert(!"Not found filename (class : ResourceManager::TextureResouece)");
		static std::shared_ptr<TextureResource> p = nullptr;
		return p;
	}

#pragma endregion Texture

	// For ImGui
	[[nodiscard]] std::vector<std::string> FbxModelNames() { return fbx_model_names; }

private:
	void AddFBXData(const std::shared_ptr<FbxModelResource>& resource)
	{
		// ファイル名が既に存在していれば追加しない
		if (fbx_model_resources.contains(resource->GetModelName()))
			return;

		fbx_model_names.emplace_back(resource->GetModelName());
		fbx_model_resources.insert(std::make_pair(resource->GetModelName(), resource));
	}

	void AddTextureData(const std::shared_ptr<TextureResource>& resource)
	{
		// ファイル名が既に存在していれば追加しない
		if (texture_resources.contains(resource->GetTextureData()->filename))
			return;

		texture_filenames.emplace_back(resource->GetTextureData()->filename);
		texture_resources.insert(std::make_pair(resource->GetTextureData()->filename, resource));
	}
};