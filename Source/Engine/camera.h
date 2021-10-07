#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "frame_buffer.h"
#include "shader_interop_renderer.h"

namespace cumulonimbus::camera
{
	class Camera final
	{
	public:
		explicit Camera(float width, float height);
		explicit Camera() = default;
		Camera(const Camera& other);
		Camera& operator=(const Camera& other);
		~Camera() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(max_camera_angle),

				CEREAL_NVP(eye_position),
				CEREAL_NVP(focus_position),
				CEREAL_NVP(focus_offset),

				CEREAL_NVP(up_vec),
				CEREAL_NVP(current_up_vec),
				CEREAL_NVP(right_vec),
				CEREAL_NVP(front_vec),

				CEREAL_NVP(current_camera_up),
				CEREAL_NVP(camera_right),

				CEREAL_NVP(view_mat),
				CEREAL_NVP(projection_mat),
				CEREAL_NVP(view_projection_mat),

				CEREAL_NVP(camera_angle),

				CEREAL_NVP(near_z),
				CEREAL_NVP(far_z),
				CEREAL_NVP(fov),
				CEREAL_NVP(aspect),
				CEREAL_NVP(width),
				CEREAL_NVP(height),

				CEREAL_NVP(camera_speed),
				CEREAL_NVP(cur_mouse_pos),
				CEREAL_NVP(prev_mouse_pos),

				CEREAL_NVP(is_perspective)
			);
		}

		std::unique_ptr<Camera> Clone() { return std::make_unique<Camera>(*this); }

		void Initialize();
		void Update(float dt);
		void RenderImGui();

		void ClearFrameBuffer(
			const DirectX::SimpleMath::Vector4& clear_color = { 0.0f,0.0f,0.0f,1.0f }) const;
		void BindFrameBuffer() const;
		void UnbindFrameBuffer() const;

		void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
		void UnbindCBuffer() const;

		/**
		 * @brief		 : �N�H�[�^�j�I�����g�p����right_vector�����
		 *			       front_vector����]
		 * @param radian : ��]���̊p�x(���W�A���p)
		 */
		void RotationFrontVectorFromRightVector(float radian);
		/**
		 * @brief		 : �N�H�[�^�j�I�����g�p����up_vector�����
		 *			       front_vector����]
		 * @param radian : ��]���̊p�x(���W�A���p)
		 */
		void RotationFrontVectorFromUpVector(float radian);

		/**
		 * @brief			: TPS�J������z�肵�ẴJ�����̃s�b�`(X��)��]
		 * @param radian	: ��]���̊p�x(���W�A���p)
		 */
		void RotationTPSPitch(float radian);

		/**
		 * @brief			: TPS�J������z�肵�ẴJ������Yaw(Y��)��]
		 * @param radian	: ��]���̊p�x(���W�A���p)
		 */
		void RotationTPSYaw(float radian);

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
		void Pan(float velocity);			// �J�����̍��E�̌X��(�ʒu�͌Œ�)
		void Tilt(float velocity);			// �J�����̏㉺�̌X��(�ʒu�͌Œ�)
		void DollyInOut(float velocity);	// �J�����̑O��̓���(�����͌Œ�)
		void Track(float velocity,
			const DirectX::SimpleMath::Vector3& axis /*���*/);								// �J�����̍��E�ړ�(�����͌Œ�)
		void Crane(float velocity,
			const DirectX::SimpleMath::Vector3& axis /*���*/);	// �J�����̏㉺�ړ�(�����͌Œ�)

		/**
		 * @brief				: �J�����p�����[�^�̐ݒ�(�ʒu�A�����_�A�J�����A�b�v�x�N�g��)
		 * @param pos			: �J�����ʒu
		 * @param target		: �J���������_
		 * @param camera_up		: �J�����A�b�v�x�N�g��
		 */
		void SetViewInfo(
			const DirectX::SimpleMath::Vector3& pos,
			const DirectX::SimpleMath::Vector3& target,
			const DirectX::SimpleMath::Vector3& camera_up);
		/**
		 * @brief        : �������e�ɕK�v�ȃp�����[�^�̐ݒ�
		 * @param fov    : ����p
		 * @param aspect : �A�X�y�N�g��
		 * @param min    : �ŋߒl
		 * @param max    : �ŉ��n
		 */
		void SetProjection(float fov, float aspect, float min, float max);
		/**
		 * @brief        : ���s���e�ɕK�v�ȃp�����[�^�̐ݒ�
		 * @param width  : �J������
		 * @param height : �J��������
		 * @param min    : �ŋߒl
		 * @param max    : �ŉ��n
		 */
		void SetOrtho(float width, float height, float min, float max);

		/**
		* @brief : �J�����ʒu�̐ݒ�
		*/
		void SetEyePosition(const DirectX::SimpleMath::Vector3& pos) { eye_position = pos; }
		/**
		 * @brief : �J���������_�̐ݒ�
		 */
		void SetFocusPosition(const DirectX::SimpleMath::Vector3& target) { focus_position = target; }
		/**
		 * @brief : �J�����ʒu�I�t�Z�b�g�l�̐ݒ�
		 */
		void SetFocusOffset(const DirectX::SimpleMath::Vector3& offset) { focus_offset = offset; }

		/**
		 * @brief : ����p(fov)�̐ݒ�
		 */
		void SetFov(const float fov) const { cb_camera->data.camera_fov = fov; }

		void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { camera_speed = speed; }

		[[nodiscard]]
		float GetFov()    const { return fov; }
		[[nodiscard]]
		float GetAspect() const { return aspect; }
		[[nodiscard]]
		float GetNearZ()  const { return near_z; }
		[[nodiscard]]
		float GetFarZ()   const { return far_z; }

		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetViewMat()			const { return view_mat; }
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetProjectionMat()	const { return projection_mat; }

		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetEyePosition()   const { return eye_position; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetFocusPosition() const { return focus_position; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetFocusOffset()   const { return focus_offset; }

		// �J�����̌����擾
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetCameraRight() const { return right_vec; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetCameraUp()    const { return up_vec; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetCameraFront() const { return front_vec; }

		[[nodiscard]]
		const DirectX::SimpleMath::Vector2& GetCameraSpeed() const { return camera_speed; }

		[[nodiscard]]
		ID3D11ShaderResourceView** GetFrameBufferSRV_Address() const { return off_screen->GetRenderTargetSRV(); }
	private:
		std::shared_ptr<buffer::ConstantBuffer<CameraCB>>	cb_camera{ nullptr };
		std::shared_ptr<FrameBuffer>						off_screen{ nullptr };

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
		bool  is_perspective = true;  // �������e���g�p���邩
	};
} // cumulonimbus::camera