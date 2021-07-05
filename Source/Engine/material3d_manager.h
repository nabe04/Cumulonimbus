#pragma once
#include <unordered_map>
#include <memory>

#include "enum_state_map.h"
#include "shader_asset.h"
#include "shader_asset_mapping.h"

namespace cumulonimbus::shader_asset
{
	class Material3DManager
	{
	public:
		explicit Material3DManager();
		~Material3DManager() = default;

		/*
		 * brief : 現在セットされているシェーダーの
		 *		   コンスタントバッファのセット(バインド)
		 */
		void BindCBuffer() const;
		/*
		 * brief : 現在のセットされているシェーダーの
		 *		    コンスタントバッファをリセット(アンバインド)
		 */
		void UnbindCBuffer() const;

		/*
		 * brief : 現在セットされているシェーダーの
		 *		   テクスチャのセット(バインド)
		 */
		void BindTexture() const;
		/*
		 * brief : 現在　セットされているシェーダーの
		 *		   テクスチャのリセット(アンバインド)
		 */
		void UnbindTexture() const;


		/*
		 * brief : ShaderAssetが持つGBuffer用のシェーダースロットのセット(バインド)
		 *		   ライティング時に使用
		 */
		void BindGBuffShaderSlot()   const;
		void UnbindGBuffShaderSlot() const;

		/*
		 * brief     : Shader全体の種類を変更するためのImGui
		 */
		void RenderImGuiComboShader();
		/*
		 * brief : mapping::shader_assets::ShaderAsset3Dで指定されたshader個々の
		 *		  ImGui(パラメータ)表示
		 */
		void RenderImGuiShaderParameter();

		/*
		 * brief : shader_asset_mapに登録されている全ての
		 *         ShaderAsset3Dのmaterial_pathのセット
		 */
		void SetMaterialPathForAllShaderAsset3D(const shader_asset::MaterialPath& material_path);

		/*
		 * brief : assetで指定されたShaderAsset3Dの
		 *         material_pathのセット
		 */
		void SetMaterialPath(mapping::shader_assets::ShaderAsset3D asset, const shader_asset::MaterialPath& material_path);

		[[nodiscard]] mapping::shader_assets::ShaderAsset3D GetCurrentAsset() const { return shader_assets.GetCurrentState(); }

	private:
		std::unordered_map<mapping::shader_assets::ShaderAsset3D, std::shared_ptr<shader_asset::ShaderAsset>> shader_asset_map{};
		EnumStateMap<mapping::shader_assets::ShaderAsset3D> shader_assets;

		template<typename T>
		void RegisterShaderAsset(mapping::shader_assets::ShaderAsset3D asset_type, u_int gbuffer_shader_slot)
		{
			if (shader_asset_map.contains(asset_type))
				return;

			shader_asset_map.emplace(asset_type, std::make_shared<T>(gbuffer_shader_slot));
		}
	};
}
