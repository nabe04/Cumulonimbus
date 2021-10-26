#include "scene_manager.h"

#include "loader.h"
#include "editor_manager.h"
#include "gaussian_blur.h"
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

	}

	void SceneManager::Run()
	{
		Initialize();

		Execute();

		UnInitialize();
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
	}

	void SceneManager::Update(float dt)
	{// �V�[�����̍X�V����

	}

	void SceneManager::Render()
	{// �V�[�����̕`�揈��

	}

	void SceneManager::InitialCreatePrefab() const
	{
	}
} // cumulonimbus::scene
