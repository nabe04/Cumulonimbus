#pragma once
#include "rename_type_mapping.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus

namespace cumulonimbus::editor
{
	class Hierarchy final
	{
	public:
		void Render(ecs::Registry* registry);
		[[nodiscard]] mapping::rename_type::Entity GetSelectedEntity() const { return selected_entity; }
	private:
		mapping::rename_type::Entity selected_entity{};
	};
} // cumulonimbus::editor