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
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#include "file_path_helper.h"


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
	explicit Entity(class Scene* scene, UpdateOrder update_order = UpdateOrder::Default, const EntityTag entity_tag = EntityTag::Default);
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
	 * brief : Entity��Save
	 * file_path : �ۑ��������t�@�C���p�X(�Ō�Ɂu/�v�͕t���Ȃ�)
	 */
	void Save(std::string file_path);

	/*
	 * brief : Entity��Load(���Component��Load)
	 * file_path : �ǂݍ��݂����t�@�C���p�X(�Ō�Ɂu/�v�͕t���Ȃ�)
	 */
	void Load(std::string file_path);

	void  SetScene(Scene* scene) { this->scene = scene; }
	auto* GetScene()		 const { return scene; }
	auto  GetUpdateOrder() const { return update_order; }

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
			CEREAL_NVP(components),	// ������������̂Ŏ������Ă��Ȃ�
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
		:entity{entity} {}
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
	auto* GetEntity()  const { return entity; }
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


namespace cumulonimbus::ecs
{
	enum class Entity : uint64_t {};
	using EntityId = uint64_t;	// Entity�̎��ʎq
	using ComponentId = uint64_t;	// Component�̎��ʎq
	using ComponentName = std::string;

	static const uint64_t START_ID = 1;	// EntityId,ComponentId�̎n�܂�̎��ʎq(�ŏ���ID�͂ǂ��"1"����)

	class ComponentArrayBase
	{
	public:
		virtual ~ComponentArrayBase() = default;

		virtual void Destroy(Entity entity) = 0;
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

		/*
		 * brief : Entity��T�^Component��ǉ�
		 *		 : ����Component�������Ă����ꍇ�́A
		 *		 �@�V������炸�����Ă���Component��Ԃ�
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
			// entity��component�������Ă��Ȃ��ꍇ�͉������Ȃ�
			if (!Content(entity))
				return;

			// EntityId�̎擾(components�̃C���f�b�N�X�ԍ��ɂȂ�)
			const EntityId index = GetEntityID(entity);
			// EntityId��components�̍Ō���w���Ă����ꍇ�͂��̂܂܍폜
			if (index == (components.size() - 1))
			{
				entity_id.erase(entity);
				components.pop_back();
				return;
			}

			// �R���|�[�l���g�ƃR���|�[�l���g�z��̍Ō�̗v�f�����ւ���
			std::iter_swap(components.begin() + static_cast<int>(index), components.end() - 1);

			Entity key{ static_cast<Entity>(0) };
			// components�̍Ō�̗v�f�Ɠ���EntityId������Entity������
			for (auto& it : entity_id)
			{
				if (it.second == components.size() - 1)
				{
					key = it.first;
					break;
				}
			}

			// �R���|�[�l���g�ƃC���f�b�N�X���폜����
			components.pop_back();
			// ���Ƃ��ƍŌ���w���Ă����G���e�B�e�B�̃C���f�b�N�X���C������
			entity_id.at(key) = index;
			entity_id.erase(entity);
		}

		void Destroy(const Entity entity) override
		{
			if (!entity_id.contains(entity))
				return;

			// Registry��RemoveComponent()�Ɠ�������
			RemoveComponent(entity);
		}

		/*
		 *   brief	 : Entity������Component��Ԃ�
		 * ��caution : Entity��Component�������Ă��Ȃ��ꍇ�A
		 *			�@ �V����Component����邪�A�����Ȃ���
		 *			   �R���X�g���N�^������N���X����
		 */
		T& GetComponent(const Entity entity)
		{
			if (!entity_id.contains(entity))
				assert(!"Don't have component");

			const size_t index = static_cast<size_t>(entity_id.at(entity));
			return components.at(index);
		}

		/*
		 *  brief : Entity��Component�������Ă��邩
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
		 * brief : Entity��EntityID(components�̃C���f�b�N�X�ԍ�)��Ԃ�
		 *		 : Entity��Component�������ĂȂ��ꍇ��"-1"��Ԃ�
		 */
		EntityId GetEntityID(Entity entity)
		{
			if (!Content(entity))
				return -1;

			return entity_id.at(entity);
		}

