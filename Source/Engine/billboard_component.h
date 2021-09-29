#pragma once
#include "component_base.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::component
{
	class BillboardComponent final : public ComponentBase
	{
	public:
		BillboardComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		BillboardComponent()  = default; // for cereal
		~BillboardComponent() = default;

	};
} // cumulonimbus::component
