#pragma once
#include <d3d11.h>
#include <wrl.h>

class FrameBuffer;

namespace cumulonimbus::shader_system
{
	class VertexShader;
	class PixelShader;
} // cumulonimbus::shader_system

namespace cumulonimbus::graphics::buffer
{
	class GBuffer final
	{
	public:
		explicit GBuffer();

		/**
		 * @brief : render_target_view(frame_buffer)とdepth_stencil_viewの画面クリア処理
		 */
		void Clear(float r = 0, float g = 0, float b = 0, float a = 1);

		/**
		 * @brief : GBuffer用のRTVとシェーダーのバインド
		 */
		void BindShaderAndRTV();
		void UnbindShaderAndRTV();

		/**
		 * @brief : GBuffer用のrender_target_viewのセット
		 * @remark : depth_stencil_viewは全てのGBufferで共通のものを使用する
		 */
		void BindRTV();
		/**
		 * @brief     : セットされているrender_target_viewのdepth_stencil_viewを
		 *			    "default" render_target_viewにセット
		 */
		void UnbindRtv();

		/**
		 * @brief : GBuffer用シェーダーのセット
		 */
		void BindShader() const;
		/**
		 * @brief : GBuffer用シェーダーのリセット
		 */
		void UnbindShader() const;

		/**
		 * @brief : GBufferのライティング用シェーダーのセット
		 * @remark : ピクセルシェーダーのみのセット
		 */
		void BindGBuffLightingShader() const;

		/**
		 * @brief : 全てのGBufferテクスチャのバインド
		 * @remark : バインドされるテクスチャのスロット番号
		 * @remark : slot TexSlot_BaseColorMap(20) : AlbedoBufferテクスチャ
		 * @remark : slot TexSlot_NormalMap(21)	 : NormalBufferテクスチャ
		 * @remark : slot TexSlot_Position(30)	 : PositionBufferテクスチャ
		 * @remark : slot TexSlot_ShaderSlot(31)	 : ShaderSlotBufferテクスチャ
		 */
		void BindGBufferTextures() const;

		/**
		 * @brief : 全てのGBufferテクスチャのアンバインド
		 * @remark : バインドされるテクスチャのスロット番号
		 * @remark : slot TexSlot_BaseColorMap(20) : AlbedoBufferテクスチャ
		 * @remark : slot TexSlot_NormalMap(21)	 : NormalBufferテクスチャ
		 * @remark : slot TexSlot_Position(30)	 : PositionBufferテクスチャ
		 * @remark : slot TexSlot_ShaderSlot(31)	 : ShaderSlotBufferテクスチャ
		 */
		void UnbindGBufferTextures() const;

		/**
		 * @brief : albedo_bufferテクスチャのバインド
		 * @remark : slot番号 : TexSlot_BaseColorMap(20)
		 */
		void BindColorTexture() const;
		/**
		 * @brief : normal_bufferテクスチャのバインド
		 * @remark : slot番号 : TexSlot_NormalMap(21)
		 */
		void BindNormalMapTexture() const;
		/**
		 * @brief : position_bufferテクスチャのバインド
		 * @remark : slot番号 : TexSlot_Position(30)
		 */
		void BindPositionTexture() const;
		/**
		 * @breif : 深度バッファテクスチャのバインド
		 * @remark : slot番号 : TexSlot_Depth(0)
		 */
		void BindDepthTexture();

		/**
		 * @brief : albedo_bufferテクスチャのアンバインド
		 * @remark : slot番号 : TexSlot_BaseColorMap(20)
		 */
		void UnbindColorTexture() const;
		/**
		 * @brief : normal_bufferテクスチャのアンバインド
		 * @remark :slot番号 : TexSlot_NormalMap(21)
		 */
		void UnbindNormalMapTexture() const;
		/**
		 * @brief : position_bufferテクスチャのアンバインド
		 * @remark :slot番号 : TexSlot_Position(30)
		 */
		void UnbindPositionTexture() const;
		/**
		 * @breif : 深度バッファテクスチャのアンバインド
		 * @remark : slot番号 : TexSlot_Depth(0)
		 */
		void UnbindDepthTexture();

		/*
		 * brief : GBufferテクスチャのSRVのゲット
		 */
		[[nodiscard]] ID3D11ShaderResourceView** GetColorBufferSRV_Address()		const;
		[[nodiscard]] ID3D11ShaderResourceView** GetPositionBufferSRV_Address()		const;
		[[nodiscard]] ID3D11ShaderResourceView** GetNormalBufferSRV_Address()		const;

	private:
		/*
		 * brief	 : GBufferとして使用するテクスチャ群
		 * ※caution : FrameBufferクラスのActivate系は使わない
		 *			   FrameBufferのActivateでのOMSetRenderTargetは
		 *			   FrameBufferクラスのみのRenderTargetViewのセットだから
		 */
		// テクスチャ本来の色情報バッファ
		std::unique_ptr<FrameBuffer> color_buffer	{ nullptr };
		// ワールド座標バッファ
		std::unique_ptr<FrameBuffer> position_buffer{ nullptr };
		// 法線情報バッファ
		std::unique_ptr<FrameBuffer> normal_buffer	{ nullptr };
		// メタリック(metallic)、ラフネス(roughness)、オクリュージョン(occlusion)バッファ
		std::unique_ptr<FrameBuffer> mro_buffer{ nullptr };

		static constexpr u_int num_rtv = 3; // GBufferで使用するrender_target_viewの総数

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> default_render_target_view{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> default_depth_stencil_view{ nullptr };

		// GBuffer用のdepth_stencil_view,shader_resource_view
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		dsv_for_g_buffer{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	srv_for_g_buffer{ nullptr };

		// GBuffer用シェーダー
		std::unique_ptr<shader_system::VertexShader> g_buffer_vs{ nullptr };
		std::unique_ptr<shader_system::PixelShader>  g_buffer_ps{ nullptr };
		// GBufferのライティング用シェーダー
		std::unique_ptr<shader_system::PixelShader> g_buff_lighting_ps{ nullptr };
	};
} // cumulonimbus::graphics::buffer