		/*
		 * brief    : components��entity_id�̕ۑ�
		 * filename : �ۑ�����t�@�C����(�p�X�A�g���q�ȂǂȂ�)
		 */
		void Save(const std::string& filename) override
		{
			// ./Contents/Scenes/filename(����)/Components/�uT�^�̃t�@�C�����v�܂ł��擾(�g���q�͂Ȃ�) & �t�H���_�쐬
			const std::string save_file_path{ file_path_helper::AttachComponentsDirectory(filename) + "/" + file_path_helper::GetTypeName<T>() };
			std::filesystem::create_directories(save_file_path);

			// �ۑ������̃t�H���_�w�� & �쐬
			const std::string save_file_path_and_name{ save_file_path + "/" + file_path_helper::GetTypeName<T>() };

			{// JSON�ł̏o��
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetJsonExtension());
				cereal::JSONOutputArchive output_archive(ofs);
				output_archive(
					CEREAL_NVP(components),
					CEREAL_NVP(entity_id)
				);
			}

			{// �o�C�i���ł̏o��
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetBinExtension(), std::ios_base::binary);
				cereal::BinaryOutputArchive output_archive(ofs);
				output_archive(
					CEREAL_NVP(components),
					CEREAL_NVP(entity_id)
				);
			}
		}

		/*
		 * brief : component��entity_id�̃��[�h
		 *
		 */
		void Load(const std::string& filename) override
		{
			// ./Contents/Scenes/filename(����)//Components/�uT�^�̃t�@�C�����v/�uT�^�̃t�@�C�����v
			const std::string load_file_path_and_name{ file_path_helper::AttachComponentsDirectory(filename) + "/" +
													   file_path_helper::GetTypeName<T>() + "/" +
													   file_path_helper::GetTypeName<T>() };

			{// �o�C�i���t�@�C������擾
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
		 * brief : �w���Entity�̍폜
		 *         Entity�Ɋ܂܂��R���|�[�l���g���폜
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
		 * brief : ���ׂẴG���e�B�e�B�̍폜
		 *         Entity�Ɋ܂܂��R���|�[�l���g���폜
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
		 * brief      : Entity������T�^��Component��Ԃ�
		 * ��caution  : Entity��T�^��ێ����Ă��Ȃ��ꍇ��nullptr��Ԃ�
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
		 * brief : T�^��Component��Entity���폜
		 */
		template <typename T>
		void RemoveComponent(const Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			array.RemoveComponent(entity);
		}

		/*
		 * brief : T�^������ComponentId(uint64_t)����ComponentArray���擾����
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
		 * brief : �R���|�[�l���g������Ύ����Ă���R���|�[�l���g��Ԃ�
		 *         �Ȃ���΁A�R���|�[�l���g�����
		 */
		template<typename T, typename... Args>
		T& GetOrEmplaceComponent(Entity entity, Args... args)
		{
			T* t = TryGetComponent<T>(entity);
			if (t)
			{
				return *t;
			}

			// �ǉ�����
			return AddComponent<T>(entity, args...);

		}

		[[nodiscard]] const std::unordered_map<Entity, Entity>& GetEntities()
		{
			return entities;
		}

		/*
		 * brief : Entity�̍쐬
		 */
		Entity CreateEntity()
		{
			static uint64_t entity = START_ID;

			// ��u�C���N�������g�����Ă���̂�START_ID�����߂̗v�f�ɃZ�b�g����������
			while (entities.contains(static_cast<Entity>(entity)))
			{
				entity++;
			}

			entities.emplace(static_cast<Entity>(entity), static_cast<Entity>(entity));
			return entities.at(static_cast<Entity>(entity));
		}

		/*
		 * brief        : T�^��ComponentArray��o�^
		 * ��caution(1) : namespace���܂܂�Ă���ꍇ�N���X���̂ݕۑ������
		 * ��caution(2) :�uclass �v���܂܂��ꍇ���N���X���̂ݕۑ������
		 * ��caution(3) : ��L�ȊO�̏ꍇ��typeid(T).name()���ۑ������
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
		 * brief        : entities��component_arrays�̃t�@�C��Save�p�֐�
		 *                �u.json�v�Ɓu.bin�v�ŏ����o�����
		 *				  ./Contents/�u�t�@�C�����v�ɏo�͂����
		 * ��caution(1) : �t�@�C�����݂̂ŗǂ�(�t�@�C���p�X�Ȃǂ̋L�q�̕K�v�Ȃ�)
		 *				  ��) OK : �u�t�@�C�����v
		 *				      NG :  ./Contents/�u�t�@�C�����v
		 * ��caution(2) : �g���Ȃǂ̎w��͕K�v�Ȃ�
		 * ��caution(3) : �����t�@�C�����̏ꍇ�㏑������Ă��܂����ߒ���
		 */
		void Save(const std::string& filename)
		{
			// �t�@�C�����쐬
			// ./Contents/�u�t�@�C�����v�܂ł̎擾(�g���q�͂Ȃ�)
			const std::string save_file_path = file_path_helper::AttachSceneDirectory(filename);
			std::filesystem::create_directories(save_file_path);
			// �ۑ������̃t�@�C�����w��
			const std::string save_file_path_and_name{ save_file_path + "/" + filename };

			{
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetJsonExtension());
				cereal::JSONOutputArchive output_archive(ofs);
				output_archive(
					CEREAL_NVP(entities)
				);
			}

			{
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetBinExtension(), std::ios_base::binary);
				cereal::BinaryOutputArchive output_archive(ofs);
				output_archive(
					CEREAL_NVP(entities)
				);
			}

			for (auto&& [component_name, component_array] : component_arrays)
			{
				component_array->Save(filename);
			}
		}

		/*
		 * brief		: entities��component_arrays�̃t�@�C��Load�p�֐�
		 * ��caution(1) : �t�@�C�����݂̂ŗǂ�(�t�@�C���p�X�Ȃǂ̋L�q�̕K�v�Ȃ�)
		 *				  ��) OK : �u�t�@�C�����v
		 *				      NG :  ./Contents/�u�t�@�C�����v
		 * ��caution(2) : ���݂�entities��component_arrays�͏��������
		 * ��caution(3) : component_arrays�̃L�[�l�Ɍ^�̖��O���o�^����Ă��Ȃ��ꍇ
		 *				  assertion���������邽��ResisterComponentName�֐����ĂԂ��A
		 *			      RegisterComponent�֐��Ō^��o�^����K�v����
		 *			      (ResisterComponentName�֐��͓����ōs���Ă���̂ŁA
		 *			      assertion�����������ꍇ��RegisterComponentName�֐����m�F����Ηǂ�)
		 */
		void Load(const std::string& filename)
		{
			// entities�̍폜
			if (!entities.empty())
			{
				DestroyAllEntities();
			}
			component_arrays.clear();


			{
				// ���[�h�����̃t�@�C�����w��
				// ./Contents/Scenes/�u�t�@�C�����v/�u�t�@�C�����v.bin
				std::ifstream ifs(file_path_helper::AttachSceneDirectory(filename) + "/" + filename + file_path_helper::GetBinExtension(), std::ios_base::binary);
				if (!ifs)
					assert(!"Not open file");
				cereal::BinaryInputArchive input_archive(ifs);
				input_archive(
					CEREAL_NVP(entities)
				);
			}

			// component_arrays�̌^�̍Đݒ�
			RegisterComponentName();
			// component_array�̃��[�h
			for (auto& it : component_arrays)
			{
				it.second->Load(filename);
			}
		}

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
		 * brief     : component_arrays�̃L�[�l��\�ߓo�^
		 * ��caution : �f�V���A���C�Y�̍ۂ̌^�̔��ʂɎg�p
		 */
		void RegisterComponentName();
		//---------- values ----------//

		std::unordered_map<ComponentName, std::unique_ptr<ComponentArrayBase>> component_arrays;
		std::unordered_map<Entity, Entity> entities;
		Scene* scene;
	};
}
