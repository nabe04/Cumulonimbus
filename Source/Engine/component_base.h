#pragma once
#include <string>
#include <map>

#include "rename_type_mapping.h"
#include "component_tag_mapping.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::component
{
	class ComponentBase
	{
	public:
		explicit ComponentBase() = default;
		explicit ComponentBase(const mapping::component_tag::ComponentTag tag) { component_tag = tag; }	// �C���X�y�N�^�[�r���[�p
		explicit ComponentBase(ecs::Registry* registry, mapping::rename_type::Entity ent)
			: registry{ registry }
			, entity{ ent } {}
		ComponentBase(const ComponentBase& other)
			:registry(other.registry),
		     entity(other.entity) {}
		ComponentBase& operator=(const ComponentBase& other)
		{
			if(this == &other)
			{
				return *this;
			}

			registry = other.registry;
			entity   = other.entity;

			return *this;
		}
		virtual ~ComponentBase() = default;

		/**
		 * @brief : �C���X�y�N�^�[�r���[�ȂǃR���|�[�l���g��
		 *			�ǉ����Ɉ������w��ł��Ȃ��ꍇ�Ɏg�p
		 */
		virtual void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) {}

		/**
		 * @brief : �G���e�B�e�B�̏��L�����ύX���ꂽ���Ɏg�p
		 *		    ��)�v���t�@�u����I�u�W�F�N�g���쐬���鎞
		 * @param registry :
		 * @param connector : �̂̃G���e�B�e�B�ƍ��̃G���e�B�e�B�Ƃ��q������(�R�l�N�^�[)
		 */
		virtual void OnDeserialize(
			ecs::Registry* registry,
			const std::map<mapping::rename_type::Entity, mapping::rename_type::Entity>& connector) {}

		virtual void PreCommonUpdate(float dt) {}
		virtual void CommonUpdate(float dt) {}
		virtual void PostCommonUpdate(float dt) {}

		virtual void PreSceneUpdate(float dt) {}
		virtual void SceneUpdate(float dt) {}
		virtual void PostSceneUpdate(float dt) {}

		virtual void PreGameUpdate(float dt) {}
		virtual void GameUpdate(float dt) {}
		virtual void PostGameUpdate(float dt) {}

		virtual void RenderImGui() {}
		virtual void Load(ecs::Registry* registry) = 0;

		template<class Archive>
		void load(Archive&& archive, uint32_t version)
		{
			archive(
				CEREAL_NVP(entity)
			);
		}

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const
		{
			archive(
				CEREAL_NVP(entity)
			);
		}

		[[nodiscard]] ecs::Registry* GetRegistry() const { return registry; }
		[[nodiscard]] mapping::rename_type::Entity GetEntity() const { return entity; }
		[[nodiscard]] mapping::component_tag::ComponentTag GetComponentTag() const { return component_tag; }

		void SetRegistry(ecs::Registry* registry) { this->registry = registry; }
		void SetEntity(const mapping::rename_type::Entity ent) { entity = ent; }

	protected:
		mapping::component_tag::ComponentTag component_tag{ mapping::component_tag::ComponentTag::NoTag };
		/**
		 * @brief : �e�X�̃R���|�[�l���g��ImGui�ŕ\������ۂ�
		 *			���j���[�\���p�̃{�^����ǉ�����
		 */
		void MenuPopup(
			const mapping::rename_type::Entity& ent,
			bool& is_removed_component) const;

	private:
		ecs::Registry* registry{};
		mapping::rename_type::Entity entity{};
	};
} // cumulonimbus::component

CEREAL_CLASS_VERSION(cumulonimbus::component::ComponentBase, 0)