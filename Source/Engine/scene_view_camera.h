#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

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
	private:
		std::unique_ptr<buffer::ConstantBuffer<CameraCB>>	cb_camera{ nullptr };
		std::unique_ptr<FrameBuffer>						off_screen{ nullptr };

		//-- �J�����̊�{�p�����[�^ --//
		// �J�����̍ő�p�x(���) (y,z�͖���`)
		DirectX::SimpleMath::Vector3 max_camera_angle{ 85.0f,-1.f,-1.f }; //	�J�����̍ő�p(xyz���ꂼ��+-�����̊p�x��\���B�p�����[�^��0�ȉ��̏ꍇ�̓J�����̊p�x�����Ȃ�)

		DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };	// �J�����ʒu
		DirectX::SimpleMath::Vector3 focus_position{ 0.0f,0.0f,1.0f };	// �J�����œ_�ʒu
		DirectX::SimpleMath::Vector3 focus_offset{ 0.0f,0.0f,0.0f };	// �J�����œ_�ʒu�̃I�t�Z�b�g�l

		DirectX::SimpleMath::Vector3 up_vec{ 0.0f,1.0f,0.0f };			// view�ϊ��s��ɓn���Ƃ��Ɏg�p����up�x�N�g��(�Œ�)
		DirectX::SimpleMath::Vector3 current_up_vec{ 0.0f,1.0f,0.0f };	// �J�����̌��݂�up�x�N�g��
		DirectX::SimpleMath::Vector3 right_vec{ 1.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 front_vec{ focus_position.x - eye_position.x,
												focus_position.y - eye_position.y,
												focus_position.z - eye_position.z };

		DirectX::SimpleMath::Vector3 current_camera_up{ .0f,1.f,.0f };		// �J�����̂��ꂼ��̃x�N�g�������s�ɂȂ�悤�ɕ␳�����x�N�g��(�v�Z�ȂǂɎg�p)
		DirectX::SimpleMath::Vector3 camera_right{ 1.0f,0.0f,0.0f };

		DirectX::SimpleMath::Matrix view_mat{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix projection_mat{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix view_projection_mat{ DirectX::SimpleMath::Matrix::Identity };

		DirectX::SimpleMath::Vector3 camera_angle{}; // �J�����̊p�x(Degree),����n�̍��W������̊p�x

		float near_z{};	// �J�����̍ŋߒl
		float far_z{};	// �J�����̍ŉ��n
		float fov{};		// ����p
		float aspect{};	// �A�X�y�N�g��
		float width{};	// �r���[�s���̕�
		float height{};	// �r���[�s���̍���

		//-- for debug --//
		DirectX::SimpleMath::Vector2 camera_speed{ 1.f,1.f };
		DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
		DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

		//-- �J�����ƃI�u�W�F�N�g�̃A�^�b�`�p�ϐ� --//
		float camera_length{}; // �I�u�W�F�N�g�ƃJ�����̒���

		bool  is_use_camera_for_debug = true; // �f�o�b�O�p�J�������g�p���邩
		bool  is_perspective = true;  // �������e���g�p���邩
		bool  is_active		 = true;  // �J������`��Ώۂɉ����邩(RenderPath::Render�֐�����true�̏ꍇ�̂ݕ`�悷��)
		bool  is_main_camera = false; // �o�b�N�o�b�t�@�p�̃J����(�S�Ă�camera_component�̒���true�ɂȂ�̂͏�Ɉ����)

		/**
		 * @brier : ���s�x�N�g��(right,up,front)�̎Z�o
		 */
		void CalcCameraOrthogonalVector();

		/**
		 * @brief : ��I�u�W�F�N�g�A�^�b�`���̍X�V�֐�
		 *			is_use_camera_for_object == false���Ɏ��s�����
		 */
		void UpdateDefaultCamera(float dt);

		/**
		 * @brief   : ���݂̃J�����̐��K���s�x�N�g�����Z�o
		 */
		void CalcCameraDirectionalVector();

		/**
		 * @brief : ������W������ɂ��ẴJ�����̊p�x(Degree)�v�Z
		 */
		void CalcCameraAngle();

		/**
		 * @brief : �J�����̊p�x����
		 *			�J�����̍ő�p�����ƂɃJ�����O���x�N�g��(front_vec)�𒲐�
		 */
		void AdjustCameraAngle();

		/**
		 * @brief : �R���X�^���g�o�b�t�@�p�����[�^�̃Z�b�g
		 *			(Update�֐��̍Ō�ōs��)
		 */
		void SetCBufferParam() const;

		void SetCameraRight(const DirectX::SimpleMath::Vector3& right);
		void SetCameraUp(const DirectX::SimpleMath::Vector3& up);
		void SetCameraFront(const DirectX::SimpleMath::Vector3& front);

		/**
		 * @brief			: �J�������[�N���̓���
		 * @param velocity	: �J�����X�s�[�h(�f�o�b�N�̐ݒ�ŕύX�\)
		 */
		void Pan(float velocity);								// �J�����̍��E�̌X��(�ʒu�͌Œ�)
		void Tilt(float velocity);								// �J�����̏㉺�̌X��(�ʒu�͌Œ�)
		void DollyInOut(float velocity);						// �J�����̑O��̓���(�����͌Œ�)
		void Track(float velocity,
			const DirectX::SimpleMath::Vector3& axis /*���*/);								// �J�����̍��E�ړ�(�����͌Œ�)
		void Crane(float velocity,
			const DirectX::SimpleMath::Vector3& axis /*���*/);	// �J�����̏㉺�ړ�(�����͌Œ�)
	};
} // cumulonimbus::camera