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

		/*
		 * brief : render_target_view(frame_buffer)とdepth_stencil_viewの画面クリア処理
		 */
		void Clear(float r = 0, float g = 0, float b = 0, float a = 1);

		/*
		 * brief : GBuffer用のRTVとシェーダーのバインド
		 */
		void BindShaderAndRTV();
		void UnbindShaderAndRTV();

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

		/*
		 * brief	 : GBufferのライティング用シェーダーのセット
		 * ※caution : ピクセルシェーダーのみのセット
		 */
		void BindGBuffLightingShader() const;

		/*
		 * brief : 全てのGBufferテクスチャのバインド
		 * バインドされるテクスチャのスロット番号
		 * slot TexSlot_BaseColorMap(20) : AlbedoBufferテクスチャ
		 * slot TexSlot_NormalMap(21)	 : NormalBufferテクスチャ
		 * slot TexSlot_Position(30)	 : PositionBufferテクスチャ
		 * slot TexSlot_ShaderSlot(31)	 : ShaderSlotBufferテクスチャ
		 */
		void BindGBufferTextures() const;

		/*
		 * brief : 全てのGBufferテクスチャのアンバインド
		 * バインドされるテクスチャのスロット番号
		 * slot TexSlot_BaseColorMap(20) : AlbedoBufferテクスチャ
		 * slot TexSlot_NormalMap(21)	 : NormalBufferテクスチャ
		 * slot TexSlot_Position(30)	 : PositionBufferテクスチャ
		 * slot TexSlot_ShaderSlot(31)	 : ShaderSlotBufferテクスチャ
		 */
		void UnbindGBufferTextures() const;


		/*
		 * brief : GBufferテクスチャの個別のSRVのバインド
		 */
		/*
		 * brief	: albedo_bufferテクスチャのバインド
		 * slot番号 : TexSlot_BaseColorMap(20)
		 */
		void BindColorTexture() const;
		/*
		 * brief	: normal_bufferテクスチャのバインド
		 * slot番号 : TexSlot_NormalMap(21)
		 */
		void BindNormalMapTexture() const;
		/*
		 * brief	: position_bufferテクスチャのバインド
		 * slot番号 : TexSlot_Position(30)
		 */
		void BindPositionTexture() const;

		/*
		 * brief : GBufferテクスチャの個別のSRVのアンバインド
		*/
		/*
		 * brief	 : albedo_bufferテクスチャのアンバインド
		* slot番号 : TexSlot_BaseColorMap(20)
		 */
		void UnbindColorTexture() const;
		/*
		 * brief	: normal_bufferテクスチャのアンバインド
		 * slot番号 : TexSlot_NormalMap(21)
		 */
		void UnbindNormalMapTexture() const;
		/*
		 * brief	: position_bufferテクスチャのアンバインド
		 * slot番号 : TexSlot_Position(30)
		 */
		void UnbindPositionTexture() const;


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