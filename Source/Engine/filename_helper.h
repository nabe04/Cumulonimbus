#pragma once
#include <string>

namespace cumulonimbus::filename_helper
{
	/**
	 * @brief : �uCumulonimbus�v��Ԃ�
	 * @return : Cumulonimbus
	 */
	[[nodiscard]]
	std::string GetMyLibName();

	/**
	 * @brief : �uData�v��Ԃ�
	 * @return : Data
	 */
	[[nodiscard]]
	std::string GetData();

	/**
	 * @brief : �uAssets�v��Ԃ�
	 * @return : Assets
	 */
	[[nodiscard]]
	std::string GetAssets();

	/**
	 * @brief : �uAssetSheet�v��Ԃ�
	 * @return : AssetSheet
	 */
	[[nodiscard]]
	std::string GetAssetSheet();

	/**
	 * @brief : �uProjectSetting�v��Ԃ�
	 * @return : ProjectSetting
	 */
	[[nodiscard]]
	std::string GetProjectSetting();

	/**
	 * @brief : �uSystem�v��Ԃ�
	 * @return : System
	 */
	[[nodiscard]]
	std::string GetSystem();

	/**
	 * @brief : �uMaterials�v��Ԃ�
	 * @return : Materials
	 */
	[[nodiscard]]
	std::string GetMaterials();

	/**
	 * @brief : �uTextures�v��Ԃ�
	 * @return : Textures
	 */
	[[nodiscard]]
	std::string GetTextures();

	/**
	 * @brief : �uTemporarySave�v��Ԃ�
	 * @return : TemporarySave
	 */
	[[nodiscard]]
	std::string GetTemporarySave();

	/**
	 * @brief : �uNone�v��Ԃ�
	 * @return : None
	 */
	[[nodiscard]]
	std::string GetNone();
	
	/**
	 * @brief : �uNoTitled�v��Ԃ�
	 * @return : NoTitled
	 */
	[[nodiscard]]
	std::string GetNoTitled();

	/**
	 * @brief : �uMain Camera�v��Ԃ�
	 * @return : Main Camera
	 */
	[[nodiscard]]
	std::string GetMainCamera();

} // cumulonimbus::filename_helper