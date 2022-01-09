#pragma once
#include <filesystem>

#include <./Effekseer/Effekseer.h>

namespace cumulonimbus::asset
{
	class Effect final
	{
	public:
		explicit Effect(const std::string& file_path);
		~Effect();

	private:
		Effekseer::Effect* effect{ nullptr };
		std::filesystem::path file_path{};
	};
} //cumulonimbus::asset