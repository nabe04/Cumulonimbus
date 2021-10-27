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
		// �C���X�^���X��
		framework		= std::make_unique<Framework>(window);
		editor_manager	= std::make_unique<editor::EditorManager>();
		system			= std::make_shared<system::System>();
		asset_manager	= std::make_shared<asset::AssetManager>();
		render_path		= std::make_unique<renderer::RenderPath>(locator::Locator::GetDx11Device()->device.Get());
		// �V�X�e���̃��[�h
		system->Load();
		// ���P�[�^�ւ̓o�^
		locator::Locator::Provide<system::System>(system);
		locator::Locator::Provide<asset::AssetManager>(asset_manager);

		SetWindowLongPtr(window->GetHWND(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&framework));
		CreateNewScene();

		//-- �V�K�V�[���̍쐬 --//
		active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>());
		active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>());

		// �G�f�B�^�[���ő��삷��V�[���̃Z�b�g(�ŏ��ɒǉ����ꂽ�V�[�����Z�b�g����)
		editor_manager->SetSelectedSceneId(active_scenes.begin()->first);
	}

	void SceneManager::Run()
	{
		Initialize();

		Execute();

		UnInitialize();
	}

	void SceneManager::CreateNewScene()
	{
		//-- ���݊J����Ă���V�[���̍폜 --//
		if(active_scenes.size() > 0)
		{
			active_scenes.clear();
		}

		//-- �V�K�V�[���̍쐬 --//
		active_scenes.emplace(utility::GenerateUUID(), std::make_unique<scene::Scene>());
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
	{// �V�[�����̍X�V����
		editor_manager->Update(dt);
		// �{�^���������ꂽ���̕ۑ�
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
	{// �V�[�����̕`�揈��
		auto* immediate_context = framework->GetDeviceContext();
		render_path->RenderScene(immediate_context, active_scenes, editor_manager->GetSceneView().GetSceneViewCamera().GetCamera());
		render_path->RenderGame(immediate_context, active_scenes);
#ifdef _DEBUG
		// Todo : �����V�[���ǂݍ��ݑΉ��㌻�ݑI������Ă���V�[�����킩��Α�1,2������ύX����
		//editor_manager->RenderEditor(active_scenes.begin()->second.get(), active_scenes.begin()->second->GetRegistry());

		// �����V�[���Ή���
		editor_manager->RenderEditor(active_scenes);
#endif // _DEBUG
	}

	void SceneManager::InitialCreatePrefab() const
	{
	}
} // cumulonimbus::scene
