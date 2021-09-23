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
	//using EntityId		= uint64_t;	// Entity�̎��ʎq
	//using EntityName	= std::string;
	//using ComponentId	= uint64_t;	// Component�̎��ʎq
	//using ComponentName = std::string;

	static const uint64_t START_ID = 0;	// EntityId,ComponentId�̎n�܂�̎��ʎq(�ŏ���ID�͂ǂ��"1"����)

	class ComponentArrayBase
	{
	public:
		virtual ~ComponentArrayBase() = default;

		virtual void PreUpdate(float dt)	= 0;
		virtual void Update(float dt)		= 0;
		virtual void PostUpdate(float dt)	= 0;
		virtual void Destroy(mapping::rename_type::Entity entity)	  = 0;
		virtual void RenderImGui(mapping::rename_type::Entity entity) = 0;
		virtual void Save(const std::string& filename) = 0;
		virtual void Load(const std::string& filename, ecs::Registry* registry) = 0;
		virtual size_t GetHashCode() = 0;
		virtual component::ComponentBase* AddComponentFromInspector(mapping::rename_type::Entity entity) = 0;

		//component::ComponentBase* base;
	};

	template <typename T>
	class ComponentArray final : public ComponentArrayBase
	{
	public:
		explicit ComponentArray() = default;

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

		void PostUpdate(float dt) override
		{
			for (auto& component : components)
			{
				component.PostUpdate(dt);
			}
		}

		void RenderImGui(mapping::rename_type::Entity entity) override
		{
			if (!Content(entity))
				return;

			GetComponent(entity).RenderImGui();
		}

		/*
		 * brief : Entity��T�^Component��ǉ�
		 *		 : ����Component�������Ă����ꍇ�́A
		 *		 �@�V������炸�����Ă���Component��Ԃ�
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

		component::ComponentBase* AddComponentFromInspector(const mapping::rename_type::Entity entity) override
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

		void RemoveComponent(mapping::rename_type::Entity entity)
		{
			// entity��component�������Ă��Ȃ��ꍇ�͉������Ȃ�
			if (!Content(entity))
				return;

			// EntityId�̎擾(components�̃C���f�b�N�X�ԍ��ɂȂ�)
			const mapping::rename_type::EntityId index = GetEntityID(entity);
			// EntityId��components�̍Ō���w���Ă����ꍇ�͂��̂܂܍폜
			if (index == (components.size() - 1))
			{
				entity_id.erase(entity);
				components.pop_back();
				return;
			}

			// �R���|�[�l���g�ƃR���|�[�l���g�z��̍Ō�̗v�f�����ւ���
			std::iter_swap(components.begin() + static_cast<int>(index), components.end() - 1);

			mapping::rename_type::Entity key{};
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

		void Destroy(const mapping::rename_type::Entity entity) override
		{
			if (!entity_id.contains(entity))
				return;

			// Registry��RemoveComponent()�Ɠ�������
			RemoveComponent(entity);
		}

		/**
		 * @brief	: Entity������Component��Ԃ�
		 * @details	: Entity��Component�������Ă��Ȃ��ꍇ�A \n
		 *			  �V����Component����邪�A�����Ȃ��� \n
		 *			  �R���X�g���N�^������N���X���� \n
		 */
		T& GetComponent(const mapping::rename_type::Entity entity)
		{
			if (!entity_id.contains(entity))
				assert(!"Don't have component");

			const size_t index = static_cast<size_t>(entity_id.at(entity));
			return components.at(index);
		}

		/**
		 * @brief : Entity��Component�������Ă��邩
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
		 * @brief : Entity��EntityID(components�̃C���f�b�N�X�ԍ�)��Ԃ� \n
		 *			Entity��Component�������ĂȂ��ꍇ��"-1"��Ԃ�
		 */
		mapping::rename_type::EntityId GetEntityID(mapping::rename_type::Entity entity)
		{
			if (!Content(entity))
				return -1;

			return entity_id.at(entity);
		}

		/**
		 * @brief			: components��entity_id�̕ۑ�
		 * @param file_path	: �ۑ�����t�@�C���p�X
		 * @details caution(1) : �����ɂ́u"Components"�v�܂ł̃t�@�C���p�X�������Ă��� \n
		 *						 �u./�p�X/"Components"�v
		 */
		void Save(const std::string& file_path) override
		{
			// ./Contents/Scenes/filename(����)/Components/�uT�^�̃t�@�C�����v�܂ł��擾(�g���q�͂Ȃ�) & �t�H���_�쐬
			const std::string save_file_path{ file_path + "/" + file_path_helper::GetTypeName<T>() };
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
		/**
		 * @brief : component��entity_id�̃��[�h
		 */
		void Load(const std::string& filename, ecs::Registry* registry) override
		{
			// ./Contents/Scenes/filename(����)/Components/�uT�^�̃t�@�C�����v/�uT�^�̃t�@�C�����v
			const std::string load_file_path_and_name{ filename + "/" +
													   file_path_helper::GetComponentsFilename() + "/" +
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

		/*
		 * brief : Component�S�̂�PreUpdate����
		 */
		void PreUpdate(float dt);

		/*
		 * brief : Component�S�̂�Update����
		 */
		void Update(float dt);

		/*
		 * brief : Component�S�̂�PostUpdate����
		 */
		void PostUpdate(float dt);

		/*
		 * brief : Entity������Component��
		 *		   "RenderImGui"�֐����Ă�
		 */
		void RenderImGui();

		/*
		 * brief : �w���Entity�̍폜
		 *         Entity�Ɋ܂܂��R���|�[�l���g���폜
		 */
		void Destroy(mapping::rename_type::Entity entity)
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

		/*
		 * brief      : Entity������T�^��Component��Ԃ�
		 * ��caution  : Entity��T�^��ێ����Ă��Ȃ��ꍇ��nullptr��Ԃ�
		 */
		template <typename T>
		[[nodiscard]] T* TryGetComponent(const mapping::rename_type::Entity entity)
		{
			ComponentArray<T>& array = GetArray<T>();
			if (!array.Content(entity))
				return nullptr;

			return &array.GetComponent(entity);
		}

		/*
		 * brief : Component�̒ǉ�
		 *         ���ł�Component���������Ă����ꍇ
		 *         �V�����ǉ������A�������Ă���Component��Ԃ�
		 */
		template <typename T, typename... Args>
		T& AddComponent(const mapping::rename_type::Entity entity, Args... args)
		{
			ComponentArray<T>& array = GetArray<T>();
			array.AddComponent(this, entity, args...);

			return array.GetComponent(entity);
		}

		/*
		 * brief : T�^��Component��Entity���폜
		 */
		template <typename T>
		void RemoveComponent(const mapping::rename_type::Entity entity)
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
		T& GetOrEmplaceComponent(mapping::rename_type::Entity entity, Args... args)
		{
			T* t = TryGetComponent<T>(entity);
			if (t)
			{
				return *t;
			}

			// �ǉ�����
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
		 * @brief : �w�肳�ꂽ�G���e�B�e�B�������݂��邩
		 * @return : true -> ���݂���
		 * @return : false -> ���݂��Ȃ�
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

		/*
		 * brief : Entity�̍쐬
		 */
		mapping::rename_type::Entity CreateEntity();

		/*
		 * brief        : T�^��ComponentArray��o�^
		 * ��caution(1) : namespace���܂܂�Ă���ꍇ�N���X���̂ݕۑ������
		 * ��caution(2) :�uclass �v���܂܂��ꍇ���N���X���̂ݕۑ������
		 * ��caution(3) : ��L�ȊO�̏ꍇ��typeid(T).name()���ۑ������
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
		 * @brief				: entities��component_arrays�̃t�@�C��Save�p�֐�
		 * @param filename		: �ۑ�����ꏊ�܂ł̃t�@�C���p�X
		 * @param scene_name	: �ۑ�����V�[����
		 * @details	��caution(1) :�u.json�v�Ɓu.bin�v�ŏ����o�����\n
		 *			��caution(2) : �g���q�̎w��͕K�v�Ȃ� \n
		 *			��caution(3) : �t�@�C���p�X�͍Ō�u/�v�̕K�v�͂Ȃ�
		 */
		void Save(const std::string& filename,const std::string& scene_name);

		/**
		 * @brief					: entities��component_arrays�̃t�@�C��Load�p�֐�
		 * @param file_path			: �u./Assets/Scenes/�C�ӂ̃V�[�����v�܂ł̃p�X
		 * @details ��caution(1)	: �t�@�C�����݂̂ŗǂ�(�t�@�C���p�X�Ȃǂ̋L�q�̕K�v�Ȃ�) \n
		 *					��) OK	: �u�t�@�C���� \n
		 *						NG	:  ./Contents/�u�t�@�C�����v\n
		 * @details ��caution(2)	:	���݂�entities��component_arrays�͏��������
		 * @details ��caution(3)	:	component_arrays�̃L�[�l�Ɍ^�̖��O���o�^����Ă��Ȃ��ꍇ \n
		 *								assertion���������邽��ResisterComponentName�֐����ĂԂ��A \n
		 *								RegisterComponent�֐��Ō^��o�^����K�v���� \n
		 *								(ResisterComponentName�֐��͓����ōs���Ă���̂ŁA \n
		 *								assertion�����������ꍇ��RegisterComponentName�֐����m�F����Ηǂ�)
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

		/*
		 * brief     : component_arrays�̃L�[�l��\�ߓo�^
		 * ��caution : �f�V���A���C�Y�̍ۂ̌^�̔��ʂɎg�p
		 */
		void RegisterComponentName();

		/*
		 * brief : entities��EntityId(uint64_t),EntityName(std::string)�̓o�^
		 */
		void CreateEntity(mapping::rename_type::Entity ent);

		//---------- values ----------//

		std::unordered_map<mapping::rename_type::ComponentName, std::unique_ptr<ComponentArrayBase>> component_arrays;
		std::unordered_map<mapping::rename_type::Entity, std::pair<mapping::rename_type::Entity, mapping::rename_type::EntityName>> entities;
		scene::Scene* scene;
	};
} // cumulonimbus::ecs
