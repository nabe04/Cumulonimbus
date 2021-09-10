#pragma once
#include "loader.h"

namespace cumulonimbus::asset
{
	class ModelLoader final : public Loader
	{
	public:
		explicit ModelLoader() = default;
		~ModelLoader() override = default;
		void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path) override;
		bool Supported(std::filesystem::path extension) override;
	private:
		mapping::rename_type::UUID Convert(const std::filesystem::path& path) override;
	};
} // cumulonimbus::asset
