#include "generic.h"

#include "filename_helper.h"

namespace cumulonimbus::utility
{
	std::string ConvertAbsolutePathToRelativePath(const std::string& path)
	{
		// ��΃p�X�𑊑΃p�X�ɕϊ�����
		const auto pos = path.find(filename_helper::GetMyLibName());
		if (pos == std::string::npos)
			assert(!"You may be trying to open from outside the current project.(MenuBar::FileMenu)");

		return { "." + path.substr(pos + filename_helper::GetMyLibName().size()) };
	}

} // cumulonimbus::utility