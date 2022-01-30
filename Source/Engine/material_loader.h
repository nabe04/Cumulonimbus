#pragma once
#include "loader.h"
#include "material.h"

namespace cumulonimbus::asset
{
	class MaterialLoader final : public Loader
	{
	public:
		explicit MaterialLoader();
		~MaterialLoader() override = default;

		/**
		 * @brief : アセット(「.mat」)のロード
		 * @param  asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : アセット(「.mat」)のロード
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : アセットの名前変更
		 * @remark : 変更先のクラスでasset_idが存在しない場合処理を抜ける
		 * @remark : マテリアルのアセットには「.mat」と「.json」ファイルが
		 *			 存在するのでこれらのファイル名を変更する
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
		bool Supported(std::filesystem::path extension) override;

		/**
		 * @brief : 指定されたidのマテリアルを保持しているか
		 */
		[[nodiscard]]
		bool HasMaterial(const mapping::rename_type::UUID& mat_id) const;

		/**
		 * @brief : 新規マテリアルの作成
		 * @remark : material_nameを記述しない場合名前を「New Material」にする
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param parent_path : 保存するマテリアルの親階層のファイルパス
		 * @param material_data : Material構造体のパラメータ
		 * @param material_name : マテリアル名(※拡張子不要)
		 * @return : 作成されたマテリアルID(UUID)
		 */
		[[nodiscard]]
		mapping::rename_type::UUID CreateMaterial(
			AssetManager& asset_manager, const std::filesystem::path& parent_path,
			const MaterialData& material_data = {}, std::string material_name = { "" });

		/**
		 * @brief : 取得したいマテリアルのID(UUID)を元にマテリアルを取得
		 * @param mat_id : 取得したいマテリアルのID(UUID)
		 * @return : 取得したマテリアルの参照
		 */
		Material& GetMaterial(const mapping::rename_type::UUID& mat_id);
		/**
		 * @brief : マテリアル名の取得
		 * @param mat_id : 取得したいマテリアルID(UUID)
		 * @return : 指定されたマテリアル名(IDが存在しない場合空文字""を返す)
		 */
		[[nodiscard]]
		std::string GetMaterialName(const mapping::rename_type::UUID& mat_id) const;
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Material>> materials{};
		std::unique_ptr<Material> dummy_material{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : マテリアルアセットとフォルダ内のマテリアルファイルの削除
		 * @param mat_id : マテリアルID(UUID)
		 * @param delete_path : 削除先のマテリアルフォルダパス
		 */
		void DeleteMaterial(
			const mapping::rename_type::UUID& mat_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset
