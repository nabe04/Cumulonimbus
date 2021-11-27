#include "file_path_helper.h"
#include <string_view>

namespace cumulonimbus::file_path_helper
{
	std::string GetBinExtension()
	{
		return { ".bin" };
	}

	std::string GetJsonExtension()
	{
		return { ".json" };
	}

	std::string GetFbxExtension()
	{
		return { ".fbx" };
	}

	std::string GetFbmExtension()
	{
		return { ".fbm" };
	}

	std::string GetSceneExtension()
	{
		return { ".scene" };
	}

	std::string GetSystemExtension()
	{
		return { ".sys" };
	}

	std::string GetWaveSystemExtension()
	{
		return { ".wsys" };
	}

	std::string GetLastSceneExtension()
	{
		return { "lsn" };
	}

	std::string GetSheetExtension()
	{
		return { ".sheet" };
	}

	std::string GetModelExtension()
	{
		return { ".model" };
	}

	std::string GetMaterialExtension()
	{
		return { ".mat" };
	}

	std::string GetPrefabExtension()
	{
		return { ".prefab" };
	}

	std::string GetWaveExtension()
	{
		return { ".wave" };
	}

	std::string GetContentsFilename()
	{
		return { "Contents" };
	}

	std::string GetEntitiesFilename()
	{
		return { "Entities" };
	}


	std::string GetComponentsFilename()
	{
		return { "Components" };
	}

	std::string GetScenesFilename()
	{
		return { "Scenes" };
	}

	std::string GetSaveSceneViewFilePathAndName()
	{
		return { "./Data/SceneView/save_scene" };
	}

	bool SupportedTextureExtension(const std::filesystem::path& path)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".png" , ".PNG",
			".jpeg", ".JPEG",
			".tga" , ".TGA",
			".dds" , ".DDS"
		};
		return extensions.contains(path.extension());
	}

	constexpr std::string GetContentsDirectory()
	{
		return std::string{ "./" + GetContentsFilename() + "/" };
	}

	constexpr std::string GetSceneDirectory()
	{
		return std::string{ GetContentsDirectory() + GetScenesFilename() + "/" };
	}

	std::string AttachContentsDirectory(const std::string& filename)
	{
		return std::string{ GetContentsDirectory() + filename };
	}

	std::string AttachSceneDirectory(const std::string& filename)
	{
		return std::string{ GetSceneDirectory() + filename};
	}

	std::string AttachComponentsDirectory(const std::string& filename)
	{
		return std::string{ GetSceneDirectory() + filename + "/" + GetComponentsFilename()};
	}
}
