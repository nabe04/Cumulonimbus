#pragma once
#include "shader_interop_renderer.h"
#include "shader_asset.h"

namespace cumulonimbus::shader_asset
{
	class PBRMaterial final : public ShaderAsset
	{
	public:
		explicit PBRMaterial();
		~PBRMaterial() override = default;

		void BindCBuffer()	 override;
		void UnbindCBuffer() override;

		void BindTexture()	 override;
		void UnbindTexture() override;

		void RenderImGui() override;

		/*
		 * brief : マテリアルのパラメータ使用するか
		 * true  : MaterialCB(コンスタントバッファ)のパラメータをシェーダーで使用
		 * false : PBRテクスチャの値を参照
		 */
		void SetCustomMaterial(bool flg) const;

	private:
		bool use_custom_parameter = true;

		std::unique_ptr<buffer::ConstantBuffer<MaterialCB>> cb_pbr_material{ nullptr };

		/*
		 * brief : MaterialPathでのデフォルトのファイル名を設定
		 *         コンスタントバッファで使用する
		 */
		void SetDefaultMaterialFilename();
	};

} // cumulonimbus::shader_asset
