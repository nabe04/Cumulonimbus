#pragma once
#include <string>

#include <cereal/cereal.hpp>

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

		virtual void NewFrame(float dt)	  {}
		virtual void Update(float dt)	  {}
		virtual void PostUpdate(float dt) {}
		virtual void RenderImGui()		  {}
		virtual void Save(const std::string& file_path)			 = 0;
		virtual void Load(const std::string& file_path_and_name) = 0;

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