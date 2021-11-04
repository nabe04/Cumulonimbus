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
} // cumulonimbus::filename_helper