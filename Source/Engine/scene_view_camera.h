#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <imgui_internal.h>

#include "camera.h"
#include "constant_buffer.h"
#include "frame_buffer.h"
#include "shader_interop_renderer.h"
#include "locator.h"

namespace cumulonimbus::camera
{
	class SceneViewCamera final
	{
	public:
		explicit SceneViewCamera(float width, float height);
		explicit SceneViewCamera() = default; // for cereal
		~SceneViewCamera() = default;

		template<class Archive>
		void serialize(Archive&& archive);

		void Update(float dt);
		void RenderImGui();

		[[nodiscard]]
		Camera& GetCamera() const { return *camera.get(); }

		/**
		 * @brief : SceneView’†‚ÌƒJƒƒ‰ƒ[ƒN
		 */
		void EditCamera(ImGuiWindow* window) const;
	private:
		std::unique_ptr<Camera> camera{};
	};
} // cumulonimbus::camera