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
	class SystemInspector final
	{
	public:
		explicit SystemInspector() = default;
		~SystemInspector() = default;

		void Render(ecs::Registry* registry);
	private:

	};
} // cumulonimbus::editor