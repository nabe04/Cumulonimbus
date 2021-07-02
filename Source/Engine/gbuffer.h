#pragma once
#include "frame_buffer.h"
#include "shader.h"

namespace cumulonimbus::graphics::buffer
{
	class GBuffer final
	{
	public:
		explicit GBuffer();

		/*
		 * brief : render_target_view(frame_buffer)とdepth_stencil_viewの画面クリア処理
		 */
		void Clear(float r = 0, float g = 0, float b = 0, float a = 1) const;

		/*
		 * brief     : GBuffer用のrender_target_viewのセット
		 * ※caution : depth_stencil_viewは全てのGBufferで共通のものを使用する
		 */
		void BindRTV(ID3D11DepthStencilView* depth_stencil_view);
		/*
		 * brief     : セットされているrender_target_viewのdepth_stencil_viewを
		 *			   "default" render_target_viewにセット
		 */
		void UnbindRTV();

		/*
		 * brief : GBuffer用シェーダーのセット
 		 */
		void BindShader() const;
		/*
		 * brief : GBuffer用シェーダーのリセット
		 */
		void UnbindShader() const;

	private:
		/*
		 * brief	 : GBufferとして使用するテクスチャ群
		 * ※caution : FrameBufferクラスのActivate系は使わない
		 *			   FrameBufferのActivateでのOMSetRenderTargetは
		 *			   FrameBufferクラスのみのRenderTargetViewのセットだから
		 */
		std::unique_ptr<FrameBuffer> albedo_buffer	{ nullptr };
		std::unique_ptr<FrameBuffer> position_buffer{ nullptr };
		std::unique_ptr<FrameBuffer> normal_buffer	{ nullptr };

		static constexpr u_int num_rtv = 3; // GBufferで使用するrender_target_viewの総数

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> default_render_target_view{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> default_depth_stencil_view{ nullptr };

		// GBuffer用シェーダー
		std::unique_ptr<shader_system::VertexShader> gbuffer_vs{ nullptr };
		std::unique_ptr<shader_system::PixelShader>  gbuffer_ps{ nullptr };
	};
} // cumulonimbus::graphics::buffer