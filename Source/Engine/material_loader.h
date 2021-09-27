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
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : アセット(「.mat」)のロード
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : マテリアルアセットの削除
		 */
		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		bool Supported(std::filesystem::path extension) override;

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
			const MaterialData& material_data = {}, std::string material_name = {""});

		/**
		 * @brief : 取得したいマテリアルのID(UUID)を元にマテリアルを取得
		 * @param id : 取得したいマテリアルのID(UUID)
		 * @return : 取得したマテリアルの参照
		 */
		Material& GetMaterial(const mapping::rename_type::UUID& id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Material>> materials{};
		std::unique_ptr<Material> dummy_material{};

		mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
	};
} // cumulonimbus::asset
