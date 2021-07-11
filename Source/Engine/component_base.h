#pragma once
#include <string>

#include <cereal/cereal.hpp>

#include "rename_type_mapping.h"

namespace cumulonimbus::ecs
{
	class Registry;
}

namespace cumulonimbus::component
{
	class ComponentBase
	{
	public:
		explicit ComponentBase() = default;
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
		[[nodiscard]] mapping::rename_type::Entity    GetEntity()   const { return entity; }

	private:
		ecs::Registry* registry{};
		mapping::rename_type::Entity entity;
	};
}