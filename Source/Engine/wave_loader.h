#pragma once
#include "loader.h"
//#include "wave.h"

namespace cumulonimbus::asset
{
	struct WaveAsset
	{
		std::filesystem::path wave_file_path{}; // 「.wave」ファイルまでのパス
	};

	class WaveLoader final : public Loader
	{
	public:
		explicit WaveLoader()  = default;
		~WaveLoader() override = default;

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

		void CreateWave(scene::Scene& scene);
		void OpenWave(scene::Scene& scene, const std::filesystem::path& path);

	private:
		std::map<mapping::rename_type::UUID, WaveAsset> waves{};


		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;
	};
} // cumulonimbus::asset