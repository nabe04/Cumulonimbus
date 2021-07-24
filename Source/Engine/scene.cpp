#include "scene.h"

#include "geometric_primitive_resource.h"
#include "input_manager.h"
#include "input_manager.h"
#include "locator.h"
#include "render_path.h"
#include "resource_manager.h"
#include "window.h"

#include "scene_game.h"
#include "scene_title.h"

#include "ecs.h"
#include "light.h"
#include "view.h"

// Components
#include "anim_sprite.h"
#include "mesh_object.h"
#include "sprite_object.h"

//*************************************
//
//	Initialization process
//	(Apply to all scenes)
//
//*************************************
const std::shared_ptr<Scene>& Scene::Execute(Framework* framework)
{
	if (!this->framework)
	{
		this->framework = framework;
	}

	Initialize();

	while (framework->ProcessLoop())
	{
		const float delta_time = std::min(0.03f, static_cast<float>(framework->GetHighResolutionTimer().GetDeltaTime()));

		// Update process
		Update(delta_time);

		framework->DrawBegin();
		Render();
		framework->DrawEnd();

		if (next_scene)
		{
			break;
		}
	}

	UnInitialize();

	return next_scene;
}

void Scene::Initialize()
{
	next_scene = nullptr;

	// Create Light
	if (!this->light)
	{
		light = std::make_unique<Light>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());
		light->SetLightDir(XMFLOAT3{ .0f,.0f,1.f, });
	}

	if(!this->registry)
	{
		registry = std::make_unique<cumulonimbus::ecs::Registry>();
	}

	if (!this->main_camera)
	{// Create Camera
		main_camera = std::make_unique<Camera>(registry.get());
	}

	if(!this->render_path)
	{
		render_path = std::make_unique<cumulonimbus::renderer::RenderPath>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());
	}

	if (!this->geom_prim_res)
	{// Create Geometric Primitive Data
		geom_prim_res = std::make_unique<GeometricPrimitiveResource>(GetFramework()->GetDevice());
	}

	if (!this->collision_manager)
	{
		collision_manager = std::make_unique<cumulonimbus::collision::CollisionManager>();
	}

	if (!this->sound_resource)
	{// Sound
		sound_resource = std::make_unique<SoundResource>(GetFramework()->GetWindow()->GetHWND());
	}
#ifdef _DEBUG
	if (!this->editor_manager)
	{
		editor_manager = std::make_unique<EditorManager>();
	}
#endif

	if (!this->texture_resource_manager)
	{
		using namespace cumulonimbus;
		texture_resource_manager = std::make_shared<manager::texture::TextureResourceManager>(locator::Locator::GetDx11Device()->device.Get());
		locator::Locator::Provide<manager::texture::TextureResourceManager>(texture_resource_manager);
	}

	if(!this->resource_manager)
	{
		resource_manager = std::make_shared<ResourceManager>();
		resource_manager->Initialize(GetFramework()->GetDevice());
		cumulonimbus::locator::Locator::Provide<ResourceManager>(resource_manager);
	}

	//pad_combine = std::make_unique<pad_link::PadLink>();
	//{
	//	pad_combine->Register(InputType::GamePad::Up_DPad, InputType::Key::KEY_W);
	//	pad_combine->Register(InputType::GamePad::Left_DPad, InputType::Key::KEY_A);
	//	pad_combine->Register(InputType::GamePad::Down_DPad, InputType::Key::KEY_S);
	//	pad_combine->Register(InputType::GamePad::Right_DPad, InputType::Key::KEY_D);
	//	pad_combine->Register(InputType::GamePad::Right_Shoulder, InputType::Mouse::RIGHT);
	//	pad_combine->Register(InputType::GamePad::Pad_A, InputType::Key::KEY_SPACE);
	//	//pad_combine->Register(InputType::GamePad::Pad_X, InputType::Key::KEY_F);
	//	pad_combine->Register(InputType::GamePad::Pad_X, InputType::Mouse::LEFT);
	//	pad_combine->Register(InputType::GamePad::Left_Shoulder, InputType::Key::KEY_R);
	//	pad_combine->Register(InputType::GamePad::Start, InputType::Key::KEY_BACK);
	//}

	InitializeScene();
}

void Scene::UnInitialize()
{
	registry->DestroyAllEntities();
	UnInitializeScene();
}

void Scene::Update(const float elapsed_time)
{
	//if(!is_paused)
	collision_manager->Update(registry.get());

	registry->PreUpdate(elapsed_time);
	registry->Update(elapsed_time);
	registry->PostUpdate(elapsed_time);

	main_camera->SetProjection(XM_PI / 8.0f, static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);

	// Camera update
	main_camera->Update(elapsed_time);

	// light update
	light->Update(main_camera.get());

	UpdateScene(elapsed_time);

	if (cumulonimbus::locator::Locator::GetInput()->Keyboard().GetState(Keycode::A) == ButtonState::Press)
	{
		//this->SaveScene(this->scene_name);
	}

	if (cumulonimbus::locator::Locator::GetInput()->Keyboard().GetState(Keycode::L) == ButtonState::Press)
	{
		//this->LoadScene("./Content/Scene/Scene_Title");
	}
}

void Scene::Render()
{
	auto* immediate_context = framework->GetDeviceContext();

	render_path->Render(immediate_context, registry.get(), main_camera.get(), light.get());

#ifdef _DEBUG
	// ImGui
	{
		const RECT window_rect = cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect();

		//ImGui::SetNextWindowPos(ImVec2{ static_cast<float>(window_rect.right), static_cast<float>(window_rect.top)});
		ImGui::Begin("Scene");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		this->RenderImGui();
		main_camera->WriteImGui();
		light->WriteImGui();
		if (ImGui::CollapsingHeader("Objects"))
		{
			registry->RenderImGui();
		}
		ImGui::Text("window left    : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().left);
		ImGui::Text("window right   : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().right);
		ImGui::Text("window top     : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().top);
		ImGui::Text("window bottom  : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisWindowRect().bottom);

		ImGui::Text("client left    : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().left);
		ImGui::Text("client right   : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().right);
		ImGui::Text("client top     : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().top);
		ImGui::Text("client bottom  : %d", cumulonimbus::locator::Locator::GetWindow()->GetThisClientRect().bottom);

		POINT cursor_pos{};
		GetCursorPos(&cursor_pos);
		ImGui::Text("mouse x : %d", cursor_pos.x);
		ImGui::Text("mouse y : %d", cursor_pos.y);

		ImGui::End();

		editor_manager->RenderEditor(this);
	}
#endif
}

void Scene::WriteImGui() const
{
	if (ImGui::CollapsingHeader("Light Info"))
	{
		//light->WriteImGui();

		//ImGui::DragFloat("Direction X", &light->GetLightDir().x, 0.01f, -1.0f, 1.0f);
		//ImGui::DragFloat("Direction Y", &light->GetLightDir().y, 0.01f, -1.0f, 1.0f);
		//ImGui::DragFloat("Direction Z", &light->GetLightDir().z, 0.01f, -1.0f, 1.0f);
	}
}

//-------------------------------------
//
// SceneManager Class
//
//-------------------------------------
void SceneManager::Execute(Framework* framework)
{
	//Scene* scene = new SceneTitle();
	std::shared_ptr<Scene> scene = std::make_shared<SceneGame>();

	while (scene)
	{
		scene = scene->Execute(framework);
	}
}
