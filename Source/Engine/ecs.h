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

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

#include "file_path_helper.h"

class Scene;


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
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	// Entityの識別子
	using ComponentId	= uint64_t;	// Componentの識別子
	using ComponentName = std::string;

	static const uint64_t START_ID = 1;	// EntityId,ComponentIdの始まりの識別子(最初のIDはどれも"1"から)

	class ComponentArrayBase
	{
	public:
		virtual ~ComponentArrayBase() = default;

		virtual void PreUpdate(float dt) = 0;
		virtual void Update(float dt) = 0;
		virtual void Destroy(Entity entity) = 0;
		virtual void RenderImGui(Entity entity) = 0;
		virtual void Save(const std::string& filename) = 0;
		virtual void Load(const std::string& filename) = 0;
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

		void PreUpdate(float dt) override
		{
			for(auto& component : components)
			{
				component.NewFrame(dt);
			}
		}

		void Update(float dt) override
		{
			for (auto& component : components)
			{
				component.Update(dt);
			}
		}

		void RenderImGui(Entity entity) override
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

			Entity key{ static_cast<Entity>(0) };
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
		[[nodiscard]] bool Content(const Entity entity)
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

		/*
		 * brief    : componentsとentity_idの保存
		 * filename : 保存するファイル名(パス、拡張子などなし)
		 */
		void Save(const std::string& filename) override
		{
			// ./Contents/Scenes/filename(引数)/Components/「T型のファイル名」までを取得(拡張子はなし) & フォルダ作成
			const std::string save_file_path{ file_path_helper::AttachComponentsDirectory(filename) + "/" + file_path_helper::GetTypeName<T>() };
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

		/*
		 * brief : componentとentity_idのロード
		 *
		 */
		void Load(const std::string& filename) override
		{
			// ./Contents/Scenes/filename(引数)//Components/「T型のファイル名」/「T型のファイル名」
			const std::string load_file_path_and_name{ file_path_helper::AttachComponentsDirectory(filename) + "/" +
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
		Registry()
		{
			RegisterComponentName();
		}

		/*
		 * brief : Component全体のPreUpdate処理
		 */
		void PreUpdate(float dt);

		/*
		 * brief : Component全体のUpdate処理
		 */
		void Update(float dt);

		/*
		 * brief : Entityが持つComponentの
		 *		   "RenderImGui"関数を呼ぶ
		 */
		void RenderImGui();

		/*
		 * brief : 指定のEntityの削除
		 *         Entityに含まれるコンポーネントも削除
		 */
		void Destroy(Entity entity)
		{
			if (!entities.contains(entity))
				return;

			for (auto&& [component_base, array_base] : component_arrays)
			{
				array_base->Destroy(entity);
			}

			entities.erase(entity);
		}

		/*
		 * brief : すべてのエンティティの削除
		 *         Entityに含まれるコンポーネントも削除
		 */
		void DestroyAllEntities()
		{
			for (auto& entity : entities)
			{
				for (auto&& [component_base, array_base] : component_arrays)
				{
					array_base->Destroy(entity.second);
				}
			}

			entities.clear();
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
			//const ComponentId component_id = GetComponentId<T>();
			const std::string component_name = file_path_helper::GetTypeName<T>();

			if (!component_arrays.contains(component_name))
			{
				return  static_cast<ComponentArray<T>&>(*component_arrays.emplace(component_name, std::make_unique<ComponentArray<T>>()).first->second);
			}

			return static_cast<ComponentArray<T>&>(*component_arrays.at(component_name).get());
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

		[[nodiscard]] const std::unordered_map<Entity, Entity>& GetEntities()
		{
			return entities;
		}

		/*
		 * brief : Entityの作成
		 */
		Entity CreateEntity();

		/*
		 * brief        : T型のComponentArrayを登録
		 * ※caution(1) : namespaceが含まれている場合クラス名のみ保存される
		 * ※caution(2) :「class 」が含まれる場合もクラス名のみ保存される
		 * ※caution(3) : 上記以外の場合はtypeid(T).name()が保存される
		 */
		template <typename T>
		void RegistryComponent()
		{
			const ComponentName component_name = file_path_helper::GetTypeName<T>();

			if (component_arrays.contains(component_name))
				return;

			component_arrays.emplace(component_name, std::make_unique<ComponentArray<T>>());
		}

		/*
		 * brief        : entitiesとcomponent_arraysのファイルSave用関数
		 *                「.json」と「.bin」で書き出される
		 *				  ./Contents/「ファイル名」に出力される
		 * ※caution(1) : ファイル名のみで良い(ファイルパスなどの記述の必要なし)
		 *				  例) OK : 「ファイル名」
		 *				      NG :  ./Contents/「ファイル名」
		 * ※caution(2) : 拡張などの指定は必要ない
		 * ※caution(3) : 同じファイル名の場合上書きされてしまうため注意
		 */
		void Save(const std::string& filename);

		/*
		 * brief		: entitiesとcomponent_arraysのファイルLoad用関数
		 * ※caution(1) : ファイル名のみで良い(ファイルパスなどの記述の必要なし)
		 *				  例) OK : 「ファイル名」
		 *				      NG :  ./Contents/「ファイル名」
		 * ※caution(2) : 現在のentitiesとcomponent_arraysは消去される
		 * ※caution(3) : component_arraysのキー値に型の名前が登録されていない場合
		 *				  assertionが発生するためResisterComponentName関数を呼ぶか、
		 *			      RegisterComponent関数で型を登録する必要あり
		 *			      (ResisterComponentName関数は内部で行っているので、
		 *			      assertionが発生した場合はRegisterComponentName関数を確認すれば良い)
		 */
		void Load(const std::string& filename);

		void SetScene(Scene* scene)		  { this->scene = scene; }
		[[nodiscard]] Scene*   GetScene()   const { return scene; }

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(entities)
			);
		}

	private:
		//---------- functions ----------//
		//template <typename T>
		//static ComponentId GetComponentId()
		//{
		//	static ComponentId id = typeid(T).hash_code();
		//	return id;
		//}

		/*
		 * brief     : component_arraysのキー値を予め登録
		 * ※caution : デシリアライズの際の型の判別に使用
		 */
		void RegisterComponentName();
		//---------- values ----------//

		std::unordered_map<ComponentName, std::unique_ptr<ComponentArrayBase>> component_arrays;
		std::unordered_map<Entity, Entity> entities;
		Scene* scene;
	};
}
