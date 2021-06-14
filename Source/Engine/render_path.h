#pragma once
#include <d3d11.h>

namespace cumulonimbus::ecs
{
	class Registry;
}

class View;
class Light;

namespace cumulonimbus::renderer
{
	class RenderPath final
	{
	public:
		void Render(ID3D11DeviceContext* immediate_context, cumulonimbus::ecs::Registry* registry, View* view, Light* light);

	private:

	};
}
