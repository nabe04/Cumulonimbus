#include "scene_manager.h"

#include "loader.h"
#include "editor_manager.h"
#include "gaussian_blur.h"
#include "render_path.h"
#include "sky_box.h"

namespace cumulonimbus::scene
{
	SceneManager::SceneManager(const std::shared_ptr<Window>& window)
	{
		// インスタンス化
		framework		= std::make_unique<Framework>(window);
		editor_manager	= std::make_unique<editor::EditorManager>();
		system			= std::make_shared<system::System>();
		asset_manager	= std::make_shared<asset::AssetManager>();
		render_path		= std::make_unique<renderer::RenderPath>(locator::Locator::GetDx11Device()->device.Get());
		// システムのロード
		system->Load();
		// ロケータへの登録
		locator::Locator::Provide<system::System>(system);
		locator::Locator::Provide<asset::AssetManager>(asset_manager);

		collision_manager = std::make_unique<collision::CollisionManager>(*system.get());
		
		SetWindowLongPtr(window->GetHWND(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&framework));
		CreateNewScene();

		////-- 新規シーンの作成 --//
		//active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>());
		//active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>());

		// エディター側で操作するシーンのセット(最初に追加されたシーンをセットする)
		editor_manager->SetSelectedSceneId(active_scenes.begin()->first);
	}

	void SceneManager::Run()
	{
		Initialize();

		Execute();

		UnInitialize();
	}

	scene::Scene& SceneManager::CreateNewScene()
	{
		//-- 現在開かれているシーンの削除 --//
		if(active_scenes.size() > 0)
		{
			active_scenes.clear();
		}

		//-- 新規シーンの作成 --//
		const mapping::rename_type::UUID& new_scene_id = utility::GenerateUUID();
		active_scenes.emplace(new_scene_id, std::make_unique<scene::Scene>(this));
		// エディターマネージャ側の選択されているシーンIDも再設定
		editor_manager->SetSelectedSceneId(new_scene_id);
		return *active_scenes.begin()->second.get();
	}

	void SceneManager::AddScene()
	{
		// シーンの追加
		active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>(this));
	}

	void SceneManager::AddScene(
		const mapping::rename_type::UUID& scene_id,
		const std::filesystem::path& scene_file_path)
	{
		// 既に追加したいシーンを開いた場合処理を抜ける
		if (active_scenes.contains(scene_id))
			return;

		// シーンの追加
		active_scenes.emplace(scene_id, std::make_unique<scene::Scene>(this));
		active_scenes.at(scene_id)->LoadScene(scene_file_path.parent_path().string(),scene_file_path.filename().string());
	}

	void SceneManager::DeleteScene(const mapping::rename_type::UUID& scene_id)
	{
		// 削除したいシーンが存在しない場合処理を抜ける
		if (!active_scenes.contains(scene_id))
			return;

		// シーンの削除
		active_scenes.erase(scene_id);
	}

	void SceneManager::DeleteAllScene()
	{
		// シーンの全削除
		active_scenes.clear();
	}

	void SceneManager::Execute()
	{
		while(framework->ProcessLoop())
		{
			const float delta_time = std::min(0.03f, static_cast<float>(framework->GetHighResolutionTimer().GetDeltaTime()));

			// Update process
			Update(delta_time);

			framework->DrawBegin();
			Render();
			framework->DrawEnd();
		}
	}

	void SceneManager::Initialize()
	{
		InitialCreatePrefab();
		//CreateScene();
	}

	void SceneManager::UnInitialize()
	{
		active_scenes.clear();
		//framework->Un
	}

	void SceneManager::Update(const float dt)
	{// シーン毎の更新処理
		editor_manager->Update(dt);
		// ボタンが押された時の保存
		//if (editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
		//	ButtonState::Press)
		//{
		//	SaveScene(file_path_helper::GetSaveSceneViewFilePathAndName(), "test");
		//}
		//if (editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
		//	ButtonState::Release)
		//{
		//	LoadScene(file_path_helper::GetSaveSceneViewFilePathAndName(), "test" + file_path_helper::GetSceneExtension());
		//}

		for(auto& [scene_id, scene] : active_scenes)
		{
			scene->CommonUpdate(dt);
			scene->SceneUpdate(dt);
			if(editor_manager->GetToolBar().IsPlaybackState(editor::ToolBar::Button::Play))
			{
				scene->GameUpdate(dt);
			}
		}

		system->Update(dt);
	}

	void SceneManager::Render()
	{// シーン毎の描画処理
		auto* immediate_context = framework->GetDeviceContext();
		render_path->RenderScene(immediate_context, active_scenes, editor_manager->GetSceneView().GetSceneViewCamera().GetCamera());
		render_path->RenderGame(immediate_context, active_scenes);
#ifdef _DEBUG
		// Todo : 複数シーン読み込み対応後現在選択されているシーンがわかれば第1,2引数を変更する
		//editor_manager->RenderEditor(active_scenes.begin()->second.get(), active_scenes.begin()->second->GetRegistry());

		// 複数シーン対応版
		editor_manager->RenderEditor(active_scenes);
#endif // _DEBUG
	}

	void SceneManager::InitialCreatePrefab() const
	{
	}
} // cumulonimbus::scene
