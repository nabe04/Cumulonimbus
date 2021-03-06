#pragma once

class FrameBuffer;

namespace cumulonimbus::camera
{
	class Camera;
} // cumulonimbus::camera

namespace cumulonimbus::system
{
	class EffekseerManager final
	{
	public:
		explicit EffekseerManager(ID3D11Device* device, ID3D11DeviceContext* device_context);
		~EffekseerManager();

		void Begin(ID3D11DeviceContext* immediate_context, const camera::Camera* camera,ID3D11DepthStencilView* dsv = nullptr) const;
		void Render() const;
		void End(ID3D11DeviceContext* immediate_context) const;

		void Update(float dt) const;

		[[nodiscard]]
		const Effekseer::Manager* GetManager() const
		{
			return effekseer_manager;
		}

		[[nodiscard]]
		const EffekseerRenderer::Renderer* GetRenderer() const
		{
			return effekseer_renderer;
		}

	private:
		Effekseer::Manager* effekseer_manager{ nullptr };
		EffekseerRenderer::Renderer* effekseer_renderer{ nullptr };

		std::unique_ptr<FrameBuffer> frame_buffer{ nullptr };
	};
} // cumulonimbus::system
