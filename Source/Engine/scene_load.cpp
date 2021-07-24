#include <thread>

#include <cereal/types/array.hpp>

#include "scene.h"
#include "scene_load.h"
#include "sprite.h"

SceneLoad::SceneLoad(Scene* scene)
{
	this->next_scene.reset(scene);

	is_initialized = true;
}

void SceneLoad::InitializeScene()
{
	//-- Camera --//
	main_camera->SetViewInfo(XMFLOAT3(.0f, 100.0f, -100.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));

	// �X���b�h�J�n(this�͈����Astatic�֐��ȊO���ĂԂ̂ŕK�v)
	std::thread thread(LoadingThread, this);
	// �����̊֐��������Ă�LoadingThead()�����s�����邽�ߎ����
	thread.detach();
}

void SceneLoad::UnInitializeScene()
{

}

void SceneLoad::UpdateScene(const float delta_time)
{
	int a;
	a = 0;

	//// ���̃V�[����
	//if (is_initialized)
	//{
	//	SetNextScene(next_scene.get());
	//	is_initialized = false;
	//}
}

void SceneLoad::LoadingThread(SceneLoad* scene)
{
	// ���̃V�[����
	if (scene->is_initialized)
	{
		scene->SetNextScene(scene->next_scene);
		scene->is_initialized = false;
	}
}