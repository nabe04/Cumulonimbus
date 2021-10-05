#include "scene.h"

#include "asset_manager.h"
#include "asset_sheet_manager.h"
#include "loader.h"
#include "camera_component.h"
#include "geometric_primitive_resource.h"
#include "input_manager.h"
#include "input_manager.h"
#include "locator.h"
#include "render_path.h"
#include "resource_manager.h"
#include "window.h"
#include "file_path_helper.h"

#include "scene_game.h"

#include "ecs.h"
#include "light.h"
#include "camera_component.h"

// Components
#include "anim_sprite.h"
#include "mesh_object.h"
#include "transform_component.h"
#include "sky_box.h"

//Todo : 消す
#include "prefab_loader.h"

namespace cumulonimbus::scene
{
	template <class Archive>
	void Scene::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(collision_manager),
			CEREAL_NVP(light)
		);
	}


	cumulonimbus::scene::Scene::Scene()
	{
	}

	void Scene::Execute(Framework* framework)
	{
		this->framework = framework;

		Initialize();

		while(framework->ProcessLoop())
		{
			const float delta_time = std::min(0.03f, static_cast<float>(framework->GetHighResolutionTimer().GetDeltaTime()));

			// Update process
			Update(delta_time);

			framework->DrawBegin();
			Render();
			framework->DrawEnd();
		}

		UnInitialize();
	}

	void Scene::SaveScene(const std::string& file_dir, const std::string& scene_name)
	{
		registry->Save(file_dir, scene_name);

		// Scenesまでのパス/シーン名
		const std::string filename = file_dir + "/" + scene_name;
		{// 「.scene」形式(バイナリ)での保存
			std::ofstream ofs(filename + cumulonimbus::file_path_helper::GetSceneExtension());
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		{// 「.json」形式での保存

			std::ofstream ofs(filename + cumulonimbus::file_path_helper::GetJsonExtension());
			cereal::JSONOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void Scene::LoadScene(const std::string& file_dir, const std::string& scene_name)
	{

		std::ifstream ifs(file_dir + "/" + scene_name);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);

		light->Load();

		registry->Load(file_dir);
	}

	void Scene::Initialize()
	{
#ifdef _DEBUG
		if (!this->editor_manager)
		{
			editor_manager = std::make_unique<editor::EditorManager>();
		}
#endif

		light = std::make_unique<Light>(locator::Locator::GetDx11Device()->device.Get());
		light->SetLightDir(XMFLOAT3{ .0f,.0f,1.f, });
		registry    = std::make_unique<ecs::Registry>();
		registry->SetScene(this);
		render_path = std::make_unique<renderer::RenderPath>(locator::Locator::GetDx11Device()->device.Get());

		asset_manager = std::make_shared<asset::AssetManager>();
		locator::Locator::Provide<asset::AssetManager>(asset_manager);
		collision_manager = std::make_unique<collision::CollisionManager>();

		const mapping::rename_type::Entity ent_main_camera = registry->CreateEntity();
		registry->AddComponent<component::CameraComponent>(ent_main_camera, true);
		registry->GetComponent<component::CameraComponent>(ent_main_camera).GetCamera()->SetViewInfo({ 25,100,-300 }, { .0f, .0f, .0f }, XMFLOAT3(.0f, 1.0f, .0f));
		registry->GetComponent<component::CameraComponent>(ent_main_camera).GetCamera()->SetProjection(XM_PI / 8.0f, static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);

		const char* sky_filename = "./Data/Assets/cubemap/skybox";
		const mapping::rename_type::Entity ent_sky_box = registry->CreateEntity();
		registry->AddComponent<component::SkyBoxComponent>(ent_sky_box, locator::Locator::GetDx11Device()->device.Get(), sky_filename);
		registry->GetComponent<component::TransformComponent>(ent_sky_box).SetScale(3.f);

		const mapping::rename_type::Entity test_ent = registry->CreateEntity();
		registry->GetComponent<component::TransformComponent>(test_ent).SetPosition({ 10,100,-30 });
		registry->GetComponent<component::TransformComponent>(test_ent).SetScale({20,60,1000});

		asset::PrefabLoader* prefab_loader = asset_manager->GetLoader<asset::PrefabLoader>();
		prefab_loader->CreatePrefab(*asset_manager.get(), registry.get(), test_ent);
	}

	void Scene::UnInitialize()
	{
		registry->DestroyAllEntities();
	}

	void Scene::Update(const float dt)
	{
		// 共通の更新処理
		registry->PreCommonUpdate(dt);
		registry->CommonUpdate(dt);
		registry->PostCommonUpdate(dt);
		// Scene Viewの更新処理
		registry->PreSceneUpdate(dt);
		registry->SceneUpdate(dt);
		registry->PostSceneUpdate(dt);
		if(editor_manager->GetToolBar().IsPlaybackState(editor::ToolBar::Button::Play))
		{
			// Game Viewの更新処理
			registry->PreGameUpdate(dt);
			registry->GameUpdate(dt);
			registry->PostGameUpdate(dt);
		}

		collision_manager->Update(dt, registry.get());

		// light update
		for (const auto& camera_comp : registry->GetArray<cumulonimbus::component::CameraComponent>().GetComponents())
		{
			if (camera_comp.GetIsMainCamera())
				light->Update(&camera_comp);
		}

		editor_manager->Update(dt);
		if( editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
			ButtonState::Press)
		{
			SaveScene(file_path_helper::GetSaveSceneViewFilePathAndName(), "test");
		}
		if (editor_manager->GetToolBar().GetToolBarButton().GetButtonState(editor::ToolBar::Button::Play) ==
			ButtonState::Release)
		{
			LoadScene(file_path_helper::GetSaveSceneViewFilePathAndName(), "test" + file_path_helper::GetSceneExtension());
		}
	}

	void Scene::Render()
	{
		auto* immediate_context = framework->GetDeviceContext();

		render_path->RenderScene(immediate_context, registry.get(),
								 &editor_manager->GetSceneView().GetSceneViewCamera().GetCamera(),
								 light.get());
		render_path->RenderGame(immediate_context, registry.get(), light.get());

#ifdef _DEBUG
		editor_manager->RenderEditor(this, registry.get());
#endif // _DEBUG
	}

} // cumulonimbus::scene