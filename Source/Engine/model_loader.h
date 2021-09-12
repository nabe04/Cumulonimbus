#pragma once
#include "loader.h"
#include "model.h"

namespace cumulonimbus::asset
{
	class ModelLoader final : public Loader
	{
	public:
		explicit ModelLoader() = default;
		~ModelLoader() override = default;
		/**
		 * @brief : アセットのロード
		 * @param : sheet_manager : AssetSheetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : アセットのロード
		 * @param : sheet_manager : AssetSheetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		bool Supported(std::filesystem::path extension) override;
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Model>> models{};
		
		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		void Load(AssetSheetManager& sheet_manager, const mapping::rename_type::UUID& id) override;
	};
} // cumulonimbus::asset
