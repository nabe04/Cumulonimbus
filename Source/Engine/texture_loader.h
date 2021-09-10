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
		 * @brief : テクスチャのロード
		 *
		 */
		void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path) override;

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		bool Supported(std::filesystem::path extension) override;

	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures;

		/**
		 * @brief : 任意のフォルダから指定された画像ファイル(「.png」,「.jpeg」,「.tga」,「.dds」)を
		 *			自分の管理するテクスチャフォルダにコピー
		 * @remark : 戻り地のUUID地がtexturesのキー値になる
		 * @param path : ファイル選択時のパスとファイル名(拡張子含む)
		 * @return : ファイルパスのUUID
		 */
		//mapping::rename_type::UUID Convert(const std::filesystem::path& path) override;
	};
} // cumulonimbus::asset
