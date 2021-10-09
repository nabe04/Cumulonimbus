#include "system.h"

#include "camera_component.h"

namespace cumulonimbus::system
{
	System::System()
	{
		camera_texture = std::make_unique<camera::CameraTexture>(*this);
	}

	void System::Render(ecs::Registry* registry)
	{
		for(auto& [hash,render_function] : render_functions)
		{
			render_function(registry);
		}
	}

	void System::RegisterRenderFunction(
		const mapping::rename_type::Hash& hash,
		const std::function<void(ecs::Registry* registry)>& render_func)
	{
		// Œ^‚ª‚·‚Å‚É“o˜^‚³‚ê‚Ä‚¢‚½ê‡ã‘‚«‚·‚é
		if (render_functions.contains(hash))
			render_functions.at(hash) = render_func;

		render_functions.emplace(hash, render_func);
	}
} // cumulonimbus::system