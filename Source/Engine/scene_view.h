#pragma once
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "scene_view_camera.h"

namespace cumulonimbus::editor
{
	class SceneView final
	{
	public:
		explicit SceneView();
		~SceneView() = default;

		void Update(float dt);
		void Render();

		/**
		 * @brief : SceneView���̃}�E�X�ʒu��Window���̈ʒu�ɕϊ�
		 * @remark : SceneView�O�Ƀ}�E�X�����݂���ꍇ(-1,-1)��������
		 */
		[[nodiscard]]
		DirectX::XMINT2 ConvertWindowPos() const;

		[[nodiscard]]
		camera::SceneViewCamera& GetSceneViewCamera() const { return *scene_view_camera.get(); }
	private:
		std::unique_ptr<camera::SceneViewCamera> scene_view_camera{};
		DirectX::SimpleMath::Vector2 image_size{};
		DirectX::SimpleMath::Vector2 window_offset{};
		DirectX::SimpleMath::Vector2 window_mouse_pos{};
		float title_bar_height{};
	};
} // cumulonimbus::editor
