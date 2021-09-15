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

	std::string GetFbmExtension()
	{
		return { ".fbm" };
	}

	std::string GetSceneExtension()
	{
		return { ".scene" };
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
