#include "scene_manager.h"

#include <ranges>

#include "editor_manager.h"
#include "filename_helper.h"
#include "gaussian_blur.h"
#include "loader.h"
#include "prefab_loader.h"
#include "render_path.h"
#include "sky_box.h"
#include "wave_system.h"
// components
#include "player_component.h"
#include "enemy_soldier_component.h"

namespace cumulonimbus {
	namespace asset {
		struct SceneAsset;
	}
}

CEREAL_CLASS_VERSION(cumulonimbus::scene::SceneManager, 0)

namespace
{
	using namespace cumulonimbus;
	// �uPlay�v�{�^���������ꂽ���̃V�[���ۑ��p�X ./Data/ProjectSetting/TemporarySave/
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
		// �C���X�^���X��
		framework		= std::make_unique<Framework>(window);
		editor_manager	= std::make_unique<editor::EditorManager>();
		system			= std::make_shared<system::System>();
		wave_system		= std::make_unique<system::WaveSystem>();
		asset_manager	= std::make_shared<asset::AssetManager>();
		render_path		= std::make_unique<renderer::RenderPath>(locator::Locator::GetDx11Device()->device.Get());
		// �V�X�e���̃��[�h
		system->Load();
		// �E�F�[�u�V�X�e���̃��[�h
		wave_system->Load();
		// �V�X�e���ւ̊֐��o�^
		const auto& wave_hash = utility::GetHash<system::WaveSystem>();
		system->RegisterRenderFunction(wave_hash, [&](ecs::Registry* registry) {wave_system->RenderImGui(registry); });
		// ���P�[�^�ւ̓o�^
		locator::Locator::Provide<system::System>(system);
		locator::Locator::Provide<asset::AssetManager>(asset_manager);

		collision_manager = std::make_unique<collision::CollisionManager>(*system.get());

		SetWindowLongPtr(window->GetHWND(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&framework));
		CreateNewScene();

		////-- �V�K�V�[���̍쐬 --//
		//active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>());
		//active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>());

		// �G�f�B�^�[���ő��삷��V�[���̃Z�b�g(�ŏ��ɒǉ����ꂽ�V�[�����Z�b�g����)
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
		//-- ���݊J����Ă���V�[���̍폜 --//
		if(active_scenes.size() > 0)
		{
			active_scenes.clear();
		}

