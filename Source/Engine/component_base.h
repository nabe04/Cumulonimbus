#pragma once
#include <string>

//#include <cereal/cereal.hpp>

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
		explicit ComponentBase(const mapping::component_tag::ComponentTag tag) { component_tag = tag; }	// インスペクタービュー用
		explicit ComponentBase(ecs::Registry* registry, mapping::rename_type::Entity ent)
			: registry{ registry }
			, entity{ ent }
		{
		}
		virtual ~ComponentBase() = default;

		/**
		 * @brief : インスペクタービューなどコンポーネントの
		 *			追加時に引数を指定できない場合に使用
		 */
		virtual void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) {}

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

		template<typename Archive>
		void serialize(Archive&& archive)
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

	private:
		ecs::Registry* registry{};
		mapping::rename_type::Entity entity{};
	};
} // cumulonimbus::component