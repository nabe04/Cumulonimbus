#pragma once
#include <unordered_map>

#include "enum_state_map.h"
#include "shader_asset.h"
#include "shader_asset_mapping.h"

namespace cumulonimbus::shader_asset
{
	class Material3DManager final
	{
	public:
		explicit Material3DManager();
		~Material3DManager() = default;

		// コンスタントバッファのバインド、アンバインド
		void BindAsset(mapping::shader_assets::ShaderAsset3D asset);
		void UnbindAsset(mapping::shader_assets::ShaderAsset3D asset);

		/*
		 * brief     : Shader全体の種類を変更するためのImGui
		 */
		void RenderImGui();
		/*
		 * brief : mapping::shader_assets::ShaderAsset3Dで指定されたshader個々の
		 *		  ImGui(パラメータ)表示
		 */
		void RenderImGui(mapping::shader_assets::ShaderAsset3D asset);

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
		std::unordered_map<mapping::shader_assets::ShaderAsset3D, shader_asset::ShaderAsset> shader_asset_map{};
		EnumStateMap<mapping::shader_assets::ShaderAsset3D> shader_assets;

		template<typename T>
		void RegisterShaderAsset(mapping::shader_assets::ShaderAsset3D asset_type)
		{
			if (shader_asset_map.contains(asset_type))
				return;

			shader_asset_map.emplace(asset_type, T());
		}
	};
}
