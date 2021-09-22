#include "scene_view_camera.h"

#include <cassert>
#include <cmath>
#include <wrl.h>

#include "arithmetic.h"
#include "cereal_helper.h"
#include "framework.h"
#include "frame_buffer.h"

namespace cumulonimbus::camera
{
	template <class Archive>
	void SceneViewCamera::serialize(Archive&& archive)
	{
		archive(

		);
	}

	SceneViewCamera::SceneViewCamera(float width, float height)
	{
		camera = std::make_unique<Camera>(width, height);
	}

	void SceneViewCamera::Update(float dt)
	{
		camera->Update(dt);
	}

	void SceneViewCamera::RenderImGui()
	{

	}

	void SceneViewCamera::EditCamera(ImGuiWindow* window) const
	{
		const auto& mouse = locator::Locator::GetInput()->Mouse();
		const auto& key = locator::Locator::GetInput()->Keyboard();

		if (mouse.GetState(MouseButton::Left) == ButtonState::Held &&
			mouse.GetState(MouseButton::Right) == ButtonState::Held)
		{
			camera->Track(static_cast<float>(mouse.DeltaX()), camera->GetCameraRight());
			camera->Crane(static_cast<float>(-mouse.DeltaY()), { 0,1,0 });
		}
		else if (mouse.GetState(MouseButton::Left) == ButtonState::Held)
		{
			camera->DollyInOut(static_cast<float>(mouse.DeltaY()));
			camera->Pan(static_cast<float>(mouse.DeltaX()));

			if (key.GetState(Keycode::D) == ButtonState::Held)
				camera->Track(camera->GetCameraSpeed().x, camera->GetCameraRight());
			if (key.GetState(Keycode::A) == ButtonState::Held)
				camera->Track(-camera->GetCameraSpeed().x, camera->GetCameraRight());
			if (key.GetState(Keycode::W) == ButtonState::Held)
				camera->Crane(camera->GetCameraSpeed().y, camera->GetCameraFront());
			if (key.GetState(Keycode::S) == ButtonState::Held)
				camera->Crane(-camera->GetCameraSpeed().y, camera->GetCameraFront());
		}
		else if (mouse.GetState(MouseButton::Right) == ButtonState::Held)
		{
			camera->Pan(static_cast<float>(mouse.DeltaX()));
			camera->Tilt(static_cast<float>(mouse.DeltaY()));

			if (key.GetState(Keycode::D) == ButtonState::Held)
				camera->Track(camera->GetCameraSpeed().x, camera->GetCameraRight());
			if (key.GetState(Keycode::A) == ButtonState::Held)
				camera->Track(-camera->GetCameraSpeed().x, camera->GetCameraRight());
			if (key.GetState(Keycode::W) == ButtonState::Held)
				camera->Crane(camera->GetCameraSpeed().y, camera->GetCameraFront());
			if (key.GetState(Keycode::S) == ButtonState::Held)
				camera->Crane(-camera->GetCameraSpeed().y, camera->GetCameraFront());
		}
	}

} // cumulonimbus::camera
