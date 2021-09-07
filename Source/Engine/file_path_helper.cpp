#include "file_path_helper.h"
#include <string_view>

namespace cumulonimbus::file_path_helper
{
	/**
	 * @brief  : �u.bin�v��Ԃ�
	 * @return : .bin
	 */
	std::string GetBinExtension()
	{
		return { ".bin" };
	}

	/**
	 * @brief  : �u.json�v��Ԃ�
 	 * @return : .json
	 */
	std::string GetJsonExtension()
	{
		return { ".json" };
	}

	/**
	 * @brief  : �u.scene�v��Ԃ�
	 * @return : .scene
	 */
	std::string GetSceneExtension()
	{
		return { ".scene" };
	}

	/*
	 * brief  : �uContents�v��Ԃ�
	 * return : Contents
	 */
	std::string GetContentsFilename()
	{
		return { "Contents" };
	}

	/*
	* brief  : �uComponents�v��Ԃ�
	* return : Components
	*/
	std::string GetComponentsFilename()
	{
		return { "Components" };
	}

	/*
	 * brief  : �uScenes�v��Ԃ�
	 * return : Scene
	 */
	std::string GetScenesFilename()
	{
		return { "Scenes" };
	}

	/*
	 * brief  : Contents�t�H���_�܂ł̑��΃p�X���擾
	 * return : ./Contents/
	 */
	constexpr std::string GetContentsDirectory()
	{
		return std::string{ "./" + GetContentsFilename() + "/" };
	}

	/*
	 * brief  : Scene�t�H���_�܂ł̑��΃p�X���擾
	 * return : ./Contents/Scene/
	 */
	constexpr std::string GetSceneDirectory()
	{
		return std::string{ GetContentsDirectory() + GetScenesFilename() + "/" };
	}

	/*
	 * brief    : Contents�t�H���_�܂ł̑��΃p�X��filename�𑫂������̂�Ԃ�
	 * filename : �u/�v�Ȃǂ͕K�v�Ȃ��t�@�C�����݂̂ŗǂ�
	 * return   : .Contents/�u�t�@�C�����v
	 */
	std::string AttachContentsDirectory(const std::string& filename)
	{
		return std::string{ GetContentsDirectory() + filename };
	}

	/*
	 * brief    : Scene�t�H���_�܂ł̑��΃p�X��filename�𑫂������̂�Ԃ�
	 * filename : �u/�v�Ȃǂ͕K�v�Ȃ��t�@�C�����݂̂ŗǂ�
	 * return   : ./Contents/Scene/�u�t�@�C�����v
	 */
	std::string AttachSceneDirectory(const std::string& filename)
	{
		return std::string{ GetSceneDirectory() + filename};
	}


	/*
	 * brief  : Components�t�H���_�܂ł̑��΃p�X���擾
	 * return : ./Contents/Scenes/�u�t�@�C�����v/Components
	 */
	std::string AttachComponentsDirectory(const std::string& filename)
	{
		return std::string{ GetSceneDirectory() + filename + "/" + GetComponentsFilename()};
	}
}
