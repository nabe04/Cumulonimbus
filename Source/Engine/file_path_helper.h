#pragma once
#include <string>

namespace cumulonimbus::file_path_helper
{
	/*
	 * brief  : �u.bin�v��Ԃ�
	 * return : .bin
	 */
	[[nodiscard]] std::string GetBinExtension();

	/*
	 * brief  : �u.json�v��Ԃ�
	 * return : .json
	 */
	[[nodiscard]] std::string GetJsonExtension();

	/*
	 * brief  : �uContents�v��Ԃ�
	 * return : Contents
	 */
	[[nodiscard]] std::string GetContentsFilename();

	/*
	* brief  : �uComponents�v��Ԃ�
	* return : Components
	*/
	[[nodiscard]] std::string GetComponentsFilename();

	/*
	 * brief  : �uScenes�v��Ԃ�
	 * return : Scene
	 */
	[[nodiscard]] std::string GetScenesFilename();

	/*
	 * brief  : Contents�t�H���_�܂ł̑��΃p�X���擾
	 * return : ./Contents/
	 */
	[[nodiscard]] constexpr std::string GetContentsDirectory();

	/*
	 * brief  : Scene�t�H���_�܂ł̑��΃p�X���擾
	 * return : ./Contents/Scene/
	 */
	[[nodiscard]] constexpr std::string GetSceneDirectory();

	/*
	 * brief    : Contents�t�H���_�܂ł̑��΃p�X��filename�𑫂������̂�Ԃ�
	 * filename : �u/�v�Ȃǂ͕K�v�Ȃ��t�@�C�����݂̂ŗǂ�
	 * return   : .Contents/�u�t�@�C�����v
	 */
	[[nodiscard]] std::string AttachContentsDirectory(const std::string& filename);

	/*
	 * brief    : Scene�t�H���_�܂ł̑��΃p�X��filename�𑫂������̂�Ԃ�
	 * filename : �u/�v�Ȃǂ͕K�v�Ȃ��t�@�C�����݂̂ŗǂ�
	 * return   : ./Contents/Scene/�u�t�@�C�����v
	 */
	[[nodiscard]] std::string AttachSceneDirectory(const std::string& filename);

	 /*
	  * brief  : Components�t�H���_�܂ł̑��΃p�X���擾
	  * return : ./Contents/Scenes/�u�t�@�C�����v/Components
	  */
	[[nodiscard]] std::string AttachComponentsDirectory(const std::string& filename);

	
	/*
	 * brief : T�^��ComponentName��component_arrays�̃L�[�l�ɓo�^����ۂ̖��O�̓��o
	 */
	template<typename T>
	[[nodiscard]] std::string GetTypeName()
	{
		std::string_view type_name(typeid(T).name());

		std::string_view search_name = ":";
		std::size_t pos = type_name.find_last_of(search_name);
		if (pos != std::string_view::npos)
		{// namespace�����O�Ɋ܂܂�Ă����ꍇ�Ȃ�
		 // ��)cumulonimbus::ecs::sample  ��  sample
			type_name.remove_prefix(pos + 1);
			return type_name.data();
		}

		search_name = " ";
		pos = type_name.find_first_of(search_name);
		if (pos != std::string_view::npos)
		{// class �uT�^�̖��O�̏ꍇ�v
		 // ��)class sample ��  sample
			type_name.remove_prefix(pos + 1);
			return type_name.data();
		}

		// ��L�ȊO
		return type_name.data();
	}
}