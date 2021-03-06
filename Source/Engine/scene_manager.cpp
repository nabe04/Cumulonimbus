#include "scene_manager.h"

#include <ranges>

#include "editor_manager.h"
#include "effekseer_manager.h"
#include "filename_helper.h"
#include "gaussian_blur.h"
#include "loader.h"
#include "prefab_loader.h"
#include "render_path.h"
#include "sky_box.h"
#include "wave_system.h"
#include "time_scale.h"
#include "post_effect_manager.h"
// components
#include "player_component.h"
#include "player_sword_component.h"
#include "enemy_soldier_component.h"

namespace cumulonimbus
{
	namespace asset
	{
		struct SceneAsset;
	} // asset
} // cumulonimbus

CEREAL_CLASS_VERSION(cumulonimbus::scene::SceneManager, 0)

namespace
{
	using namespace cumulonimbus;
	// 「Play」ボタンが押された時のシーン保存パス ./Data/ProjectSetting/TemporarySave/
	const std::string save_path{ "./" + filename_helper::GetData() + "/" + filename_helper::GetProjectSetting() + "/" + filename_helper::GetTemporarySave() + "/"};
}

namespace cumulonimbus::scene
{
	template <class Archive>
	void SceneManager::load(Archive&& archive, uint32_t version)
	{
		if(version == 0)
		{
			archive(
				CEREAL_NVP(active_scenes)
			);
		}
	}

	template <class Archive>
	void SceneManager::save(Archive&& archive, uint32_t version) const
	{
		if (version == 0)
		{
			archive(
				CEREAL_NVP(active_scenes)
			);
		}
	}

