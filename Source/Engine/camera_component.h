#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"
#include "constant_buffer.h"
#include "frame_buffer.h"
#include "shader_interop_renderer.h"
#include "rename_type_mapping.h"
#include "locator.h"

class FrameBuffer;

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::component
{
	class CameraComponent final : public ComponentBase
	{
	public:
		/**
		 * @param is_main_camera	: �o�b�N�o�b�t�@�p�J�����ɐݒ肷�邩(�f�t�H���g��true)
		 * @param width				: �r���[�s�񎞂̕�(�f�t�H���g�̓X�N���[���̕�)
		 * @param height			: �r���[�s�񎞂̍���(�f�t�H���g�̓X�N���[���̍���)
		 */
		explicit CameraComponent(
			ecs::Registry* registry, mapping::rename_type::Entity ent,
			bool is_main_camera = true,
			float width  = locator::Locator::GetWindow()->Width(),
			float height = locator::Locator::GetWindow()->Height());
		explicit CameraComponent()  = default; // for cereal
 		~CameraComponent() override = default;

		void Update(float dt) override;
		void RenderImGui() override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		void ClearFrameBuffer(DirectX::SimpleMath::Vector4 clear_color = { 0.0f,0.0f,0.0f,1.0f }) const;
		void BindFrameBuffer() const;
		void UnbindFrameBuffer() const;

		void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
		void UnbindCBuffer() const;

		/**
		 * @brief						: �I�u�W�F�N�g�J�������g�p����ۂ�
		 *								  �Ώۃ��f���̃G���e�B�e�B���w��
		 * @param ent					: �Ώۃ��f���̃G���e�B�e�B
		 * @attention					: is_use_camera_for_debug(�f�o�b�O�p�J����)�t���O��"false"�ɂȂ�
		 */
		void AttachObject(cumulonimbus::mapping::rename_type::Entity ent);

		/**
		 * @brief				: �I�u�W�F�N�g�p�J�����̃p�����[�^�̏�����
		 *						  �J�����̏����ʒu���I�u�W�F�N�g�̌���ɃZ�b�g
		 *						  (�I�u�W�F�N�g�̃t�����g�x�N�g�� * -1) * camera_length
		 * @param camera_length : �I�u�W�F�N�g�ƃJ�����Ԃ̒���(�f�t�H���g == 50)
		 */
		void InitializeObjectCameraParam(float camera_length = 50.0f);

		/**
		 * @brief		: �o�b�N�o�b�t�@�p�̃J�����ɐ؂�ւ���
		 * @attention	: ���N���X��CameraComponent::is_main_camera��true��
		 *				  ���N���X�ȊO�̑S�Ă�CameraComponent::is_main_camera��false�ɂ���
		 */
		void SwitchMainCamera();

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
		 * @brief				: �J�����p�����[�^�̐ݒ�(�ʒu�A�����_�A�J�����A�b�v�x�N�g��)
		 * @param eye_position	: �J�����ʒu
		 * @param target		: �J���������_
		 * @param camera_up		: �J�����A�b�v�x�N�g��
		 */
		void SetViewInfo(
			DirectX::SimpleMath::Vector3 eye_position,
			DirectX::SimpleMath::Vector3 target,
			DirectX::SimpleMath::Vector3 camera_up);
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
		 * @brief : �J���������_�̐ݒ�
		 */
		void SetFocusPosition(const DirectX::SimpleMath::Vector3& target) { focus_position = target; }
		/**
		 * @brief : �J�����ʒu�̐ݒ�
		 */
		void SetEyePosition(const DirectX::SimpleMath::Vector3& pos) { eye_position = pos; }
		/**
		 * @brief		: �f�o�b�O���̃J�������x�̐ݒ�
		 * @pram speed	: �J�����̑���
		 */
		void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_speed = speed; }
		/**
		 * @brief : ����p(fov)�̐ݒ�
		 */
		void SetFov(const float fov) const { cb_camera->data.camera_fov = fov; }

		void SetIsDebugCamera(const bool flg) { is_use_camera_for_debug = flg; }
		void SetIsActive(const bool flg) { is_active = flg; }

		[[nodiscard]] float GetFov()    const { return fov; }
		[[nodiscard]] float GetAspect() const { return aspect; }
		[[nodiscard]] float GetNearZ()  const { return near_z; }
		[[nodiscard]] float GetFarZ()   const { return far_z; }

		[[nodiscard]] DirectX::SimpleMath::Matrix GetViewMat()			const { return view_mat; }
		[[nodiscard]] DirectX::SimpleMath::Matrix GetProjectionMat()	const { return projection_mat; }

		[[nodiscard]] DirectX::SimpleMath::Vector3 GetEyePosition()   const { return eye_position; }
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetFocusPosition() const { return focus_position; }

		// �J�����̌����擾
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetCameraRight() const { return right_vec; }
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetCameraUp()    const { return up_vec; }
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetCameraFront() const { return front_vec; }

		[[nodiscard]] DirectX::SimpleMath::Vector2 GetCameraSpeed() const { return camera_speed; }

		[[nodiscard]] bool GetIsActive() const { return is_active; }
		[[nodiscard]] bool GetIsMainCamera() const { return is_main_camera; }

		[[nodiscard]] ID3D11ShaderResourceView** GetFrameBufferSRV_Address() const { return off_screen->GetRenderTargetSRV(); }

	private:
		std::shared_ptr<buffer::ConstantBuffer<CameraCB>>	cb_camera{ nullptr };
		std::shared_ptr<FrameBuffer>						off_screen{ nullptr };

		//-- �J�����̊�{�p�����[�^ --//
		// �J�����̍ő�p�x(���) (y,z�͖���`)
		DirectX::SimpleMath::Vector3 max_camera_angle{ 85.0f,-1.f,-1.f }; //	�J�����̍ő�p(xyz���ꂼ��+-�����̊p�x��\���B�p�����[�^��0�ȉ��̏ꍇ�̓J�����̊p�x�����Ȃ�)

		DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 focus_position{ 0.0f,0.0f,1.0f };

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

		float near_z;	// �J�����̍ŋߒl
		float far_z;	// �J�����̍ŉ��n
		float fov;		// ����p
		float aspect;	// �A�X�y�N�g��
		float width;	// �r���[�s���̕�
		float height;	// �r���[�s���̍���

		//-- for debug --//
		DirectX::SimpleMath::Vector2 camera_speed{ 3.f,3.f };
		DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
		DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

		//-- �J�����ƃI�u�W�F�N�g�̃A�^�b�`�p�ϐ� --//
		mapping::rename_type::Entity attach_entity;	// �A�^�b�`����I�u�W�F�N�g�̃G���e�B�e�B
		float camera_length; // �I�u�W�F�N�g�ƃJ�����̒���

		bool  is_use_camera_for_debug = true; // �f�o�b�O�p�J�������g�p���邩
		bool  is_perspective = true;  // �������e���g�p���邩
		bool  is_active		 = true;  // �J������`��Ώۂɉ����邩(RenderPath::Render�֐�����true�̏ꍇ�̂ݕ`�悷��)
		bool  is_main_camera = false; // �o�b�N�o�b�t�@�p�̃J����(�S�Ă�camera_component�̒���true�ɂȂ�̂͏�Ɉ����)

		/**
		 * @brier : ���s�x�N�g��(right,up,front)�̎Z�o
		 */
		void CalcCameraOrthogonalVector();

		/**
		 * @brief : �I�u�W�F�N�g�A�^�b�`���̍X�V�֐�
		 *			is_use_camera_for_object == true���Ɏ��s�����
		 */
		void UpdateObjectCamera(float dt);

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

} // cumulonimbus::component


