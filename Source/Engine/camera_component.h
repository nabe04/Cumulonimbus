#pragma once
#include "component_base.h"
#include "camera.h"
#include "cereal_helper.h"
#include "rename_type_mapping.h"
#include "locator.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::camera
{
	/**
	 * @brief : CameraComponent�p�r���{�[�h��K�p����
	 *			�J�����e�N�X�`���̐ݒ�
	 * @remark : SystemInspector�N���X�Ńp�����[�^��ҏW����
	 *			 �S�ẴJ�����R���|�[�l���g�̃e�N�X�`���ɓK�p����
	 */
	struct CameraTexture
	{
		mapping::rename_type::UUID	 tex_id{};	 // �J�����e�N�X�`��ID
		DirectX::SimpleMath::Vector2 tex_size{ 1.f,1.f }; // ��ʂɕ\������e�N�X�`���T�C�Y
		DirectX::SimpleMath::Matrix  scaling_matrix{DirectX::SimpleMath::Matrix::Identity}; // �J�����e�N�X�`���S�̂ɓK�p����{��(�`�掞��)
	};
} // cumulonimbus::camera

namespace cumulonimbus::component
{
	class CameraComponent final : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit CameraComponent(); // for cereal & Inspector View��ł�AddComponent�p
		explicit CameraComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CameraComponent& copy_comp); // for prefab
		/**
		 * @param registry :
		 * @param ent :
		 * @param is_main_camera	: �o�b�N�o�b�t�@�p�J�����ɐݒ肷�邩(�f�t�H���g��true)
		 * @param width				: �r���[�s�񎞂̕�(�f�t�H���g�̓X�N���[���̕�)
		 * @param height			: �r���[�s�񎞂̍���(�f�t�H���g�̓X�N���[���̍���)
		 */
		explicit CameraComponent(
			ecs::Registry* registry, mapping::rename_type::Entity ent,
			bool is_main_camera = true,
			float width = locator::Locator::GetWindow()->Width(),
			float height = locator::Locator::GetWindow()->Height());
		explicit CameraComponent(mapping::component_tag::ComponentTag tag);
		CameraComponent(const CameraComponent& other);
		CameraComponent& operator=(const CameraComponent& other);

		~CameraComponent() override = default;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void CommonUpdate(float dt) override;

		void SceneUpdate(float dt) override;

		void GameUpdate(float dt) override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void serialize(Archive&& archive);

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

		[[nodiscard]]
		camera::Camera* GetCamera() const { return camera.get(); }
		[[nodiscard]]
		bool GetIsActive() const { return is_active; }
		[[nodiscard]]
		bool GetIsMainCamera() const { return is_main_camera; }
	private:
		std::unique_ptr<camera::Camera> camera{};

		//-- �J�����ƃI�u�W�F�N�g�̃A�^�b�`�p�ϐ� --//
		mapping::rename_type::Entity attach_entity;	// �A�^�b�`����I�u�W�F�N�g�̃G���e�B�e�B
		float camera_length{100}; // �I�u�W�F�N�g�ƃJ�����̒���

		bool  is_active				  = true;  // �J������`��Ώۂɉ����邩(RenderPath::Render�֐�����true�̏ꍇ�̂ݕ`�悷��)
		bool  is_use_camera_for_debug = true; // �f�o�b�O�p�J�������g�p���邩
		bool  is_main_camera		  = false; // �o�b�N�o�b�t�@�p�̃J����(�S�Ă�camera_component�̒���true�ɂȂ�̂͏�Ɉ����)

		/**
		 * @brief : �I�u�W�F�N�g�A�^�b�`���̍X�V�֐�
		 *			is_use_camera_for_object == true���Ɏ��s�����
		 */
		void UpdateObjectCamera(float dt);
	};

} // cumulonimbus::component