	SceneManager::SceneManager(const std::shared_ptr<Window>& window)
	{
		// インスタンス化
		framework			= std::make_unique<Framework>(window);
		editor_manager		= std::make_unique<editor::EditorManager>();
		wave_system			= std::make_unique<system::WaveSystem>();
		render_path			= std::make_unique<renderer::RenderPath>(locator::Locator::GetDx11Device()->device.Get());
		effekseer_manager	= std::make_shared<system::EffekseerManager>(locator::Locator::GetDx11Device()->device.Get(), locator::Locator::GetDx11Device()->immediate_context.Get());
		locator::Locator::Provide<system::EffekseerManager>(effekseer_manager);
		system				= std::make_shared<system::System>();
		locator::Locator::Provide<system::System>(system);
		asset_manager		= std::make_shared<asset::AssetManager>();
		locator::Locator::Provide<asset::AssetManager>(asset_manager);
		// システムのロード
		system->Load();
		// ウェーブシステムのロード
		wave_system->Load();
		// システムへの関数登録
		const auto& wave_hash = utility::GetHash<system::WaveSystem>();
		system->RegisterRenderFunction(wave_hash, [&](ecs::Registry* registry) {wave_system->RenderImGui(registry); });


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

	void SceneManager::OpenScene(
		const mapping::rename_type::UUID& scene_id,
		const std::filesystem::path& scene_file_path)
	{
		DeleteAllScene();
		active_scenes.emplace(scene_id, std::make_unique<scene::Scene>(this));
		// シーン読み込み処理
		active_scenes.begin()->second->LoadScene(scene_file_path.parent_path().string(), scene_file_path.filename().string());
		// エディターマネージャ側の選択されているシーンIDも再設定
		editor_manager->SetSelectedSceneId(scene_id);
	}

	void SceneManager::SwitchScene(const mapping::rename_type::UUID& scene_id, const std::filesystem::path& scene_file_path)
	{
		next_scene_id		 = scene_id;
		next_scene_file_path = scene_file_path;
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

	bool SceneManager::IsInGame() const
	{
		return editor_manager->GetToolBar().IsPlayGame();
	}

	void SceneManager::Execute()
	{
		while(framework->ProcessLoop())
		{
			const float delta_time = std::min(0.03f, static_cast<float>(framework->GetHighResolutionTimer().GetDeltaTime()));
			system->GetTime().SetUnscaledDeltaTime(delta_time);

			// Update process
			Update(delta_time * system->GetTime().GetScale());

			framework->DrawBegin();
			Render();
			framework->DrawEnd();
		}
	}

	void SceneManager::Initialize()
	{
		InitialCreatePrefab();
	}

	void SceneManager::UnInitialize()
	{
		active_scenes.clear();
		system->Save();
		wave_system->Save();
	}

	void SceneManager::Update(const float dt)
	{// シーン毎の更新処理
		editor_manager->Update(dt);
		effekseer_manager->Update(dt);
		// ボタンが押された時の保存
		if (editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
			ButtonState::Press)
		{
			BeginGame();
			wave_system->Start(*this);
		}
		if (editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
			ButtonState::Release)
		{
			wave_system->End(*this);
			EndGame();
		}

		for(auto& [scene_id, scene] : active_scenes)
		{
			scene->CommonUpdate(dt);
			scene->SceneUpdate(dt);
			if(editor_manager->GetToolBar().IsPlaybackState(editor::ToolBar::Button::Play))
			{
				scene->GameUpdate(dt);
			}
		}

		if (editor_manager->GetToolBar().IsPlaybackState(editor::ToolBar::Button::Play))
		{// ゲームプレイ時のみ判定処理を行う
			collision_manager->Update(dt, *this);
			wave_system->Update(*this);
		}

		system->Update(dt);

		// エンティティの削除処理
		for (auto& [scene_id, scene] : active_scenes)
		{
			scene->GetRegistry()->DestroyEntity();
		}

		// 新規シーンの読み込み
		if (!next_scene_id.empty() &&
			!next_scene_file_path.empty())
		{
			OpenScene(next_scene_id, next_scene_file_path);
			active_scenes.at(next_scene_id)->StartGame();

			next_scene_id.clear();
			next_scene_file_path.clear();
		}
	}

	void SceneManager::Render()
	{// シーン毎の描画処理
		auto* immediate_context = framework->GetDeviceContext();
		render_path->RenderScene(immediate_context, active_scenes, editor_manager->GetSceneView().GetSceneViewCamera().GetCamera());
		render_path->RenderGame(immediate_context, active_scenes);
#ifdef _DEBUG
		// 複数シーン対応版
		editor_manager->RenderEditor(active_scenes);
#endif // _DEBUG
	}

	void SceneManager::BeginGame()
	{
		// 「Play」ボタンを押した時のファイルパス & 名前のクリア
		begin_scene_datum.clear();

		for(auto&[scene_id, scene]: active_scenes)
		{
			std::string scene_name = scene->GetSceneName();
			if (scene_name.empty())
				scene_name = filename_helper::GetNoTitled();
			begin_scene_datum.emplace_back();
			begin_scene_datum.back().scene_id		 = scene_id;
			begin_scene_datum.back().save_scene_path = save_path + "/" + scene_name;
			begin_scene_datum.back().save_scene_name = scene->GetSceneName();
			scene->SaveScene(begin_scene_datum.back().save_scene_path, begin_scene_datum.back().save_scene_name);
			scene->StartGame();
		}
	}

	void SceneManager::EndGame()
	{
		for(auto& scene : active_scenes | std::views::values)
		{
			scene->EndGame();

			//std::string scene_name = scene->GetSceneName();
			//if (scene_name.empty())
			//	scene_name = filename_helper::GetNoTitled();
			//scene->LoadScene(save_path + "/" + scene_name, scene->GetSceneName() + file_path_helper::GetSceneExtension());
		}

		DeleteAllScene();

		for (const auto& begin_scene_data : begin_scene_datum)
		{
			active_scenes.emplace(begin_scene_data.scene_id, std::make_unique<scene::Scene>(this));
			// シーン読み込み処理
			active_scenes.begin()->second->LoadScene(begin_scene_data.save_scene_path, begin_scene_data.save_scene_name + file_path_helper::GetSceneExtension());
			// エディターマネージャ側の選択されているシーンIDも再設定
			editor_manager->SetSelectedSceneId(begin_scene_data.scene_id);
			//
			//scene->LoadScene(begin_save_scene_paths.at(i), begin_save_scene_names.at(i) + file_path_helper::GetSceneExtension());
		}
	}

	void SceneManager::InitialCreatePrefab()
	{
		ecs::Registry& registry = *active_scenes.begin()->second->GetRegistry();
		// プレイヤーの基となるプレハブの作成
		const mapping::rename_type::Entity& player_base = registry.CreateEntity();
		registry.AddComponent<component::PlayerComponent>(player_base);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager.get(), &registry,
																	  std::vector<mapping::rename_type::Entity>{player_base},
																	  true, "Player_Base");
		// 武器(プレイヤー)の基となるプレハブの作成
		const mapping::rename_type::Entity& player_sword_base = registry.CreateEntity();
		registry.AddComponent<component::PlayerSwordComponent>(player_sword_base);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager.get(),&registry,
																	   std::vector<mapping::rename_type::Entity>{player_sword_base},
																	   true, "Player_Sword_Base");

		// 敵の基となるプレハブの作成
		const mapping::rename_type::Entity& enemy_soldier_base = registry.CreateEntity();
		registry.AddComponent<component::EnemySoldierComponent>(enemy_soldier_base);
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager.get(), &registry,
																	  std::vector<mapping::rename_type::Entity>{enemy_soldier_base},
																	  true, "E_Soldier_Base");

		// 作成したエンティティの削除(プレハブへの登録のみが目的の為)
		registry.Destroy(player_base);
		registry.Destroy(player_sword_base);
		registry.Destroy(enemy_soldier_base);
		// カメラを
		if(!registry.GetArray<component::CameraComponent>().GetComponents().empty())
		{
			auto& camera_comp = *registry.GetArray<component::CameraComponent>().GetComponents().begin();
			camera_comp.SwitchMainCamera();
		}
	}
} // cumulonimbus::scene
