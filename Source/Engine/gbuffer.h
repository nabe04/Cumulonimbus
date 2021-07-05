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
		void Clear(float r = 0, float g = 0, float b = 0, float a = 1);

		/*
		 * brief : GBuffer用のRTVとシェーダーのバインド
		 */
		void BindShaderAndRTV();
		void UnbindShaderAndRTV();

		[[nodiscard]] ID3D11ShaderResourceView** GetAlbedoBufferSRV_Address()		const { return albedo_buffer->GetRenderTargetSRV(); }
		[[nodiscard]] ID3D11ShaderResourceView** GetPositionBufferSRV_Address()		const { return position_buffer->GetRenderTargetSRV(); }
		[[nodiscard]] ID3D11ShaderResourceView** GetNormalBufferSRV_Address()		const { return normal_buffer->GetRenderTargetSRV(); }
		[[nodiscard]] ID3D11ShaderResourceView** GetShaderSlotBufferSRV_Address()   const { return shader_slot_buffer->GetRenderTargetSRV(); }

		/*
		 * brief : RenderPathクラスのGBuffer同士のBlit処理に加えるのか
		 */
		[[nodiscard]] bool GetIsUsedGBuffer() const { return is_used_gbuffer; }

	private:
		/*
		 * brief	 : GBufferとして使用するテクスチャ群
		 * ※caution : FrameBufferクラスのActivate系は使わない
		 *			   FrameBufferのActivateでのOMSetRenderTargetは
		 *			   FrameBufferクラスのみのRenderTargetViewのセットだから
		 */
		// テクスチャ本来の色情報バッファ
		std::unique_ptr<FrameBuffer> albedo_buffer	{ nullptr };
		// ワールド座標バッファ
		std::unique_ptr<FrameBuffer> position_buffer{ nullptr };
		// 法線情報バッファ
		std::unique_ptr<FrameBuffer> normal_buffer	{ nullptr };
		// シェーダースロット用バッファ
		std::unique_ptr<FrameBuffer> shader_slot_buffer{ nullptr };

		static constexpr u_int num_rtv = 4; // GBufferで使用するrender_target_viewの総数

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> default_render_target_view{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> default_depth_stencil_view{ nullptr };

		// GBuffer用のdepth_stencil_view,shader_resource_view
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		dsv_for_gbuffer{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	srv_for_gbuffer{ nullptr };

		// GBuffer用シェーダー
		std::unique_ptr<shader_system::VertexShader> gbuffer_vs{ nullptr };
		std::unique_ptr<shader_system::PixelShader>  gbuffer_ps{ nullptr };

		// RenderPathクラスのGBuffer同士のBlit処理時に
		// この変数がfalseならテクスチャを使用しない
		bool is_used_gbuffer = false;

		/*
		 * brief     : GBuffer用のrender_target_viewのセット
		 * ※caution : depth_stencil_viewは全てのGBufferで共通のものを使用する
		 */
		void BindRTV();
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
	};
} // cumulonimbus::graphics::buffer