#pragma once
#include <string>

namespace cumulonimbus::filename_helper
{
	/**
	 * @brief : 「Cumulonimbus」を返す
	 * @return : Cumulonimbus
	 */
	[[nodiscard]]
	std::string GetMyLibName();

	/**
	 * @brief : 「Data」を返す
	 * @return : Data
	 */
	[[nodiscard]]
	std::string GetData();

	/**
	 * @brief : 「Assets」を返す
	 * @return : Assets
	 */
	[[nodiscard]]
	std::string GetAssets();

	/**
	 * @brief : 「AssetSheet」を返す
	 * @return : AssetSheet
	 */
	[[nodiscard]]
	std::string GetAssetSheet();

	/**
	 * @brief : 「ProjectSetting」を返す
	 * @return : ProjectSetting
	 */
	[[nodiscard]]
	std::string GetProjectSetting();

	/**
	 * @brief : 「System」を返す
	 * @return : System
	 */
	[[nodiscard]]
	std::string GetSystem();

	/**
	 * @brief : 「Materials」を返す
	 * @return : Materials
	 */
	[[nodiscard]]
	std::string GetMaterials();

	/**
	 * @brief : 「Textures」を返す
	 * @return : Textures
	 */
	[[nodiscard]]
	std::string GetTextures();

	/**
	 * @brief : 「TemporarySave」を返す
	 * @return : TemporarySave
	 */
	[[nodiscard]]
	std::string GetTemporarySave();

	/**
	 * @brief : 「None」を返す
	 * @return : None
	 */
	[[nodiscard]]
	std::string GetNone();
	
	/**
	 * @brief : 「NoTitled」を返す
	 * @return : NoTitled
	 */
	[[nodiscard]]
	std::string GetNoTitled();

	/**
	 * @brief : 「Main Camera」を返す
	 * @return : Main Camera
	 */
	[[nodiscard]]
	std::string GetMainCamera();

} // cumulonimbus::filename_helper