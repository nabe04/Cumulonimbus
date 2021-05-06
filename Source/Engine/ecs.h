#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include <string_view>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>

// Entity Update Order (The smaller number is updated first)
enum class UpdateOrder
{
	Default,

	Sword,

	LightMap,
};

enum class EntityTag
{
	Default,
	Sky_Map,

	Player,
	Enemy,
	Bunny,
};

//----------------------------------------< Entity Class >-----------------------------------------------------
#pragma region Entity
class Entity final
{
	using ComponentVector = std::vector<std::unique_ptr<class Component>>;

public:
	explicit Entity(class Scene* scene, const UpdateOrder update_order = UpdateOrder::Default, const EntityTag entity_tag = EntityTag::Default);
	Entity() = default;
	~Entity() = default;

private:
	ComponentVector components		= {};
	UpdateOrder		update_order	= UpdateOrder::Default;
	EntityTag		entity_tag		= EntityTag::Default;
	Scene*			scene			= {};
	bool			is_apply_paused = true;

	// For ImGui
	std::string entity_name = "Entity";

	// For cereal
	std::vector<std::string> component_names;


public:
#pragma region
	/*
	* brief      : Adding a component
	* param T    : Type of component to be added
	* param args : Constructor arguments of "Component"
	* return     : Pointer to added components
	*/
#pragma  endregion AddComponent
	template <class T,class... Args>
	T* AddComponent(Args&&... args)
	{
		T* ptr = new T{ this,std::forward<Args>(args)... };

		if (HasComponent<T>())
			assert(!"Don't have a Component");

		components.emplace_back(std::unique_ptr<T>{ptr});
		component_names.emplace_back(ptr->GetName());

		return ptr;
	}

#pragma region
	/*
	* brief    : Getting a component
	* param T  : Type of component to be retrieved
	* return   : Pointer to component if found, or nullptr if not found
	*/
#pragma endregion  GetComponent
	template <class T>
	[[nodiscard]] T* GetComponent() const
	{
		for (auto& comp : components)
		{
			T* ptr = dynamic_cast<T*>(comp.get());
			if (ptr)
			{
				return ptr;
			}
		}
		//assert(!"Component not found. in Entity::GetComponent");
		return nullptr;
	}

#pragma region
	/*
	* brief    : Getting a component
	* param T  : Type of component to be retrieved
	* return   : Pointer to component if found, or nullptr if not found
	*/
#pragma endregion GetSameComponents
	template <class T>
	[[nodiscard]] std::vector<T*> GetSameComponents() const
	{
		std::vector<T*> comps;

		for (auto& comp : components)
		{
			T* ptr = dynamic_cast<T*>(comp.get());

			if (ptr)
				comps.emplace_back(std::move(ptr));
		}

		if (!comps.empty())
			return comps;

		//assert(!"Component not found. in Entity::GetComponent");
		return comps;
	}

#pragma region
	/*
	* brief      : Find out if you own a component
	* param T    : Types of components to look for
	* param Tail : Types of components to look for
	* return     : [true] I own them all, including "Tail".  [false] I don't own more than one, including "Tail".
	*/
#pragma endregion HasComponent
	template <class T, class... Tail>
	[[nodiscard]] bool HasComponent() const { return ComponentHelper<T, Tail...>::Has(this); }

#pragma region
	/*
	* brief           : Functions called before "Entity::Update()"
	* param delta_time : Time elapsed since the previous frame
	*/
#pragma endregion NewFrame
	void NewFrame(const float delta_time);

#pragma region
	/*
	* brief			  : Update process
	* param delta_time : Time elapsed since the previous frame
	*/
#pragma endregion Update
	void Update(const float delta_time);

#pragma region
	/*
	* brief : Rendering of ImGui
	*/
#pragma endregion RenderImgui
	void RenderImgui();

	/*
	 * brief : EntityのSave
	 * file_path : 保存したいファイルパス(最後に「/」は付けない)
	 */
	void Save(std::string file_path);

	/*
	 * brief : EntityのLoad(主にComponentのLoad)
	 * file_path : 読み込みたいファイルパス(最後に「/」は付けない)
	 */
	void Load(std::string file_path);

	void SetScene(Scene* scene) { this->scene = scene; }
	auto*   GetScene()		 const { return scene; }
	auto	GetUpdateOrder() const { return update_order; }

	auto GetEntityTag() { return entity_tag; }

	void IsApplyPaused(bool flg) { is_apply_paused = flg; }

	// ImGui
	void SetEntityName(std::string_view name) { entity_name = name.data(); }
	std::string GetEntityName() { return entity_name; }

	// Serialize
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(
			//CEREAL_NVP(components),	// 処理落ちするので実装していない
			CEREAL_NVP(entity_name),
			CEREAL_NVP(component_names),
			CEREAL_NVP(update_order),
			CEREAL_NVP(entity_tag)
		);
	}

private:
	template <class...>
	struct ComponentHelper
	{
		[[nodiscard]] static bool Has(const Entity*) { return true; }
	};
	template <class Head, class... Tail>
	struct ComponentHelper<Head, Tail...>
	{
		[[nodiscard]] static bool Has(const Entity* entity)
		{
			for (const auto& comp : entity->components)
			{
				if (dynamic_cast<Head*>(comp.get()))
				{
					return ComponentHelper<Tail...>::Has(entity);
				}
			}
			return false;
		}
	};

	bool ApplyPaused();
};
#pragma endregion Entity

//----------------------------------------< Component Class>---------------------------------------------------
#pragma region Component
class Component
{
protected:
	Entity* entity = nullptr;
	// For cereal
	std::string component_name = "Origin";

public:
	explicit Component(class Entity* entity)
		:entity{entity}{}
	Component() = default;
	virtual ~Component() = default;

	/*
	* brief           : Functions called before "Entity::Update()"
	* param delta_time : Time elapsed since the previous frame
	*/
	virtual void NewFrame(const float delatTime){}

	/*
	* brief			  : Update process
	* param delta_time : Time elapsed since the previous frame
	*/
	virtual void Update(const float delta_time){}

	/*
	* brief : Rendering of ImGui
	*/
	virtual void RenderImGui(){}


	//-- Getter --//
	auto* GetEntity() const { return entity; }
	std::string GetName() { return component_name; }

	// Serialize
	template <class Archive>
	void serialize(Archive&& ar)
	{
		ar(CEREAL_NVP(component_name));
	}

	virtual void Save(std::string file_path) {}
	virtual void Load(Entity* entity, std::string file_path_and_name) {}
};
#pragma endregion Component
