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
	class GameView final
	{
	public:
		explicit GameView() = default;
		~GameView() = default;

		void Render(ecs::Registry* registry);
	private:

	};
} // cumulonimbus::editor