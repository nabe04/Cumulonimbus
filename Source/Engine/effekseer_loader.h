#pragma once
#include "loader.h"
#include "effect.h"

namespace cumulonimbus::asset
{
	class EffekseerLoader final : public Loader
	{
	public:
		explicit EffekseerLoader()  = default;
		~EffekseerLoader() override = default;

		/**
		 * @brief : アセットのロード
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : アセットのロード
		 * @param asset_manager : AssetManagerクラスの参照
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
		 * @brief : アセットの名前変更
		 * @remark : 変更先のクラスでasset_idが存在しない場合処理を抜ける
		 * @remark : テクスチャアセットにはテクスチャファイルの変更する
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : リネーム対象のアセットID
		 * @param changed_name : 変更後のファイル名(※ファイルパスや拡張子は含まない)
		 */
		void Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name) override;

		/**
		 * @brief : アセットの削除
		 * @remark : ※caution : 削除したいパスが存在しない場合処理を飛ばす
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param path : 削除したいファイルパス
		 */
		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @breief : アセットの削除
		 * @remark :  ※caution : 削除したいIDが存在しない場合処理を飛ばす
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : 削除したいアセットのID(UUID)
		 */
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		[[nodiscard]]
		bool Supported(std::filesystem::path extension) override;

	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Effect>> effects{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		void DeleteEffect(const mapping::rename_type::UUID& efk_id, const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset
