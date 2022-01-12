#include "effekseer_manager.h"

#include "camera.h"
#include "locator.h"

namespace cumulonimbus::system
{
	EffekseerManager::EffekseerManager(ID3D11Device* device, ID3D11DeviceContext* device_context)
	{
		// Effekseerレンダラ生成
		//Effekseer::Manager::Create()
		effekseer_renderer = EffekseerRendererDX11::Renderer::Create(device, device_context, 2048);
		// Effekseerマネージャー生成
		effekseer_manager = Effekseer::Manager::Create(2048);

		// Effekseerレンダラの各種設定（特別なカスタマイズをしない場合は定型的に以下の設定でOK）
		effekseer_manager->SetSpriteRenderer(effekseer_renderer->CreateSpriteRenderer());
		effekseer_manager->SetRibbonRenderer(effekseer_renderer->CreateRibbonRenderer());
		effekseer_manager->SetRingRenderer(effekseer_renderer->CreateRingRenderer());
		effekseer_manager->SetTrackRenderer(effekseer_renderer->CreateTrackRenderer());
		effekseer_manager->SetModelRenderer(effekseer_renderer->CreateModelRenderer());
		// Effekseer内でのローダーの設定（特別なカスタマイズをしない場合は以下の設定でOK）
		effekseer_manager->SetTextureLoader(effekseer_renderer->CreateTextureLoader());
		effekseer_manager->SetModelLoader(effekseer_renderer->CreateModelLoader());
		effekseer_manager->SetMaterialLoader(effekseer_renderer->CreateMaterialLoader());

		// Effekseerを左手座標系で計算する
		effekseer_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

		// Effekseer用のframe buffer作成
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
		// ビュー＆プロジェクション行列をEffekseerレンダラに設定
		effekseer_renderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera->GetViewMat()));
		effekseer_renderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera->GetProjectionMat()));
		// Effekseer描画開始
		effekseer_renderer->BeginRendering();
	}

	void EffekseerManager::Render() const
	{
		// マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
		// Draw()関数を実行する順序で制御できそう
		effekseer_manager->Draw();
	}

	void EffekseerManager::End(ID3D11DeviceContext* immediate_context) const
	{
		// Effekseer描画終了
		effekseer_renderer->EndRendering();
		//frame_buffer->Deactivate(immediate_context);
	}

	void EffekseerManager::Update(const float dt) const
	{
		effekseer_manager->Update();
	}
} // cumulonimbus::system
