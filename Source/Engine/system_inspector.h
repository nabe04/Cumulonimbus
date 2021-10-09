#pragma once
#include "rename_type_mapping.h"
#include "camera_component.h"

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

		void Update(float dt);
		void Render(ecs::Registry* registry);

		//[[nodiscard]]
		//const mapping::rename_type::UUID& GetCameraTexID() const { return camera_texture.tex_id; }
		//[[nodiscard]]
		//const DirectX::SimpleMath::Matrix& GetCameraScalingMatrix() const { return camera_texture.scaling_matrix; }
	private:
		// CameraComponentのScene View上に表示するビルボードテクスチャID(UUID)
		//mapping::rename_type::UUID camera_tex_id{};
		//camera::CameraTexture camera_texture{};
	};
} // cumulonimbus::editor