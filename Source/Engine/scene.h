
#pragma once
#include <deque>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <DirectXMath.h>

#include "camera_work.h"
#include "collision.h"
#include "dx11_configurator.h"
#include "ecs.h"
#include "editor_manager.h"
#include "gaussian_blur.h"
#include "geometric_primitive_resource.h"
#include "fbx_model_resource.h"
#include "framework.h"
#include "input_manager.h"
#include "light.h"
#include "render_path.h"
#include "resource_manager.h"
#include "sound_resource.h"
#include "view.h"

class GeometricPrimitiveResource;
namespace cumulonimbus::renderer
{
	class RenderPath;
}


enum class SceneType
{
	Title,
	Game,
	Tutorial,

	End,
};

class Scene
{
protected:
	//inline static Scene* next_scene		= nullptr;
	inline static std::shared_ptr<Scene> next_scene { nullptr };
	inline static Framework*			 framework  { nullptr };

	SceneType current_scene = SceneType::End;

	std::shared_ptr<ResourceManager>					resource_manager	{ nullptr };
	std::unique_ptr<cumulonimbus::ecs::Registry>		registry			{ nullptr };
	std::unique_ptr<CollisionManager>					collision_manager	{ nullptr };
	std::unique_ptr<EditorManager>						editor_manager		{ nullptr };
	std::unique_ptr<GeometricPrimitiveResource>			geom_prim_res		{ nullptr };
	std::unique_ptr<Light>								light				{ nullptr };
	std::unique_ptr<SoundResource>						sound_resource		{ nullptr };
	std::unique_ptr<View>								view				{ nullptr };
	std::unique_ptr<cumulonimbus::renderer::RenderPath> render_path			{ nullptr };

	//std::unique_ptr <pad_link::PadLink>			pad_combine			= nullptr;

	bool is_paused{ false };

	// For cereal
	std::string scene_name{};

public:
	void operator=(Scene&) = delete;

	explicit Scene() = default;
	virtual ~Scene() = default;

	[[nodiscard]] const std::shared_ptr<Scene>& Execute(Framework* framework);
	void SetNextScene(const std::shared_ptr<Scene>& scene) { next_scene = scene; }

	[[nodiscard]] auto* GetFramework() const { return framework; }
	[[nodiscard]] auto* GetView() const{ return view.get(); }
	[[nodiscard]] auto* GetLight() const { return light.get(); }
	[[nodiscard]] auto* GetGeomPrimRes() const { return geom_prim_res.get(); }
	//auto* GetPadLink()			{ return pad_combine.get(); }
	[[nodiscard]] auto* GetSoundResourceManager() const { return sound_resource.get(); }
	void IsPaused(bool flg) { is_paused = flg; }
	[[nodiscard]] bool IsPaused() const { return is_paused; }


	[[nodiscard]] const SceneType& GetCurrentScene() const { return current_scene; }

	//-- ImGui --//
	void WriteImGui() const;

	//-- Serialize --//
	/*
	* brief    : シリアライズされた際に保存するファイル
	* filename  : ファイル名()
	*/
	void SaveScene(std::string filename)
	{
		//const std::string file_path_and_name = "./Content/Scene/" + filename;
		//std::filesystem::create_directories(file_path_and_name);

		//const std::string exe = ".json";
		//const std::string scene_file_path_and_name = file_path_and_name + exe;

		//{ // save scene
		//	std::ofstream ofs(scene_file_path_and_name);
		//	cereal::JSONOutputArchive o_archive(ofs);
		//	o_archive(
		//		CEREAL_NVP(scene_name),
		//		CEREAL_NVP(entities)
		//	);
		//}

		//{// save entity
		//	if (this->entities.empty())
		//		return;

		//	const std::string entity_file_path = file_path_and_name + "/Entity";
		//	std::filesystem::create_directories(entity_file_path);

		//	for(const auto& ent : entities)
		//	{
		//		ent->Save(entity_file_path);
		//	}
		//}
	}

	/*
	 * brief : シーンのロード
	 * file_path_and_name : 拡張子を除く、ファイルの相対パス
	 */
	void LoadScene(std::string file_path_and_name)
	{
		//const std::string exe = ".json";

		//{// load entities
		//	const std::string input_filename = file_path_and_name + exe;

		//	std::ifstream ifs(input_filename);
		//	cereal::JSONInputArchive i_archive(ifs);
		//	i_archive(
		//		CEREAL_NVP(scene_name),
		//		CEREAL_NVP(entities)
		//	);
		//}

		//for(auto& ent:entities)
		//{
		//	ent->SetScene(this);

		//	const std::string entity_file_path_and_name = file_path_and_name + "/Entity";
		//	ent->Load(entity_file_path_and_name);
		//}
	}

	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(scene_name)
		);
	}

private:
	void			Initialize();
	void			UnInitialize();
	void			Update(const float elapsed_time);
	void			Render();
	virtual void	InitializeScene() {}
	virtual void	UnInitializeScene() = 0;
	virtual void	UpdateScene(const float elapsed_time) = 0;
	virtual void	RenderImGui() {}
};

class SceneManager final
{
public:
	explicit SceneManager() = default;
	~SceneManager() = default;

	void Execute(Framework* framework);
};

// とりあえずやりたいことのテスト
class Scene01 :public Scene
{
private:
	std::shared_ptr<FbxModelResource> fbxCube;
	std::shared_ptr<FbxModelResource> resCharacter;

public:
	explicit Scene01() = default;
	~Scene01() override = default;

public:
	static auto GetInstance()
	{
		static Scene01 instance{};
		return &instance;
	}

	void InitializeScene() override;
	void UnInitializeScene() override;
	void UpdateScene(const float delta_time) override;
};

// 主に2D系のテスト
class Scene02 :public Scene
{
public:
	explicit Scene02()	= default;
	~Scene02() override		= default;

public:
	static auto GetInstance()
	{
		static Scene02 instance{};
		return &instance;
	}

	void InitializeScene() override;
	void UnInitializeScene() override;
	void UpdateScene(const float delta_time) override;
};

// スカイマップを適用
class Scene03 : public Scene
{
private:
	//std::shared_ptr<FbxModelResource> fbxTestBox;
public:
	explicit Scene03()	= default;
	~Scene03() override = default;

public:
	static auto GetInstance()
	{
		static Scene03 instance{};
		return &instance;
	}

	void InitializeScene() override;
	void UnInitializeScene() override;
	void UpdateScene(const float delta_time) override;
};