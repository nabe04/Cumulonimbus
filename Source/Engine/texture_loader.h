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
		explicit TextureLoader();
		~TextureLoader() override = default;

		/**
		 * @brief : アセットのロード
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : アセットのロード
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;

		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 * @remark : idに登録されているアセットの作成のみを行う
		 *			 ※フォルダのコピー、作成などは行わない
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		bool Supported(std::filesystem::path extension) override;

		/**
		 * @brief : ファイルパスからテクスチャの作成
		 * @remark : ※Load関数との違い
		 * @remark : フォルダのコピーを行わずpathに指定された
		 *			 ファイルパスを元にテクスチャを作成する
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param path : テクスチャまでのファイルパス(※拡張子含む)
		 */
		void CreateTexture(AssetManager& asset_manager, const std::filesystem::path& path);

		/**
		 * @brief : 取得したいテクスチャのID(UUID)を元にテクスチャを取得
		 * @param id : 取得したいテクスチャのID(UUID)
		 * @return : 取得したテクスチャの参照
		 */
		Texture& GetTexture(const mapping::rename_type::UUID& id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures{};
		std::unique_ptr<Texture> dummy_texture{};

		mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
	};
} // cumulonimbus::asset