#pragma once
#include <unordered_map>

#include "scene.h"
#include "rename_type_mapping.h"

class Framework;

namespace cumulonimbus
{
	namespace asset
	{
		class AssetManager;
	} // asset

	namespace renderer
	{
		class RenderPath;
	} // renderer

	namespace collision
	{
		class CollisionManager;
	} // collision
} // cumulonimbus

namespace cumulonimbus::scene
{
	class SceneManager final
	{
	public:
		explicit SceneManager(const std::shared_ptr<Window>& window);
		~SceneManager() = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);
		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Run();

		/**
		 * @brief : 新規シーンの作成
		 * @remark : 現在開かれているシーンを全て削除して
		 *			 新しいシーンを作成する
		 */
		scene::Scene& CreateNewScene();

		/**
		 * @brief : 既存シーンの読み込み
		 * @remark : 現在開かれているシーンを全て削除して
		 *			既存シーンの読み込む
		 * @param scene_id : 読み込みたいシーンID
		 * @param scene_file_path : 読み込むシーン(拡張子「.scene」)までのファイルパス
		 */
		void OpenScene(const mapping::rename_type::UUID & scene_id, const std::filesystem::path& scene_file_path);

		/**
		 * @brief : シーンの追加読み込み
		 * @remark : 現在開かれているシーンに追加して
		 *			 新しいシーンを作成する
		 * @remark : 初期シーン名は「NoTitled」
		 */
		void AddScene();

		/**
		 * @brief : シーンの追加読み込み
		 * @remark : 現在開かれているシーンに追加して
		 *			 新しいシーンを作成する
		 * @remark : 既存のシーンを読み込んで追加する
		 * @param scene_id : 追加するシーンのID
		 * @param scene_file_path : 読み込むシーン(拡張子「.scene」)までのファイルパス
		 */
		void AddScene(const mapping::rename_type::UUID& scene_id, const std::filesystem::path& scene_file_path);

		/**
		 * @brief : シーンの削除
		 * @remark : この関数では削除時に「保存するか」などの確認を行わないため注意
		 * @remark : 削除するIDが存在しない場合早期リターンする(例外処理などは出さない)
		 * @param scene_id : 削除するシーンID
		 */
		void DeleteScene(const mapping::rename_type::UUID& scene_id);

		/**
		 * @brief : 全シーンの削除
		 */
		void DeleteAllScene();

		//-- ゲッター --//
		[[nodiscard]]
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>>* GetActiveScenes()
		{
			return &active_scenes;
		}
		[[nodiscard]]
		Scene* GetScene(const mapping::rename_type::UUID& scene_id)
		{
			return active_scenes.at(scene_id).get();
		}
		[[nodiscard]]
		Framework* GetFramework() const
		{
			return framework.get();
		}
		[[nodiscard]]
		editor::EditorManager* GetEditorManager() const
		{
			return editor_manager.get();
		}
		[[nodiscard]]
		collision::CollisionManager* GetCollisionManager() const
		{
			return collision_manager.get();
		}

	private:
		// 現在開かれているシーン
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>> active_scenes{};
		// フレームワーク
		std::unique_ptr<Framework>			  framework{};
		// 描画処理を担う
		std::unique_ptr<renderer::RenderPath> render_path{};
		// エディター全体で統一の設定
		std::shared_ptr<system::System>		  system{};

		//-- Manager's --//
		// アセットシート管理用マネジャー
		std::shared_ptr<asset::AssetManager>	asset_manager{};
		// エディター(GUI)管理用マネジャー
		std::unique_ptr<editor::EditorManager>	editor_manager{};
		// 当たり判定管理用マネジャー
		std::unique_ptr<collision::CollisionManager> collision_manager{};

		void Execute();
		void Initialize();
		void UnInitialize();
		void Update(float dt);
		void Render();

		/**
		 * @brief : ゲーム開始時処理
		 * @remark : 「Play」ボタンが押された時の処理
		 */
		void BeginGame();
		/**
		 * @brief : ゲーム終了時処理
		 * @remark : 「Play」ボタンが離された時の処理
		 */
		void EndGame();

		// Todo : スクリプト機能が出来れば消す
		/**
		 * @brief : プレファブの初期作成
		 */
		void InitialCreatePrefab();
	};
} // cumulonimbus::scene