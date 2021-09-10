#pragma once

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus


namespace cumulonimbus::editor
{
	class ProjectView final
	{
	public:
		void Render(ecs::Registry* registry);
	private:

	};
} // cumulonimbus::editor