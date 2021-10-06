#pragma once

#include <algorithm>
#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#include "file_path_helper.h"
#include "rename_type_mapping.h"
#include "prefab.h"

namespace cumulonimbus
{
	namespace scene
	{
		class Scene;
	} // scene

	namespace component
	{
		class ComponentBase;
	} // component

} // cumulonimbus

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

namespace cumulonimbus::ecs
{
	class Registry;
	//enum class Entity : uint64_t {};
	//using EntityId		= uint64_t;	// Entityの識別子
	//using EntityName	= std::string;
	//using ComponentId	= uint64_t;	// Componentの識別子
	//using ComponentName = std::string;

	static const uint64_t START_ID = 0;	// EntityId,ComponentIdの始まりの識別子(最初のIDはどれも"1"から)

	class ComponentArrayBase
	{
	public:
		virtual ~ComponentArrayBase() = default;

		// 共通の更新処理
		virtual void PreCommonUpdate(float dt)	= 0;
		virtual void CommonUpdate(float dt)		= 0;
		virtual void PostCommonUpdate(float dt) = 0;
		// シーン中の更新処理
		virtual void PreSceneUpdate(float dt)	= 0;
		virtual void SceneUpdate(float dt)		= 0;
		virtual void PostSceneUpdate(float dt)	= 0;
		// ゲーム中の更新処理
		virtual void PreGameUpdate(float dt)	= 0;
		virtual void GameUpdate(float dt)		= 0;
		virtual void PostGameUpdate(float dt)	= 0;


		virtual component::ComponentBase* AddComponent(mapping::rename_type::Entity entity)	  = 0;
		//virtual void CreatePrefab(mapping::rename_type::Entity entity, asset::Prefab& prefab) = 0;
		virtual component::ComponentBase* TryGetComponent(mapping::rename_type::Entity entity)   = 0;
		virtual void Destroy(mapping::rename_type::Entity entity)							  = 0;
		virtual void Save(const std::string& filename)										  = 0;
		virtual void Load(const std::string& filename, ecs::Registry* registry)				  = 0;
		virtual void RenderImGui(mapping::rename_type::Entity entity)						  = 0;

		virtual bool HasEntity(const mapping::rename_type::Entity& entity) = 0;
		virtual size_t GetHashCode()									   = 0;
	};

	template <typename T>
	class ComponentArray final : public ComponentArrayBase
	{
	public:
		explicit ComponentArray() = default;

		[[nodiscard]]
		bool HasEntity(const mapping::rename_type::Entity& entity) override
		{
			if (entity_id.contains(entity))
				return true;
			return false;
		}

		[[nodiscard]]
		size_t GetHashCode() override
		{
			return typeid(T).hash_code();
		}

		//-- 共通の更新処理 --//
		void PreCommonUpdate(const float dt) override
		{
			for(auto& component : components)
			{
				component.PreCommonUpdate(dt);
			}
		}
		void CommonUpdate(const float dt) override
		{
			for(auto& component : components)
			{
				component.CommonUpdate(dt);
			}
		}
		void PostCommonUpdate(float dt) override
		{
			for(auto& component : components)
			{
				component.PostCommonUpdate(dt);
			}
		}
		//-- シーン更新処理 --//
		void PreSceneUpdate(const float dt) override
		{
			for (auto& component : components)
			{
				component.PreSceneUpdate(dt);
			}
		}
		void SceneUpdate(const float dt) override
		{
			for (auto& component : components)
			{
				component.SceneUpdate(dt);
			}
		}
		void PostSceneUpdate(const float dt) override
		{
			for (auto& component : components)
			{
				component.PostSceneUpdate(dt);
			}
		}
		//-- ゲーム更新処理 --//
		void PreGameUpdate(const float dt) override
		{
			for(auto& component : components)
			{
				component.PreGameUpdate(dt);
			}
		}
		void GameUpdate(const float dt) override
		{
			for (auto& component : components)
			{
				component.GameUpdate(dt);
			}
		}
		void PostGameUpdate(const float dt) override
		{
			for (auto& component : components)
			{
				component.PostGameUpdate(dt);
			}
		}

		void RenderImGui(const mapping::rename_type::Entity entity) override
		{
			if (!Content(entity))
				return;

			GetComponent(entity).RenderImGui();
		}

		/*
		 * brief : EntityのT型Componentを追加
		 *		 : 既にComponentを持っていた場合は、
		 *		 　新しく作らず持っているComponentを返す
		 */
		template <typename... Args>
		T& AddComponent(class Registry* registry, const mapping::rename_type::Entity entity, Args... args)
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

