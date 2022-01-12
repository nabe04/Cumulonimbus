#include "effekseer_manager.h"

#include "camera.h"
#include "locator.h"

namespace cumulonimbus::system
{
	EffekseerManager::EffekseerManager(ID3D11Device* device, ID3D11DeviceContext* device_context)
	{
		// Effekseer�����_������
		//Effekseer::Manager::Create()
		effekseer_renderer = EffekseerRendererDX11::Renderer::Create(device, device_context, 2048);
		// Effekseer�}�l�[�W���[����
		effekseer_manager = Effekseer::Manager::Create(2048);

		// Effekseer�����_���̊e��ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK�j
		effekseer_manager->SetSpriteRenderer(effekseer_renderer->CreateSpriteRenderer());
		effekseer_manager->SetRibbonRenderer(effekseer_renderer->CreateRibbonRenderer());
		effekseer_manager->SetRingRenderer(effekseer_renderer->CreateRingRenderer());
		effekseer_manager->SetTrackRenderer(effekseer_renderer->CreateTrackRenderer());
		effekseer_manager->SetModelRenderer(effekseer_renderer->CreateModelRenderer());
		// Effekseer���ł̃��[�_�[�̐ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK�j
		effekseer_manager->SetTextureLoader(effekseer_renderer->CreateTextureLoader());
		effekseer_manager->SetModelLoader(effekseer_renderer->CreateModelLoader());
		effekseer_manager->SetMaterialLoader(effekseer_renderer->CreateMaterialLoader());

		// Effekseer��������W�n�Ōv�Z����
		effekseer_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

		// Effekseer�p��frame buffer�쐬
		const u_int width  = locator::Locator::GetWindow()->Width();
		const u_int height = locator::Locator::GetWindow()->Height();
		frame_buffer = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R24G8_TYPELESS, true, false);
	}

	EffekseerManager::~EffekseerManager()
	{
		effekseer_manager->Destroy();
		effekseer_renderer->Destroy();
	}

	void EffekseerManager::Begin(ID3D11DeviceContext* immediate_context, const camera::Camera* camera, ID3D11DepthStencilView* dsv) const
	{
		//frame_buffer->Activate(immediate_context,dsv);
		// �r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
		effekseer_renderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera->GetViewMat()));
		effekseer_renderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera->GetProjectionMat()));
		// Effekseer�`��J�n
		effekseer_renderer->BeginRendering();
	}

	void EffekseerManager::Render() const
	{
		// �}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬���A
		// Draw()�֐������s���鏇���Ő���ł�����
		effekseer_manager->Draw();
	}

	void EffekseerManager::End(ID3D11DeviceContext* immediate_context) const
	{
		// Effekseer�`��I��
		effekseer_renderer->EndRendering();
		//frame_buffer->Deactivate(immediate_context);
	}

	void EffekseerManager::Update(const float dt) const
	{
		effekseer_manager->Update();
	}
} // cumulonimbus::system
