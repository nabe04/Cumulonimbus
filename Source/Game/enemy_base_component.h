#pragma once
#include "actor3d_component.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::component
{
	class EnemyBaseComponent : public Actor3DComponent
	{
	public:
		explicit EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemyBaseComponent()  = default;
		~EnemyBaseComponent() override = default;

		void Update(float dt) override {}
		void RenderImGui()	  override {}
		void Save(const std::string& file_path)			 override {}
		void Load(const std::string& file_path_and_name) override {}
	};
} // cumulonimbus::component
