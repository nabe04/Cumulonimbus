#pragma once
#include <string>

namespace cumulonimbus::file_path_helper
{
	/**
	 * @brief  : 「.bin」を返す
	 * @return : .bin
	 */
	[[nodiscard]] std::string GetBinExtension();

	/**
	 * @brief  : 「.json」を返す
	 * @return : .json
	 */
	[[nodiscard]] std::string GetJsonExtension();

	/**
	 * @brief  : 「.scene」を返す
	 * @return : .scene
	 */
	[[nodiscard]] std::string GetSceneExtension();

	/*
	 * brief  : 「Contents」を返す
	 * return : Contents
	 */
	[[nodiscard]] std::string GetContentsFilename();

	/*
	* brief  : 「Components」を返す
	* return : Components
	*/
	[[nodiscard]] std::string GetComponentsFilename();

	/*
	 * brief  : 「Scenes」を返す
	 * return : Scene
	 */
	[[nodiscard]] std::string GetScenesFilename();

	/*
	 * brief  : Contentsフォルダまでの相対パスを取得
	 * return : ./Contents/
	 */
	[[nodiscard]] constexpr std::string GetContentsDirectory();

	/*
	 * brief  : Sceneフォルダまでの相対パスを取得
	 * return : ./Contents/Scene/
	 */
	[[nodiscard]] constexpr std::string GetSceneDirectory();

	/*
	 * brief    : Contentsフォルダまでの相対パスとfilenameを足したものを返す
	 * filename : 「/」などは必要なくファイル名のみで良い
	 * return   : .Contents/「ファイル名」
	 */
	[[nodiscard]] std::string AttachContentsDirectory(const std::string& filename);

	/*
	 * brief    : Sceneフォルダまでの相対パスとfilenameを足したものを返す
	 * filename : 「/」などは必要なくファイル名のみで良い
	 * return   : ./Contents/Scene/「ファイル名」
	 */
	[[nodiscard]] std::string AttachSceneDirectory(const std::string& filename);

	 /*
	  * brief  : Componentsフォルダまでの相対パスを取得
	  * return : ./Contents/Scenes/「ファイル名」/Components
	  */
	[[nodiscard]] std::string AttachComponentsDirectory(const std::string& filename);


	/*
	 * brief : T型のComponentNameをcomponent_arraysのキー値に登録する際の名前の導出
	 */
	template<typename T>
	[[nodiscard]] std::string GetTypeName()
	{
		std::string_view type_name(typeid(T).name());

		std::string_view search_name = ":";
		std::size_t pos = type_name.find_last_of(search_name);
		if (pos != std::string_view::npos)
		{// namespaceが名前に含まれていた場合など
		 // 例)cumulonimbus::ecs::sample  →  sample
			type_name.remove_prefix(pos + 1);
			return type_name.data();
		}

		search_name = " ";
		pos = type_name.find_first_of(search_name);
		if (pos != std::string_view::npos)
		{// class 「T型の名前の場合」
		 // 例)class sample →  sample
			type_name.remove_prefix(pos + 1);
			return type_name.data();
		}

		// 上記以外
		return type_name.data();
	}
}
