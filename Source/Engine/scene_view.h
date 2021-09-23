#pragma once
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "scene_view_camera.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace editor
	{
		class ProjectView;
	} // editor
} // cumulonimbus

namespace cumulonimbus::editor
{
	class SceneView final
	{
	public:
		//	�}�E�X�̃z�o�[���
		enum class MouseHoverEvent
		{
			Begin_Hovered,  // �z�o�[�J�n��
			Hovering,		// �z�o�[��
			End_Hovered,	// �z�o�[�I����
			None,			// �E�B���h�E�O��

			End
		};

		struct MouseHover
		{
			bool is_old_hovered{ false };
			bool is_current_hovered{ false };
			MouseHoverEvent event{ MouseHoverEvent::None };

			void Update();
		};

	public:
		explicit SceneView();
		~SceneView() = default;

		void Update(float dt);
		void Render(ecs::Registry* registry, const ProjectView* project_view);

		/**
		 * @brief : SceneView���̃}�E�X�ʒu��Window���̈ʒu�ɕϊ�
		 * @remark : SceneView�O�Ƀ}�E�X�����݂���ꍇ(-1,-1)��������
		 */
		[[nodiscard]]
		DirectX::XMINT2 ConvertWindowPos() const;

		/**
		 * @brief : Scene View�E�B���h�E���ɃJ�[�\�������邩
		 * @return : true -> �J�[�\��������
		 * @return : false -> �J�[�\�����Ȃ�
		 */
		[[nodiscard]]
		bool IsWindowHovered();

		[[nodiscard]]
		camera::SceneViewCamera& GetSceneViewCamera() const { return *scene_view_camera.get(); }
	private:
		std::unique_ptr<camera::SceneViewCamera> scene_view_camera{};
		MouseHover								 mouse_hover{};
		DirectX::SimpleMath::Vector2			 image_size{};
		DirectX::SimpleMath::Vector2			 window_offset{};
		DirectX::SimpleMath::Vector2			 window_mouse_pos{};
		DirectX::SimpleMath::Vector2			 window_pos{};
		DirectX::SimpleMath::Vector2			 window_size{}; // �^�C�g���o�[�������E�B���h�E�T�C�Y
		float									 title_bar_height{};
		mapping::rename_type::Entity			 dragging_entity{}; // ���݃h���b�O����Ă���G���e�B�e�B

		/**
		 * @brief : �v���W�F�N�g�r���[����h���b�O���Ă������f���̒ǉ�
		 */
		void AddModel(ecs::Registry* registry, const std::filesystem::path& file_path);
		/**
		 * @brief : �v���W�F�N�g�r���[����h���b�O���Ă������f�����}�E�X�Ŕz�u��
		 */
		void DraggingModel(ecs::Registry* registry) const;
	};
} // cumulonimbus::editor