		//-- �V�K�V�[���̍쐬 --//
		const mapping::rename_type::UUID& new_scene_id = utility::GenerateUUID();
		active_scenes.emplace(new_scene_id, std::make_unique<scene::Scene>(this));
		// �G�f�B�^�[�}�l�[�W�����̑I������Ă���V�[��ID���Đݒ�
		editor_manager->SetSelectedSceneId(new_scene_id);
		return *active_scenes.begin()->second.get();
	}

	void SceneManager::OpenScene(
		const mapping::rename_type::UUID& scene_id,
		const std::filesystem::path& scene_file_path)
	{
		DeleteAllScene();
		active_scenes.emplace(scene_id, std::make_unique<scene::Scene>(this));
		// �V�[���ǂݍ��ݏ���
		active_scenes.begin()->second->LoadScene(scene_file_path.parent_path().string(), scene_file_path.filename().string());
		// �G�f�B�^�[�}�l�[�W�����̑I������Ă���V�[��ID���Đݒ�
		editor_manager->SetSelectedSceneId(scene_id);
	}

	void SceneManager::AddScene()
	{
		// �V�[���̒ǉ�
		active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>(this));
	}

	void SceneManager::AddScene(
		const mapping::rename_type::UUID& scene_id,
		const std::filesystem::path& scene_file_path)
	{
		// ���ɒǉ��������V�[�����J�����ꍇ�����𔲂���
		if (active_scenes.contains(scene_id))
			return;

		// �V�[���̒ǉ�
		active_scenes.emplace(scene_id, std::make_unique<scene::Scene>(this));
		active_scenes.at(scene_id)->LoadScene(scene_file_path.parent_path().string(),scene_file_path.filename().string());
	}

	void SceneManager::DeleteScene(const mapping::rename_type::UUID& scene_id)
	{
		// �폜�������V�[�������݂��Ȃ��ꍇ�����𔲂���
		if (!active_scenes.contains(scene_id))
			return;

		// �V�[���̍폜
		active_scenes.erase(scene_id);
	}

	void SceneManager::DeleteAllScene()
	{
		// �V�[���̑S�폜
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
	}

	void SceneManager::UnInitialize()
	{
		active_scenes.clear();
		system->Save();
		wave_system->Save();
	}

	void SceneManager::Update(const float dt)
	{// �V�[�����̍X�V����
		editor_manager->Update(dt);
		// �{�^���������ꂽ���̕ۑ�
		if (editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
			ButtonState::Press)
		{
			BeginGame();
			wave_system->Start(*this);
		}
		if (editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
			ButtonState::Release)
		{
			EndGame();
			wave_system->End(*this);
		}

		for(auto& [scene_id, scene] : active_scenes)
		{
			scene->CommonUpdate(dt);
			scene->SceneUpdate(dt);
			if(editor_manager->GetToolBar().IsPlaybackState(editor::ToolBar::Button::Play))
			{
				scene->GameUpdate(dt);
				wave_system->Update(*this);
			}
		}

		if (editor_manager->GetToolBar().IsPlaybackState(editor::ToolBar::Button::Play))
		{// �Q�[���v���C���̂ݔ��菈�����s��
			collision_manager->Update(dt, *this);
		}

		system->Update(dt);

		// �G���e�B�e�B�̍폜����
		for (auto& [scene_id, scene] : active_scenes)
		{
			scene->GetRegistry()->DestroyEntity();
		}
	}

	void SceneManager::Render()
	{// �V�[�����̕`�揈��
		auto* immediate_context = framework->GetDeviceContext();
		render_path->RenderScene(immediate_context, active_scenes, editor_manager->GetSceneView().GetSceneViewCamera().GetCamera());
		render_path->RenderGame(immediate_context, active_scenes);
#ifdef _DEBUG
		// �����V�[���Ή���
		editor_manager->RenderEditor(active_scenes);
#endif // _DEBUG
	}

	void SceneManager::BeginGame()
	{
		for(auto& scene : active_scenes | std::views::values)
		{
			std::string scene_name = scene->GetSceneName();
			if (scene_name.empty())
				scene_name = filename_helper::GetNoTitled();
			scene->SaveScene(save_path + "/" + scene_name, scene->GetSceneName());
			scene->StartGame();
		}
	}

	void SceneManager::EndGame()
	{
		for(auto& scene : active_scenes | std::views::values)
		{
			std::string scene_name = scene->GetSceneName();
			if (scene_name.empty())
				scene_name = filename_helper::GetNoTitled();
			scene->LoadScene(save_path + "/" + scene_name, scene->GetSceneName() + file_path_helper::GetSceneExtension());
		}
	}

	void SceneManager::InitialCreatePrefab()
	{
		ecs::Registry& registry = *active_scenes.begin()->second->GetRegistry();
		//-- �v���C���[�̊�ƂȂ�v���n�u�̍쐬 --//
		const mapping::rename_type::Entity& player_base = registry.CreateEntity();
		registry.AddComponent<component::PlayerComponent>(player_base);
		// �v���n�u�̍쐬
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager.get(), &registry,
																	  std::vector<mapping::rename_type::Entity>{player_base},
																	  true, "Player_Base");

		//-- �G�̊�ƂȂ�v���n�u�̍쐬 --//
		const mapping::rename_type::Entity& enemy_soldier_base = registry.CreateEntity();
		registry.AddComponent<component::EnemySoldierComponent>(enemy_soldier_base);
		// �v���n�u�̍쐬
		asset_manager->GetLoader<asset::PrefabLoader>()->CreatePrefab(*asset_manager.get(), &registry,
																	  std::vector<mapping::rename_type::Entity>{enemy_soldier_base},
																	  true, "E_Soldier_Base");

		// �쐬�����G���e�B�e�B�̍폜(�v���n�u�ւ̓o�^�݂̂��ړI�̈�)
		registry.Destroy(player_base);
		registry.Destroy(enemy_soldier_base);
		// �J������
		if(!registry.GetArray<component::CameraComponent>().GetComponents().empty())
		{
			auto& camera_comp = *registry.GetArray<component::CameraComponent>().GetComponents().begin();
			camera_comp.SwitchMainCamera();
		}
	}
} // cumulonimbus::scene
