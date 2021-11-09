#include "generic.h"

#include <regex>

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

	std::vector<std::filesystem::path> GetAllSubDirectoryFilePath(const std::string& path)
	{
		// �{���������p�X�����݂��Ȃ��ꍇ��̃p�X��Ԃ�
		if (!std::filesystem::exists(path))
			return {};

		std::vector<std::filesystem::path> paths{};
		for (const auto& file : std::filesystem::recursive_directory_iterator(path))
		{
			std::string get_path = file.path().string();
			// "\\"�������܂܂�Ă����ꍇ"/"�ɕ�����ϊ�����
			if (get_path.find('\\') != std::string::npos)
				get_path = std::regex_replace(get_path, std::regex("\\\\"), "/");

			paths.emplace_back(get_path);
		}

		return paths;
	}
} // cumulonimbus::utility