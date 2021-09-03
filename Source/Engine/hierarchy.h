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
	private:
		mapping::rename_type::Entity selected_entity{};
	};
} // cumulonimbus::editor