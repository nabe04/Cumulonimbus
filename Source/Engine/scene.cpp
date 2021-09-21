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
#include "sprite_object.h"
#include "transform_component.h"
#include "sky_box.h"

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

	}

	void Scene::UnInitialize()
	{
		registry->DestroyAllEntities();
	}

	void Scene::Update(const float dt)
	{
		registry->PreUpdate(dt);
		registry->Update(dt);
		registry->PostUpdate(dt);

		collision_manager->Update(dt, registry.get());

		// light update
		for (const auto& camera_comp : registry->GetArray<cumulonimbus::component::CameraComponent>().GetComponents())
		{
			if (camera_comp.GetIsMainCamera())
				light->Update(&camera_comp);
		}

		editor_manager->Update(dt);
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


//// Todo : 新Scene システムができしたい消す
////*************************************
////
////	Initialization process
////	(Apply to all scenes)
////
////*************************************
//template <class Archive>
//void Scene::serialize(Archive&& archive)
//{
//	archive(
//		CEREAL_NVP(scene_name),
//		CEREAL_NVP(collision_manager),
//		CEREAL_NVP(light)
//	);
//}
//
//const std::shared_ptr<Scene>& Scene::Execute(Framework* framework)
//{
//	if (!this->framework)
//	{
//		this->framework = framework;
//	}
//
//	Initialize();
//
//	while (framework->ProcessLoop())
//	{
//		const float delta_time = std::min(0.03f, static_cast<float>(framework->GetHighResolutionTimer().GetDeltaTime()));
//
//		// Update process
//		Update(delta_time);
//
//		framework->DrawBegin();
//		Render();
//		framework->DrawEnd();
//
//		if (next_scene)
//		{
//			break;
//		}
//	}
//
//	UnInitialize();
//
//	return next_scene;
//}
//
//void Scene::Initialize()
//{
//	next_scene = nullptr;
//
//	// Create Light
//	if (!this->light)
//	{
//		light = std::make_unique<Light>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());
//		light->SetLightDir(XMFLOAT3{ .0f,.0f,1.f, });
//	}
//
//	if(!this->registry)
//	{
//		registry = std::make_unique<cumulonimbus::ecs::Registry>();
//	}
//
//	if(!this->render_path)
//	{
//		render_path = std::make_unique<cumulonimbus::renderer::RenderPath>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());
//	}
//
//	if (!this->geom_prim_res)
//	{// Create Geometric Primitive Data
//		geom_prim_res = std::make_unique<GeometricPrimitiveResource>(GetFramework()->GetDevice());
//	}
//
//	if (!this->collision_manager)
//	{
//		collision_manager = std::make_unique<cumulonimbus::collision::CollisionManager>();
//	}
//
//	if (!this->sound_resource)
//	{// Sound
//		sound_resource = std::make_unique<SoundResource>(GetFramework()->GetWindow()->GetHWND());
//	}
//#ifdef _DEBUG
//	if (!this->editor_manager)
//	{
//		editor_manager = std::make_unique<cumulonimbus::editor::EditorManager>();
//	}
//#endif
//
//	if (!this->texture_resource_manager)
//	{
//		using namespace cumulonimbus;
//		texture_resource_manager = std::make_shared<manager::texture::TextureResourceManager>(locator::Locator::GetDx11Device()->device.Get());
//		locator::Locator::Provide<manager::texture::TextureResourceManager>(texture_resource_manager);
//	}
//
//	if(!this->resource_manager)
//	{
//		resource_manager = std::make_shared<ResourceManager>();
//		resource_manager->Initialize(GetFramework()->GetDevice());
//		cumulonimbus::locator::Locator::Provide<ResourceManager>(resource_manager);
//	}
//
//	if(!this->asset_manager)
//	{
//		asset_manager = std::make_shared<cumulonimbus::asset::AssetManager>();
//		cumulonimbus::locator::Locator::Provide<cumulonimbus::asset::AssetManager>(asset_manager);
//	}
//
//	//pad_combine = std::make_unique<pad_link::PadLink>();
//	//{
//	//	pad_combine->Register(InputType::GamePad::Up_DPad, InputType::Key::KEY_W);
//	//	pad_combine->Register(InputType::GamePad::Left_DPad, InputType::Key::KEY_A);
//	//	pad_combine->Register(InputType::GamePad::Down_DPad, InputType::Key::KEY_S);
//	//	pad_combine->Register(InputType::GamePad::Right_DPad, InputType::Key::KEY_D);
//	//	pad_combine->Register(InputType::GamePad::Right_Shoulder, InputType::Mouse::RIGHT);
//	//	pad_combine->Register(InputType::GamePad::Pad_A, InputType::Key::KEY_SPACE);
//	//	//pad_combine->Register(InputType::GamePad::Pad_X, InputType::Key::KEY_F);
//	//	pad_combine->Register(InputType::GamePad::Pad_X, InputType::Mouse::LEFT);
//	//	pad_combine->Register(InputType::GamePad::Left_Shoulder, InputType::Key::KEY_R);
//	//	pad_combine->Register(InputType::GamePad::Start, InputType::Key::KEY_BACK);
//	//}
//
//	InitializeScene();
//}
//
//void Scene::UnInitialize()
//{
//	registry->DestroyAllEntities();
//	UnInitializeScene();
//}
//
//void Scene::Update(const float elapsed_time)
//{
//	//if(!is_paused)
//
//	registry->PreUpdate(elapsed_time);
//	registry->Update(elapsed_time);
//	registry->PostUpdate(elapsed_time);
//
//	collision_manager->Update(elapsed_time, registry.get());
//
//	//main_camera->SetProjection(XM_PI / 8.0f, static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);
//
//	// Camera update
//	//main_camera->Update(elapsed_time);
//
//	// light update
//	for(const auto& camera_comp : registry->GetArray<cumulonimbus::component::CameraComponent>().GetComponents())
//	{
//		if(camera_comp.GetIsMainCamera())
//			light->Update(&camera_comp);
//	}
//
//	UpdateScene(elapsed_time);
//
//	if (cumulonimbus::locator::Locator::GetInput()->Keyboard().GetState(Keycode::A) == ButtonState::Press)
//	{
//		//this->SaveScene(this->scene_name);
//	}
//
//	if (cumulonimbus::locator::Locator::GetInput()->Keyboard().GetState(Keycode::L) == ButtonState::Press)
//	{
//		//this->LoadScene("./Content/Scene/Scene_Title");
//	}
//}
//
//void Scene::Render()
//{
//	auto* immediate_context = framework->GetDeviceContext();
//
//	render_path->Render(immediate_context, registry.get(), light.get());
//
//#ifdef _DEBUG
//	// ImGui
//	{
//		const RECT window_rect = cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect();
//
//		ImGui::Begin("Scene");
//		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//		this->RenderImGui();
//		light->WriteImGui();
//		if (ImGui::CollapsingHeader("Objects"))
//		{
//			registry->RenderImGui();
//		}
//		ImGui::Text("window left    : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().left);
//		ImGui::Text("window right   : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().right);
//		ImGui::Text("window top     : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().top);
//		ImGui::Text("window bottom  : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().bottom);
//
//		ImGui::Text("client left    : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().left);
//		ImGui::Text("client right   : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().right);
//		ImGui::Text("client top     : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().top);
//		ImGui::Text("client bottom  : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().bottom);
//
//		POINT cursor_pos{};
//		GetCursorPos(&cursor_pos);
//		ImGui::Text("mouse x : %d", cursor_pos.x);
//		ImGui::Text("mouse y : %d", cursor_pos.y);
//
//		ImGui::End();
//
//		//editor_manager->RenderEditor(this, registry.get());
//	}
//#endif // _DEBUG
//}
//
//void Scene::SaveScene(const std::string& file_dir, const std::string& scene_name)
//{
//	registry->Save(file_dir, scene_name);
//
//	// Scenesまでのパス/シーン名
//	const std::string filename = file_dir + "/" + scene_name;
//	{// 「.scene」形式(バイナリ)での保存
//		std::ofstream ofs(filename + cumulonimbus::file_path_helper::GetSceneExtension());
//		cereal::BinaryOutputArchive output_archive(ofs);
//		output_archive(*this);
//	}
//	{// 「.json」形式での保存
//
//		std::ofstream ofs(filename + cumulonimbus::file_path_helper::GetJsonExtension());
//		cereal::JSONOutputArchive output_archive(ofs);
//		output_archive(*this);
//	}
//}
//
//void Scene::LoadScene(const std::string& file_dir, const std::string& scene_name)
//{
//	std::ifstream ifs(file_dir + "/" + scene_name);
//	cereal::BinaryInputArchive i_archive(ifs);
//	i_archive(*this);
//
//	light->Load();
//
//	registry->Load(file_dir);
//}
//
//
//void Scene::WriteImGui() const
//{
//	if (ImGui::CollapsingHeader("Light Info"))
//	{
//		//light->WriteImGui();
//
//		//ImGui::DragFloat("Direction X", &light->GetLightDir().x, 0.01f, -1.0f, 1.0f);
//		//ImGui::DragFloat("Direction Y", &light->GetLightDir().y, 0.01f, -1.0f, 1.0f);
//		//ImGui::DragFloat("Direction Z", &light->GetLightDir().z, 0.01f, -1.0f, 1.0f);
//	}
//}
//
////-------------------------------------
////
//// SceneManager Class
////
////-------------------------------------
//void SceneManager::Execute(Framework* framework)
//{
//	//Scene* scene = new SceneTitle();
//	std::shared_ptr<Scene> scene = std::make_shared<SceneGame>();
//
//	while (scene)
//	{
//		scene = scene->Execute(framework);
//	}
//}
