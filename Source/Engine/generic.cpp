#include "generic.h"

#include "filename_helper.h"

namespace cumulonimbus::utility
{
	std::string ConvertAbsolutePathToRelativePath(const std::string& path)
	{
		// 絶対パスを相対パスに変換する
		const auto pos = path.find(filename_helper::GetMyLibName());
		if (pos == std::string::npos)
			assert(!"You may be trying to open from outside the current project.(MenuBar::FileMenu)");

		return { "." + path.substr(pos + filename_helper::GetMyLibName().size()) };
	}

} // cumulonimbus::utility