		component::ComponentBase* AddComponent(const mapping::rename_type::Entity entity) override
		{
			if (entity_id.contains(entity))
			{
				const size_t index = static_cast<size_t>(entity_id.at(entity));
				return &components.at(index);
			}

			components.emplace_back(T{});

			const size_t index = static_cast<size_t>(entity_id.emplace(entity, components.size() - 1).first->second);

			return  &components.at(index);
		}

		void RemoveComponent(const mapping::rename_type::Entity entity)
		{
			// entityがcomponentを持っていない場合は何もしない
			if (!Content(entity))
				return;

			// EntityIdの取得(componentsのインデックス番号になる)
			const mapping::rename_type::EntityId index = GetEntityID(entity);
			// EntityIdがcomponentsの最後を指していた場合はそのまま削除
			if (index == (components.size() - 1))
			{
				entity_id.erase(entity);
				components.pop_back();
				return;
			}

			// コンポーネントとコンポーネント配列の最後の要素を入れ替える
			std::iter_swap(components.begin() + static_cast<int>(index), components.end() - 1);

			mapping::rename_type::Entity key{};
			// componentsの最後の要素と同じEntityIdを持つEntityを検索
			for (auto& it : entity_id)
			{
				if (it.second == components.size() - 1)
				{
					key = it.first;
					break;
				}
			}

			// コンポーネントとインデックスを削除する
			components.pop_back();
			// もともと最後を指していたエンティティのインデックスを修正する
			entity_id.at(key) = index;
			entity_id.erase(entity);
		}

		void Destroy(const mapping::rename_type::Entity entity) override
		{
			if (!entity_id.contains(entity))
				return;

			// RegistryのRemoveComponent()と同じ処理
			RemoveComponent(entity);
		}

		/**
		 * @brief	: Entityが持つComponentを返す
		 * @details	: EntityがComponentを持っていない場合、 \n
		 *			  新しくComponentを作るが、引数なしの \n
		 *			  コンストラクタがあるクラス限定 \n
		 */
		T& GetComponent(const mapping::rename_type::Entity entity)
		{
			if (!entity_id.contains(entity))
				assert(!"Don't have component");

			const size_t index = static_cast<size_t>(entity_id.at(entity));
			return components.at(index);
		}

		component::ComponentBase* TryGetComponent(const mapping::rename_type::Entity entity) override
		{
			if (entity_id.contains(entity))
			{
				const size_t index = static_cast<size_t>(entity_id.at(entity));
				return &components.at(index);
			}

			return nullptr;
		}

		/**
		 * @brief : EntityがComponentを持っているか
		 */
		[[nodiscard]] bool Content(const mapping::rename_type::Entity entity) const
		{
			if (entity_id.contains(entity))
				return true;

			return false;
		}

		std::vector<T>& GetComponents()
		{
			return components;
		}

		/**
		 * @brief : EntityのEntityID(componentsのインデックス番号)を返す \n
		 *			EntityがComponentを持ってない場合は"-1"を返す
		 */
		mapping::rename_type::EntityId GetEntityID(mapping::rename_type::Entity entity)
		{
			if (!Content(entity))
				return -1;

			return entity_id.at(entity);
		}

		/**
		 * @brief			: componentsとentity_idの保存
		 * @param file_path	: 保存するファイルパス
		 * @details caution(1) : 引数には「"Components"」までのファイルパスが入ってくる \n
		 *						 「./パス/"Components"」
		 */
		void Save(const std::string& file_path) override
		{
			// ./Contents/Scenes/filename(引数)/Components/「T型のファイル名」までを取得(拡張子はなし) & フォルダ作成
			const std::string save_file_path{ file_path + "/" + file_path_helper::GetTypeName<T>() };
			std::filesystem::create_directories(save_file_path);

			// 保存する先のフォルダ指定 & 作成
			const std::string save_file_path_and_name{ save_file_path + "/" + file_path_helper::GetTypeName<T>() };

			{// JSONでの出力
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetJsonExtension());
				cereal::JSONOutputArchive output_archive(ofs);
				output_archive(
					CEREAL_NVP(components),
					CEREAL_NVP(entity_id)
				);
			}

