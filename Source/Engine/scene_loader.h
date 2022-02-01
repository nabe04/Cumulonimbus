#pragma once
#include <map>
#include "loader.h"

namespace cumulonimbus::scene
{
	class Scene;
	class SceneManager;
} // cumulonimbus::scene

namespace cumulonimbus::asset
{
	struct SceneAsset
	{
		std::filesystem::path scene_file_path{}; // 「.scene」ファイルまでのパス
	};

	class SceneLoader final : public Loader
	{
	public:
		explicit SceneLoader()  = default;
		~SceneLoader() override = default;

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
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : アセットの名前変更
		 * @remark : 変更先のクラスでasset_idが存在しない場合処理を抜ける
		 * @remark : シーンのアセットは「.scene」と「.json」ファイルとシーン名の
		 *			 フォルダが存在するのでこれらのファイル名も名前を変更する
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : リネーム対象のアセットID
		 * @param changed_name : 変更後のファイル名(※ファイルパスや拡張子は含まない)
		 */
		void Rename(AssetManager& asset_manager,const mapping::rename_type::UUID& asset_id, const std::string& changed_name) override;

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
		 * @brief : 指定されてIDのシーンが存在するか
		 */
		[[nodiscard]]
		bool HasScene(const mapping::rename_type::UUID& scene_id) const;

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		bool Supported(std::filesystem::path extension) override;

		/**
		 * @brief : シーンの名前を付けての保存
		 * @remark : 保存時に保存するパスとシーンの確認を取る
		 * @param scene : 現在のシーン
		 * @param path : 「.scene」ファイルがある親のパス
		 *				  例) ./Data/Scenes/Sample/sample.scene -> ./Data/Scenes/Sample
		 */
		void SaveAs(scene::Scene& scene, const std::filesystem::path& path);

		// Todo : シーンマネージャーでのSave完了時不要になるかもしれない
		/**
		 * @brief : シーンの保存
		 * @remark : SaveAsとの違いはシーンが既に存在していていることを前提に
		 *			 保存を行うので、シーン保存時に名前を付けずscene_file_dirで
		 *			 指定されたパスを元の保存を行う
		 * @remark : ※注意 保存したいシーンファイルが存在しない場合例外処理が出る
		 * @param scene : 現在のシーン
		 * @param scene_file_dir : 「.scene」ファイルまでのパス
		 *							例) ./Data/Scenes/Sample/sample.scene -> ./Data/Scenes/Sample/sample.scene
		 *
		 */
		void Save(scene::Scene& scene, const std::filesystem::path& scene_file_dir);

		/**
		 * @brief : シーンの保存
		 * @remark : SaveAsとの違いはシーンが既に存在していていることを前提に
		 *			 保存を行うので、シーン保存時に名前を付けずscene_file_dirで
		 *			 指定されたパスを元の保存を行う
		 * @remark : ※注意 保存したいシーンファイルが存在しない場合例外処理が出る
		 * @param active_scenes : 現在開かれているシーン(SceneManager::active_scenes)
		 * @param save_scene_id : 保存したいシーンID
		 */
		void Save(
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes,
			const mapping::rename_type::UUID& save_scene_id);

		/**
		 * @brief : シーンの追加読み込み
		 * @param scene_manager : シーン管理クラス
		 * @param add_scene_id : 追加したいシーンのID
		 * @param add_scene_path : 追加で読み込むシーン(拡張子「.scene」)までのファイルパス
		 */
		void AddScene(
			scene::SceneManager& scene_manager,
			const mapping::rename_type::UUID& add_scene_id,
			const std::filesystem::path& add_scene_path);
		/**
		 * @brief : シーンの追加読み込み
		 * @param scene_manager : シーン管理クラス
		 * @param add_scene_id : 追加したいシーンのID
		 */
		void AddScene(
			scene::SceneManager& scene_manager,
			const mapping::rename_type::UUID& add_scene_id);
		/**
		 * @brief : シーンの追加読み込み
		 * @param scene_manager : シーン管理クラス
		 * @param add_scene_path : 追加で読み込むシーン(拡張子「.scene」)までのファイルパス
		 */
		void AddScene(
			scene::SceneManager& scene_manager,
			const std::filesystem::path& add_scene_path);

		void DestroyScene(
			scene::SceneManager& scene_manager,
			const mapping::rename_type::UUID& destroy_scene_id);

		void CreateScene(scene::SceneManager& scene_manager);

		/**
		 * @brief : ファイルからのシーン読み込み
		 * @param scene_manager : シーン管理クラス
		 * @param path : 「.scene」までのファイルパス
		 */
		void OpenScene(scene::SceneManager& scene_manager, const std::filesystem::path& path);

		void OpenScene(scene::Scene& scene, const std::filesystem::path& path);

		[[nodiscard]]
		std::string Name(const mapping::rename_type::UUID& scene_id) const;

		/**
		 * @brief : ImGui上でのシーン選択関数
		 * @remark : ※caution(1) : ImGuiを使用する関数内で使用すること
		 * @remark : ※caution(2) : ImGui::Begin()の中でこの関数を呼ぶこと
		 * @param asset_manager :
		 * @param scene_id : 格納されるシーンID(UUID)
		 */
		bool ImSelectableScene(AssetManager& asset_manager, mapping::rename_type::UUID& scene_id);

	private:
		std::map<mapping::rename_type::UUID, SceneAsset> scenes{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : シーンのフォルダと対応するscenesのIDの削除
		 * @param scene_id : シーンID(UUID)
		 * @param delete_path : 「.scene」までのファイルパス
		 */
		void DeleteScene(
			const mapping::rename_type::UUID& scene_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset
