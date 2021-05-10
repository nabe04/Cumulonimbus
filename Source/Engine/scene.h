
#pragma once
#include <fstream>
#include <filesystem>
#include <memory>
#include <deque>
#include <vector>
#include <string>

#include <DirectXMath.h>

#include <cereal/archives/json.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/memory.hpp>

#include "framework.h"
#include "dx_manager.h"
#include "ecs.h"
#include "light.h"
#include "view.h"
#include "model_render.h"
#include "sprite_renderer.h"
#include "collision.h"
#include "fbx_model_resource.h"
#include "input_manager.h"
#include "sound_resource.h"
#include "resource_manager.h"
#include "editor_manager.h"
#include "camera_work.h"

class GeometricPrimitiveResource;

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
	inline static std::shared_ptr<Scene> next_scene = nullptr;
	inline static Framework*			 framework	= nullptr;

	SceneType current_scene = SceneType::End;

	std::unique_ptr<View>						    view				= nullptr;
	std::unique_ptr<CameraWork>					camera_work			= nullptr;
	std::unique_ptr<Light>						light				= nullptr;
	std::unique_ptr<GeometricPrimitiveResource>   geom_prim_res		= nullptr;
	std::unique_ptr<CollisionManager>				collision_manager	= nullptr;
	std::unique_ptr<SpriteRenderer>				sprite_renderer     = nullptr;
	std::unique_ptr<MeshRenderer>					model_render		= nullptr;
	std::unique_ptr<SoundResource>				sound_resource		= nullptr;
	std::unique_ptr<EditorManager>				editor_manager		= nullptr;
	std::shared_ptr<ResourceManager>				resource_manager	= nullptr;

	//std::unique_ptr <pad_link::PadLink>			pad_combine			= nullptr;

	bool is_paused = false;

	// For cereal
	std::string scene_name{};


private:
	// Actor
	std::deque<std::unique_ptr<Entity>> entities = {};
	std::vector<Entity*>				dead_entities = {};

public:
	void operator=(Scene&) = delete;

	explicit Scene() = default;
	virtual ~Scene() = default;

	const std::shared_ptr<Scene>& Execute(Framework* framework);
	void SetNextScene(const std::shared_ptr<Scene>& scene) { next_scene = scene; }

	auto* GetFramework()		{ return framework; }
	auto* GetView()				{ return view.get(); }
	auto* GetLight()			{ return light.get(); }
	auto* GetGeomPrimRes()		{ return geom_prim_res.get(); }
	//auto* GetPadLink()			{ return pad_combine.get(); }
	auto* GetSoundResourceManager() { return sound_resource.get(); }

	void IsPaused(bool flg) { is_paused = flg; }
	bool IsPaused() { return is_paused; }

	//-- Entity --//
	Entity* AddEntity(const UpdateOrder update_order = UpdateOrder::Default,const EntityTag entity_tag = EntityTag::Default)
	{
		return AddEntityToArray(std::make_unique<Entity>(this, update_order, entity_tag));
	}
	void AddRemoveEntity(Entity* entity);

	auto* GetEntities()const { return &entities; }
	template<class T,class... Args>
	std::vector<Entity*> GetEntities() const
	{
		std::vector<Entity*> entities_;
		for (auto& ent : entities)
		{
			if (ent->HasComponent<T, Args...>())
			{
				entities_.emplace_back(ent.get());
			}
		}
		return entities_;
	}

	Entity* GetOtherEntity(EntityTag tag)
	{
		for (auto& ent : entities)
		{
			if (tag == ent->GetEntityTag())
			{
				return &(*ent);
			}
		}

		return nullptr;
	}

	const SceneType& GetCurrentScene() { return current_scene; }

	//-- ImGui --//
	void WriteImGui();

	//-- Serialize --//
	/*
	* brief    : シリアライズされた際に保存するファイル
	* filename  : ファイル名()
	*/
	void SaveScene(std::string filename)
	{
		const std::string file_path_and_name = "./Content/Scene/" + filename;
		std::filesystem::create_directories(file_path_and_name);

		const std::string exe = ".json";
		const std::string scene_file_path_and_name = file_path_and_name + exe;

		{ // save scene
			std::ofstream ofs(scene_file_path_and_name);
			cereal::JSONOutputArchive o_archive(ofs);
			o_archive(
				CEREAL_NVP(scene_name),
				CEREAL_NVP(entities)
			);
		}

		{// save entity
			if (this->entities.empty())
				return;

			const std::string entity_file_path = file_path_and_name + "/Entity";
			std::filesystem::create_directories(entity_file_path);

			for(const auto& ent : entities)
			{
				ent->Save(entity_file_path);
			}
		}
	}

	/*
	 * brief : シーンのロード
	 * file_path_and_name : 拡張子を除く、ファイルの相対パス
	 */
	void LoadScene(std::string file_path_and_name)
	{
		const std::string exe = ".json";

		{// load entities
			const std::string input_filename = file_path_and_name + exe;

			std::ifstream ifs(input_filename);
			cereal::JSONInputArchive i_archive(ifs);
			i_archive(
				CEREAL_NVP(scene_name),
				CEREAL_NVP(entities)
			);
		}

		for(auto& ent:entities)
		{
			ent->SetScene(this);

			const std::string entity_file_path_and_name = file_path_and_name + "/Entity";
			ent->Load(entity_file_path_and_name);
		}
	}

	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(scene_name),
			CEREAL_NVP(entities)
		);
	}

private:
	Entity* AddEntityToArray(const UpdateOrder update_order) { return AddEntityToArray(std::make_unique<Entity>(this, update_order)); }
	Entity* AddEntityToArray(std::unique_ptr<Entity> entity)
	{
		if (entities.empty())
		{
			return entities.emplace_back(std::move(entity)).get();
		}

		// .end() : 末尾の次の要素のイテレーターを返す
		for (auto it = entities.begin(); it != entities.end(); ++it)
		{
			if (entity->GetUpdateOrder() < it->get()->GetUpdateOrder())
			{
				return entities.emplace(it, std::move(entity))->get();
			}
		}
		return entities.emplace_back(std::move(entity)).get();
	}

	void			Initialize();
	void			UnInitialize();
	void			Update(const float elapsed_time);
	void			Render();
	virtual void	InitializeScene() {}
	virtual void	UnInitializeScene() = 0;
	virtual void	UpdateScene(const float elapsed_time) = 0;
	virtual void	RenderImGui() {}

	void RemoveEntity(Entity* entity);
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