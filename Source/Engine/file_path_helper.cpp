#include "file_path_helper.h"
#include <string_view>

namespace cumulonimbus::file_path_helper
{
	/**
	 * @brief  : 「.bin」を返す
	 * @return : .bin
	 */
	std::string GetBinExtension()
	{
		return { ".bin" };
	}

	/**
	 * @brief  : 「.json」を返す
 	 * @return : .json
	 */
	std::string GetJsonExtension()
	{
		return { ".json" };
	}

	/**
	 * @brief  : 「.scene」を返す
	 * @return : .scene
	 */
	std::string GetSceneExtension()
	{
		return { ".scene" };
	}

	/*
	 * brief  : 「Contents」を返す
	 * return : Contents
	 */
	std::string GetContentsFilename()
	{
		return { "Contents" };
	}

	/*
	* brief  : 「Components」を返す
	* return : Components
	*/
	std::string GetComponentsFilename()
	{
		return { "Components" };
	}

	/*
	 * brief  : 「Scenes」を返す
	 * return : Scene
	 */
	std::string GetScenesFilename()
	{
		return { "Scenes" };
	}

	/*
	 * brief  : Contentsフォルダまでの相対パスを取得
	 * return : ./Contents/
	 */
	constexpr std::string GetContentsDirectory()
	{
		return std::string{ "./" + GetContentsFilename() + "/" };
	}

	/*
	 * brief  : Sceneフォルダまでの相対パスを取得
	 * return : ./Contents/Scene/
	 */
	constexpr std::string GetSceneDirectory()
	{
		return std::string{ GetContentsDirectory() + GetScenesFilename() + "/" };
	}

	/*
	 * brief    : Contentsフォルダまでの相対パスとfilenameを足したものを返す
	 * filename : 「/」などは必要なくファイル名のみで良い
	 * return   : .Contents/「ファイル名」
	 */
	std::string AttachContentsDirectory(const std::string& filename)
	{
		return std::string{ GetContentsDirectory() + filename };
	}

	/*
	 * brief    : Sceneフォルダまでの相対パスとfilenameを足したものを返す
	 * filename : 「/」などは必要なくファイル名のみで良い
	 * return   : ./Contents/Scene/「ファイル名」
	 */
	std::string AttachSceneDirectory(const std::string& filename)
	{
		return std::string{ GetSceneDirectory() + filename};
	}


	/*
	 * brief  : Componentsフォルダまでの相対パスを取得
	 * return : ./Contents/Scenes/「ファイル名」/Components
	 */
	std::string AttachComponentsDirectory(const std::string& filename)
	{
		return std::string{ GetSceneDirectory() + filename + "/" + GetComponentsFilename()};
	}
}
