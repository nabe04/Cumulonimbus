#include "scene.h"

#include "window.h"
#include "input_manager.h"
#include "fbx_model_component.h"
#include "geometric_primitive_component.h"
#include "geometric_primitive_resource.h"
#include "input_manager.h"
#include "locator.h"
#include "resource_manager.h"

#include "scene_title.h"

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

		// Remove dead actors
		for (auto& it : dead_entities)
		{
			RemoveEntity(it);
		}
		dead_entities.clear();

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
		light = std::make_unique<Light>();
		light->SetLightDir(XMFLOAT4{ .0f,.0f,1.f,1.f });
	}

	if (!this->view)
	{// Create View
		view = std::make_unique<View>();
	}

	if(!this->camera_work)
	{
		camera_work = std::make_unique<CameraWork>();
	}

	if (!this->geom_prim_res)
	{// Create Geometric Primitive Data
		geom_prim_res = std::make_unique<GeometricPrimitiveResource>(GetFramework()->GetDevice());
	}

	if (!this->sprite_renderer)
	{
		sprite_renderer = std::make_unique<SpriteRenderer>(GetFramework()->GetDevice());
	}

	if (!this->model_render)
	{
		model_render = std::make_unique<MeshRenderer>(GetFramework()->GetDevice());
	}

	if (!this->collision_manager)
	{
		collision_manager = std::make_unique<CollisionManager>();
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

	if(!this->resource_manager)
	{
		resource_manager = std::make_shared<ResourceManager>();
		resource_manager->Initialize(GetFramework()->GetDevice());
		Locator::Provide<ResourceManager>(resource_manager.get());
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
	entities.clear();
	dead_entities.clear();
	UnInitializeScene();
}

void Scene::Update(const float elapsed_time)
{
	// ƒeƒXƒg
	camera_work->Update(true);
	view->SetCameraTarget(camera_work->GetCameraFront() * 500);
	view->SetPosition(camera_work->GetPositon());
	view->SetCameraUp({ 0,1,0 });

	if(!is_paused)
		collision_manager->Update(this);

	for (auto& ent : entities)
	{
		ent->NewFrame(elapsed_time);
	}

	for (auto& ent : entities)
	{
		ent->Update(elapsed_time);
	}

	view->SetProjection(XM_PI / 8.0f, static_cast<float>(DxManager::GetInstance().GetScreenWidth()) / static_cast<float>(DxManager::GetInstance().GetScreenHeight()), 0.1f, 2000.0f);

	// View update
	view->Activate();

	// light update
	light->Update(view.get());

	UpdateScene(elapsed_time);

	if (Locator::GetInput().Keyboard().GetState(Keycode::A) == ButtonState::Press)
	{
		this->SaveScene(this->scene_name);
	}

	if (Locator::GetInput().Keyboard().GetState(Keycode::L) == ButtonState::Press)
	{
		this->LoadScene("./Content/Scene/Scene_Title");
	}

	if (is_paused)
		return;
}

void Scene::Render()
{
	auto* immediate_context = framework->GetDeviceContext();

	model_render->ShadowBegin(immediate_context);
	for (const auto& ent : *GetEntities())
	{
		model_render->RenderShadow(immediate_context, &(*ent), view, light);
	}
	model_render->ShadowEnd(immediate_context);

	model_render->Begin(immediate_context);

	for (const auto& ent : *GetEntities())
	{
		model_render->RenderSkyBox(immediate_context, &(*ent), view, light);
	}

	for (const auto& ent : *GetEntities())
	{
		model_render->Render(immediate_context, &(*ent), view, light);
	}
	model_render->End(immediate_context);

	for (const auto& ent : *GetEntities())
	{
		sprite_renderer->Render(immediate_context, &(*ent));
	}

#ifdef _DEBUG
	// ImGui
	{
		ImGui::Begin("Scene");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		this->RenderImGui();
		view->WriteImGui();
		light->WriteImGui();
		if (ImGui::CollapsingHeader("Objects"))
		{
			for (const auto& ent : *GetEntities())
			{
				// Render ImGui
				ent->RenderImgui();
			}
		}
		ImGui::Text("window x : %d", Locator::GetWindow().WindowRect().left);
		ImGui::Text("window y : %d", Locator::GetWindow().WindowRect().top);
		POINT po{};
		GetCursorPos(&po);
		ImGui::Text("mouse x : %d",po.x);
		ImGui::Text("mouse y : %d", po.y);

		camera_work->RenderImGui();

		ImGui::End();

		editor_manager->RenderEditor(this);
	}
#endif
}

void Scene::RemoveEntity(Entity* entity)
{
	for (auto it = entities.begin(), end = entities.end(); it != end; ++it)
	{
		if (it->get() == entity) {
			entities.erase(it);
			break;
		}
	}

	dead_entities.emplace_back(entity);
}

void Scene::AddRemoveEntity(Entity* entity)
{
	dead_entities.emplace_back(entity);
	return;
}

void Scene::WriteImGui()
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
	std::shared_ptr<Scene> scene = std::make_shared<SceneTitle>();

	while (scene)
	{
		scene = scene->Execute(framework);
	}
}
