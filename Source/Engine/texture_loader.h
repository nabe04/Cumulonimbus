#pragma once
#include "loader.h"

namespace cumulonimbus::asset
{
	class Texture;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	class TextureLoader final : public Loader
	{
	public:
		~TextureLoader() override = default;
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

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		bool Supported(std::filesystem::path extension) override;
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures{};

		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		void Load(AssetSheetManager& sheet_manager, const mapping::rename_type::UUID& id) override;
	};
} // cumulonimbus::asset
