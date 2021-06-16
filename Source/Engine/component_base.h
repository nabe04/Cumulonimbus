#pragma once
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include "ecs.h"

namespace cumulonimbus::component
{
	class ComponentBase
	{
	public:
		ComponentBase() = default;
		ComponentBase(ecs::Registry* registry, ecs::Entity ent)
			: registry{ registry }
			, entity{ ent }
		{
		}
		virtual ~ComponentBase() {}

		virtual void NewFrame(float dt) = 0;
		virtual void Update(float dt) = 0;
		virtual void RenderImGui() = 0;
		virtual void Save(const std::string& file_path) = 0;
		virtual void Load(const std::string& file_path_and_name) = 0;

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(entity)
			);
		}

		[[nodiscard]] ecs::Registry* GetRegistry() const { return registry; }
		[[nodiscard]] ecs::Entity    GetEntity()   const { return entity; }

	private:
		ecs::Registry* registry{};
		ecs::Entity entity;
	};
}