			{// バイナリでの出力
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetBinExtension(), std::ios_base::binary);
				cereal::BinaryOutputArchive output_archive(ofs);
				output_archive(
					CEREAL_NVP(components),
					CEREAL_NVP(entity_id)
				);
			}
		}
		/**
		 * @brief : componentとentity_idのロード
		 */
		void Load(const std::string& filename, ecs::Registry* registry) override
		{
			// ./Contents/Scenes/filename(引数)/Components/「T型のファイル名」/「T型のファイル名」
			const std::string load_file_path_and_name{ filename + "/" +
													   file_path_helper::GetComponentsFilename() + "/" +
													   file_path_helper::GetTypeName<T>() + "/" +
													   file_path_helper::GetTypeName<T>() };

			{// バイナリファイルから取得
				std::ifstream ifs(load_file_path_and_name + file_path_helper::GetBinExtension(), std::ios_base::binary);
				if (!ifs)
					assert(!"Not open file");
				cereal::BinaryInputArchive input_archive(ifs);
				input_archive(
					CEREAL_NVP(components),
					CEREAL_NVP(entity_id)
				);
			}

			for(auto& component : components)
			{
				component.Load(registry);
			}
		}

		template<typename Archive>
		void serialize(Archive&& archive);

	private:
		std::vector<T> components;
		std::unordered_map<mapping::rename_type::Entity, mapping::rename_type::EntityId> entity_id;
	};

	class Registry final
	{
	public:
		explicit Registry()
		{
			RegisterComponentName();
		}

		//-- 共通の更新処理 --//
		void PreCommonUpdate(float dt);
		void CommonUpdate(float dt);
		void PostCommonUpdate(float dt);
		//-- シーンの更新処理 --//
		void PreSceneUpdate(float dt);
		void SceneUpdate(float dt);
		void PostSceneUpdate(float dt);
		//-- ゲームの更新処理 --//
		void PreGameUpdate(float dt);
		void GameUpdate(float dt);
		void PostGameUpdate(float dt);

		/**
		 * @brief : Entityが持つComponentの
		 *		   "RenderImGui"関数を呼ぶ
		 */
		void RenderImGui();

		/**
		 * @brief : 指定のEntityの削除
		 *          Entityに含まれるコンポーネントも削除
		 */
		void Destroy(const mapping::rename_type::Entity entity)
		{
			if (!entities.contains(entity))
				return;

			for (auto&& [component_base, array_base] : component_arrays)
			{
				array_base->Destroy(entity);
			}

			entities.erase(entity);
		}

		/**
		 * @brief : すべてのエンティティの削除
		 *          Entityに含まれるコンポーネントも削除
		 */
		void DestroyAllEntities()
		{
			for (auto& entity : entities)
			{
				for (auto&& [component_base, array_base] : component_arrays)
				{
					array_base->Destroy(entity.first);
				}
			}

			entities.clear();
		}

		template <typename T>
		[[nodiscard]] T& GetComponent(const mapping::rename_type::Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			if (!array.Content(entity))
				assert(!"Don't have Component");

			return array.GetComponent(entity);
		}

		/**
		 * @brief      : Entityが持つT型のComponentを返す
		 * ※caution  : EntityがT型を保持していない場合はnullptrを返す
		 */
		template <typename T>
		[[nodiscard]] T* TryGetComponent(const mapping::rename_type::Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			if (!array.Content(entity))
				return nullptr;

			return &array.GetComponent(entity);
		}

		/**
		 * @brief : Componentの追加
		 *          すでにComponentを所持していた場合
		 *          新しく追加せず、所持しているComponentを返す
		 */
		template <typename T, typename... Args>
		T& AddComponent(const mapping::rename_type::Entity entity, Args... args)
		{
			ComponentArray<T>& array = GetArray<T>();
			array.AddComponent(this, entity, args...);

			return array.GetComponent(entity);
		}

		/**
		 * @brief : T型のComponentのEntityを削除
		 */
		template <typename T>
		void RemoveComponent(const mapping::rename_type::Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			array.RemoveComponent(entity);
		}

		/**
		 * @brief : T型が持つComponentId(uint64_t)からComponentArrayを取得する
		 */
		template <typename T>
		ComponentArray<T>& GetArray()
		{
			//const ComponentId component_id = GetComponentId<T>();
			const std::string component_name = file_path_helper::GetTypeName<T>();

			if (!component_arrays.contains(component_name))
			{
				return  static_cast<ComponentArray<T>&>(*component_arrays.emplace(component_name, std::make_unique<ComponentArray<T>>()).first->second);
			}

			return static_cast<ComponentArray<T>&>(*component_arrays.at(component_name).get());
		}

		/**
		 * @brief : コンポーネントがあれば持っているコンポーネントを返し
		 *          なければ、コンポーネントを作る
		 */
		template<typename T, typename... Args>
		T& GetOrEmplaceComponent(mapping::rename_type::Entity entity, Args... args)
		{
			T* t = TryGetComponent<T>(entity);
			if (t)
			{
				return *t;
			}

			// 追加処理
			return AddComponent<T>(entity, args...);

		}

		[[nodiscard]]
		std::unordered_map<mapping::rename_type::Entity, std::pair<mapping::rename_type::Entity, mapping::rename_type::EntityName>>& GetEntities()
		{
			return entities;
		}

		[[nodiscard]]
		std::unordered_map<mapping::rename_type::ComponentName, std::unique_ptr<ComponentArrayBase>>& GetComponentArrays()
		{
			return component_arrays;
		}

		/**
		 * @brief : 指定されたエンティティ名が存在するか
		 * @return : true -> 存在する
		 * @return : false -> 存在しない
		 */
		[[nodiscard]]
		bool HasName(const std::string& name) const
		{
			for(const auto& entity : entities)
			{
				if (entity.second.second == name)
					return true;
			}
			return false;
		}

		[[nodiscard]]
		std::string GetName(const mapping::rename_type::Entity& ent) const
		{
			if (!entities.contains(ent))
				return "";

			return entities.at(ent).second;
		}

		/**
		 * @brief : Entityの作成
		 */
		mapping::rename_type::Entity CreateEntity();

		/**
		 * @brief        : T型のComponentArrayを登録
		 * ※caution(1)  : namespaceが含まれている場合クラス名のみ保存される
		 * ※caution(2)  :「class 」が含まれる場合もクラス名のみ保存される
		 * ※caution(3)  : 上記以外の場合はtypeid(T).name()が保存される
		 */
		template <typename T>
		void RegistryComponent()
		{
			const mapping::rename_type::ComponentName component_name = file_path_helper::GetTypeName<T>();

			if (component_arrays.contains(component_name))
				return;

			component_arrays.emplace(component_name, std::make_unique<ComponentArray<T>>());
		}

		/**
		 * @brief				: entitiesとcomponent_arraysのファイルSave用関数
		 * @param filename		: 保存する場所までのファイルパス
		 * @param scene_name	: 保存するシーン名
		 * @remark : ※caution(1)「.json」と「.bin」で書き出される
		 * @remark : ※caution(2) 拡張子の指定は必要なし
		 * @remark : ※caution(3) ファイルパスは最後「/」の必要はなし
		 */
		void Save(const std::string& filename,const std::string& scene_name);

		/**
		 * @brief : entitiesとcomponent_arraysのファイルLoad用関数
		 * @param file_path		: 「./Assets/Scenes/任意のシーン名」までのパス
		 * @remark ※caution(1)	: ファイル名のみで良い(ファイルパスなどの記述の必要なし)
		 *				例) OK	: 「ファイル名
		 *					NG	:  ./Contents/「ファイル名」
		 * @remark ※caution(2)	:	現在のentitiesとcomponent_arraysは消去される
		 * @remark ※caution(3)	:	component_arraysのキー値に型の名前が登録されていない場合
		 *							assertionが発生するためResisterComponentName関数を呼ぶか、
		 *							RegisterComponent関数で型を登録する必要あり
		 *							(ResisterComponentName関数は内部で行っているので、
		 *							assertionが発生した場合はRegisterComponentName関数を確認すれば良い)
		 */
		void Load(const std::string& file_path);

		void SetScene(scene::Scene* scene)		  { this->scene = scene; }
		[[nodiscard]] scene::Scene*   GetScene()   const { return scene; }

		template<typename Archive>
		void serialize(Archive&& archive);

	private:
		//---------- functions ----------//
		//template <typename T>
		//static ComponentId GetComponentId()
		//{
		//	static ComponentId id = typeid(T).hash_code();
		//	return id;
		//}

		/**
		 * @brief     : component_arraysのキー値を予め登録
		 * ※caution : デシリアライズの際の型の判別に使用
		 */
		void RegisterComponentName();

		/**
		 * @brief : entitiesにEntityId(uint64_t),EntityName(std::string)の登録
		 */
		void CreateEntity(mapping::rename_type::Entity ent);

		//---------- values ----------//

		std::unordered_map<mapping::rename_type::ComponentName, std::unique_ptr<ComponentArrayBase>> component_arrays;
		std::unordered_map<mapping::rename_type::Entity, std::pair<mapping::rename_type::Entity, mapping::rename_type::EntityName>> entities;
		scene::Scene* scene;
	};

	class ComponentAssetBase
	{
	public:
		explicit ComponentAssetBase() = default;
		virtual ~ComponentAssetBase() = default;
		explicit ComponentAssetBase(const ComponentAssetBase&)		= delete;
		explicit ComponentAssetBase(const ComponentAssetBase&&)		= delete;
		explicit ComponentAssetBase(ComponentAssetBase&)			= delete;
		explicit ComponentAssetBase(ComponentAssetBase&&)			= delete;
		ComponentAssetBase& operator=(const ComponentAssetBase&)	= delete;
		ComponentAssetBase& operator=(const ComponentAssetBase&&)	= delete;
		ComponentAssetBase& operator=(ComponentAssetBase&)			= delete;
		ComponentAssetBase& operator=(ComponentAssetBase&&)			= delete;

		virtual void AddComponent(ecs::Registry* registry, const mapping::rename_type::Entity& ent) = 0;
		virtual bool RegistryPrefab(
			ecs::Registry* registry,
			const mapping::rename_type::Entity& ent)	= 0;
		virtual void Save(const std::string& file_path) = 0;
		virtual void Load(const std::string& file_path) = 0;
	};

	template<class T>
	class ComponentAsset final : public ComponentAssetBase
	{
	public:
		explicit ComponentAsset() = default;
		~ComponentAsset() override = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(component_data)
			);
		}

		void AddComponent(ecs::Registry* registry,const mapping::rename_type::Entity& ent) override
		{
			auto& comp	= registry->AddComponent<T>(ent);
			comp		= component_data;
			comp.SetEntity(ent);
			comp.SetRegistry(registry);
		}

		bool RegistryPrefab(ecs::Registry* registry,const mapping::rename_type::Entity& ent) override
		{
			auto& component_arrays = registry->GetComponentArrays();
			const auto& comp_name = file_path_helper::GetTypeName<T>();
			if(!component_arrays.contains(comp_name))
				assert(!"Not registered in the component array");

			ComponentArrayBase* component = component_arrays.at(comp_name).get();
			// エンティティを持っていなければプレファブ化しない(処理を抜ける)
			if (!component->HasEntity(ent))
				return false;
			component_data = static_cast<ComponentArray<T>*>(component)->GetComponent(ent);
			return true;
		}

		/**
		 * @brief : コンポーネントの保存(シリアライズ)
		 * @param file_path : 保存したい先のファイルパス
		 *					  例 : ./Data/Components　の場合
		 *					      「Components」フォルダは作成されるので
		 *					       "./Data"までを指定する
		 */
		void Save(const std::string& file_path) override
		{
			const std::string save_file_path{ file_path + "/" + file_path_helper::GetComponentsFilename() + "/" + file_path_helper::GetTypeName<T>()};
			if(!std::filesystem::exists(save_file_path))
				std::filesystem::create_directories(save_file_path);

			// 保存する先のフォルダ指定 & 作成
			const std::string save_file_path_and_name{ save_file_path + "/" + file_path_helper::GetTypeName<T>()};

			{// JSONでの出力
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetJsonExtension());
				if (!ofs)
					assert(!"Not open file");
				cereal::JSONOutputArchive output_archive(ofs);
				output_archive(*this);
			}

			{// バイナリでの出力
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetBinExtension(), std::ios_base::binary);
				if (!ofs)
					assert(!"Not open file");
				cereal::BinaryOutputArchive output_archive(ofs);
				output_archive(*this);
			}
		}

		/**
		 * @brief : コンポーネントデータの取得(デシリアライズ)
		 * @param file_path : 取得したいコンポーネントの「Components」までのファイルパス
		 *					  例 : "./Data/Componentsなら
		 *						   "./Data"を指定
		 */
		void Load(const std::string& file_path) override
		{
			const std::string load_file_path_and_name{ file_path + "/" +
													   file_path_helper::GetComponentsFilename() + "/" +
												       file_path_helper::GetTypeName<T>() + "/" +
												       file_path_helper::GetTypeName<T>() };

			{// バイナリファイルから取得
				std::ifstream ifs(load_file_path_and_name + file_path_helper::GetBinExtension(), std::ios_base::binary);
				if (!ifs)
					assert(!"Not open file");
				cereal::BinaryInputArchive input_archive(ifs);
				input_archive(*this);
			}
		}

		const T& GetComponentData() const { return component_data; }
		void SetComponentData(const T& data) { component_data = data; }

	private:
		T component_data;
	};
} // cumulonimbus::ecs