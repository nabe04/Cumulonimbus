#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>
#include <fstream>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>


// Entity Update Order (The smaller number is updated first)
enum class UpdateOrder
{
	Default,

	Sword,

	LightMap,

	End
};

enum class EntityTag
{
	Default,
	Sky_Map,

	Player,
	Enemy,
	Bunny,

	End
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
	const std::string& GetEntityName() const { return entity_name; }

	// Serialize
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(
			CEREAL_NVP(components),	// 処理落ちするので実装していない
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

namespace cumulonimbus
{
	namespace component
	{
		class ActorComponent;

	}
}


namespace cumulonimbus::ecs
{
	enum class Entity : uint64_t {};
	using EntityId = uint64_t;	// Entityの識別子
	using ComponentId = uint64_t;	// Componentの識別子

	static const uint64_t START_ID = 0;	// EntityId,ComponentIdの始まりの識別子(最初のIDはどれも"1"から)


	class ComponentArrayBase
	{
	public:
		virtual ~ComponentArrayBase() = default;

		virtual void Destroy(Entity entity) = 0;
		virtual void Save(const std::string& filename) = 0;
		virtual size_t GetHashCode() = 0;
	};

	template <typename T>
	class ComponentArray final : public ComponentArrayBase
	{
	public:
		ComponentArray() = default;

		size_t GetHashCode() override
		{
			return typeid(T).hash_code();
		}

		/*
		 * brief : EntityのT型Componentを追加
		 *		 : 既にComponentを持っていた場合は、
		 *		 　新しく作らず持っているComponentを返す
		 */
		template <typename... Args>
		T& AddComponent(class Registry* registry, const Entity entity, Args... args)
		{
			if (entity_id.contains(entity))
			{
				const size_t index = static_cast<size_t>(entity_id.at(entity));
				return components.at(index);
			}

			components.emplace_back(T{ registry, entity ,args... });

			const size_t index = static_cast<size_t>(entity_id.emplace(entity, components.size() - 1).first->second);

			return  components.at(index);
		}

		void RemoveComponent(Entity entity)
		{
			// entityがcomponentを持っていない場合は何もしない
			if (!Content(entity))
				return;

			// EntityIdの取得(componentsのインデックス番号になる)
			const EntityId index = GetEntityID(entity);
			// EntityIdがcomponentsの最後を指していた場合はそのまま削除
			if (index == (components.size() - 1))
			{
				entity_id.erase(entity);
				components.pop_back();
				return;
			}

			// コンポーネントとコンポーネント配列の最後の要素を入れ替える
			std::iter_swap(components.begin() + static_cast<int>(index), components.end() - 1);

			// もともと最後を指していたエンティティのインデックスを修正する
			entity_id.erase(entity);
			entity_id.at(static_cast<Entity>(components.size() - 1)) = index;
			// コンポーネントとインデックスを削除する
			components.pop_back();
		}

		void Destroy(const Entity entity) override
		{
			if (!entity_id.contains(entity))
				return;

			// RegistryのRemoveComponent()と同じ処理
			RemoveComponent(entity);
		}

		/*
		 *   brief	 : Entityが持つComponentを返す
		 * ※caution : EntityがComponentを持っていない場合、
		 *			　 新しくComponentを作るが、引数なしの
		 *			   コンストラクタがあるクラス限定
		 */
		T& GetComponent(const Entity entity)
		{
			if (!entity_id.contains(entity))
				assert(!"Don't have component");

			const size_t index = static_cast<size_t>(entity_id.at(entity));
			return components.at(index);
		}

		/*
		 *  brief : EntityがComponentを持っているか
		*/
		[[nodiscard]] bool Content(const Entity entity) const
		{
			if (entity_id.contains(entity))
				return true;

			return false;
		}

		std::vector<T>& GetComponents()
		{
			return components;
		}

		/*
		 * brief : EntityのEntityID(componentsのインデックス番号)を返す
		 *		 : EntityがComponentを持ってない場合は"-1"を返す
		 */
		EntityId GetEntityID(Entity entity)
		{
			if (!Content(entity))
				return -1;

			return entity_id.at(entity);
		}

		void Save(const std::string& filename) override
		{
			//for(auto& component : components)
			//{
			//	component.Save(filename);
			//}

			std::ofstream ofs(filename);
			cereal::JSONOutputArchive o_archive(ofs);
			o_archive(
				CEREAL_NVP(components),
				CEREAL_NVP(entity_id)
			);
		}

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(components),
				CEREAL_NVP(entity_id)
			);
		}

	private:
		std::vector<T> components;
		std::unordered_map<Entity, EntityId> entity_id;
	};

	class Registry final
	{

	public:
		/*
		 * brief : Entityの削除
		 *         Entityに含まれるコンポーネントも削除
		 */
		void Destroy(Entity entity)
		{
			if (!entities.contains(entity))
				return;

			for (auto&& [component_id, array_base] : component_arrays)
			{
				array_base->Destroy(entity);
			}

			entities.erase(entity);
		}

