#pragma once
#include <memory>

#include <d3d11.h>

#include "frame_buffer.h"

namespace shader
{
	class VertexShader;
	class PixelShader;
}
class GaussianBlur;

namespace cumulonimbus::renderer
{
	/*
	 * brief : 描画対象の深度値を抽出
	 *         分散シャドウマップ用にすべての抽出が終わると
	 *         ブラー(GaussianBlur)をかける
	 */
	class DepthMap final
	{
	public:
		/*
		 * width  : 深度マップテクスチャの幅
		 * height : 深度マップテクスチャの高さ
		 */
		explicit DepthMap(ID3D11Device* device, u_int width = default_depth_map_width, u_int height = default_depth_map_height);

		/*
		 * brief : フレームバッファのクリア処理
		 *         FB,VS,PSのセット
		 */
		void Begin(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : default_fb,vs,psにセットしなおす
		 *	       抽出した深度テクスチャのブラー適用
		 */
		void End(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : FB,VS,PSの実行
		 *		   現在保持しているFB,VS,PSをdefault_fb,vs,ps
		 *		   に保持する(Deactivate時に使用)
		 */
		void Activate(ID3D11DeviceContext* immediate_context) const;

		/*
		 * brief : default_fb,vs,psにそれぞれセットしなおす
		 */
		void Deactivate(ID3D11DeviceContext* immediate_context) const;

		// ブラー適用後テクスチャのSRV
		[[nodiscard]] ID3D11ShaderResourceView** GetDepthExtractionSRV() const { return depth_extraction_fb.get()->render_target_shader_resource_view.GetAddressOf(); }

	private:
		static const u_int default_depth_map_width  = 2048;
		static const u_int default_depth_map_height = 2048;
		
		// 深度保存用バッファ
		std::unique_ptr<FrameBuffer> depth_extraction_fb{};

		// ブラー用(End()関数時にブラーをかける)
		std::unique_ptr<GaussianBlur> gaussian_blur{};

		// vertex & pixel shader
		std::unique_ptr<shader::VertexShader> depth_extraction_vs{};
		std::unique_ptr<shader::PixelShader>  depth_extraction_ps{};
	};
}