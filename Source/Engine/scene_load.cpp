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

	// スレッド開始(thisは引数、static関数以外を呼ぶので必要)
	std::thread thread(LoadingThread, this);
	// ※この関数が抜けてもLoadingThead()を実行させるため手放す
	thread.detach();
}

void SceneLoad::UnInitializeScene()
{

}

void SceneLoad::UpdateScene(const float delta_time)
{
	int a;
	a = 0;

	//// 次のシーンへ
	//if (is_initialized)
	//{
	//	SetNextScene(next_scene.get());
	//	is_initialized = false;
	//}
}

void SceneLoad::LoadingThread(SceneLoad* scene)
{
	// 次のシーンへ
	if (scene->is_initialized)
	{
		scene->SetNextScene(scene->next_scene);
		scene->is_initialized = false;
	}
}