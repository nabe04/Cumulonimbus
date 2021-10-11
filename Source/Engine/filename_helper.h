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