		template <typename T>
		[[nodiscard]] T& GetComponent(const Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			if (!array.Content(entity))
				assert(!"Don't have Component");

			return array.GetComponent(entity);
		}

		/*
		 * brief      : Entityが持つT型のComponentを返す
		 * ※caution  : EntityがT型を保持していない場合はnullptrを返す
		 */
		template <typename T>
		[[nodiscard]] T* TryGetComponent(const Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			if (!array.Content(entity))
				return nullptr;

			return &array.GetComponent(entity);
		}

		template <typename T, typename... Args>
		T& AddComponent(const Entity entity, Args... args)
		{
			ComponentArray<T>& array = GetArray<T>();
			array.AddComponent(this, entity, args...);

			return array.GetComponent(entity);
		}

		/*
		 * brief : T型のComponentのEntityを削除
		 */
		template <typename T>
		void RemoveComponent(const Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			array.RemoveComponent(entity);
		}

		/*
		 * brief : T型が持つComponentId(uint64_t)からComponentArrayを取得する
		 */
		template <typename T>
		ComponentArray<T>& GetArray()
		{
			const ComponentId component_id = GetComponentId<T>();

			if (!component_arrays.contains(component_id))
			{
				return  static_cast<ComponentArray<T>&>(*component_arrays.emplace(component_id, std::make_unique<ComponentArray<T>>()).first->second);
			}

			return static_cast<ComponentArray<T>&>(*component_arrays.at(component_id).get());
		}

		/*
		 * brief : コンポーネントがあれば持っているコンポーネントを返し
		 *         なければ、コンポーネントを作る
		 */
		template<typename T, typename... Args>
		T& GetOrEmplaceComponent(Entity entity, Args... args)
		{
			T* t = TryGetComponent<T>(entity);
			if (t)
			{
				return *t;
			}

			// 追加処理
			return AddComponent<T>(entity, args...);
		}

		template <typename T>
		[[nodiscard]] bool HasComponent(Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			if (!array.Content(entity))
				return false;

			return true;
		}

		[[nodiscard]] const std::unordered_map<Entity, Entity>& GetEntities() const
		{
			return entities;
		}

		/*
		 * brief : Entityの作成
		 */
		Entity CreateEntity()
		{
			static uint64_t entity = START_ID;

			// 後置インクリメントをしているのはSTART_IDを初めの要素にセットしたいから
			while (entities.contains(static_cast<Entity>(entity)))
			{
				entity++;
			}

			entities.emplace(static_cast<Entity>(entity), static_cast<Entity>(entity));
			return entities.at(static_cast<Entity>(entity));
		}

		void Save(const std::string& filename)
		{
			std::ofstream ofs(filename);
			cereal::JSONOutputArchive output_archive(ofs);
			output_archive(
				CEREAL_NVP(component_arrays),
				CEREAL_NVP(entities)
			);
		}

		void Load(const std::string& filename)
		{
			std::ifstream ifs(filename);
			cereal::JSONInputArchive input_archive(ifs);
			input_archive(
				CEREAL_NVP(component_arrays),
				CEREAL_NVP(entities)
			);

			RemapId();
		}

		void SetScene(Scene* scene) { this->scene = scene; }
		[[nodiscard]] Scene* GetScene() const { return scene; }

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(component_arrays),
				CEREAL_NVP(entities)
			);
		}

	private:
		template <typename T>
		static ComponentId GetComponentId()
		{
			static ComponentId id = typeid(T).hash_code();
			return id;
		}

		/*
		 * brief : デシリアライズした時にtype_id.hash_code()が
		 *		   環境によって変わるので、再設定する
		 */
		void RemapId()
		{
			std::vector<ComponentId> hashes;

			for (auto&& [first, second] : component_arrays)
			{// component_arraysのComponentId(ハッシュコード)を追加
				hashes.emplace_back(first);
			}

			for (auto& it : hashes)
			{// ComponentIdの再割り当て
				const ComponentId hash_code = component_arrays.find(it)->second->GetHashCode();
				auto node = component_arrays.extract(it);
				node.key() = hash_code;
				component_arrays.insert(std::move(node));
			}
		}

	private:
		std::unordered_map<ComponentId, std::unique_ptr<ComponentArrayBase>> component_arrays;
		std::unordered_map<Entity, Entity> entities;
		Scene* scene{};
	};
}


CEREAL_REGISTER_TYPE(cumulonimbus::ecs::ComponentArray<cumulonimbus::component::ActorComponent>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::ecs::ComponentArrayBase, cumulonimbus::ecs::ComponentArray<cumulonimbus::component::ActorComponent>)

//CEREAL_REGISTER_TYPE(cumulonimbus::ecs::ComponentArray<cumulonimbus::component::SpriteComponent>);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::ecs::ComponentArrayBase, cumulonimbus::ecs::ComponentArray<cumulonimbus::component::SpriteComponent>)
//
//CEREAL_REGISTER_TYPE(cumulonimbus::ecs::ComponentArray<cumulonimbus::component::SpriteObjectComponent>);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::ecs::ComponentArrayBase, cumulonimbus::ecs::ComponentArray<cumulonimbus::component::SpriteObjectComponent>)