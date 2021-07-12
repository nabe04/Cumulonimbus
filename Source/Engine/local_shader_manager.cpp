#include "local_shader_manager.h"

#include <cassert>

#include "shader_asset_mapping.h"
// shaders
#include "debug_collision.h"

namespace cumulonimbus::shader_asset
{
	LocalShaderAssetManager::LocalShaderAssetManager()
	{
		using namespace mapping::shader_assets;
		//ì¬‚µ‚½ShaderAsset‚Ì“o˜^
		RegisterLocalShaderAsset<shader_asset::DebugCollisionAsset>(LocalShaderAsset::Collision);
	}

	void LocalShaderAssetManager::BindCBuffer(mapping::shader_assets::LocalShaderAsset asset) const
	{
		if (!local_shader_asset_map.contains(asset))
			assert(!"No shader assets registered(LocalShaderAssetManager::BindCBuffer)");

		local_shader_asset_map.at(asset)->BindCBuffer();
	}

	void LocalShaderAssetManager::UnbindCBuffer(mapping::shader_assets::LocalShaderAsset asset) const
	{
		if (!local_shader_asset_map.contains(asset))
			assert(!"No shader assets registered(LocalShaderAssetManager::UnbindCBuffer)");

		local_shader_asset_map.at(asset)->UnbindCBuffer();
	}

	void LocalShaderAssetManager::BindTexture(mapping::shader_assets::LocalShaderAsset asset) const
	{
		if (!local_shader_asset_map.contains(asset))
			assert(!"No shader assets registered(LocalShaderAssetManager::BindTexture)");

		local_shader_asset_map.at(asset)->BindTexture();
	}

	void LocalShaderAssetManager::UnbindTexture(mapping::shader_assets::LocalShaderAsset asset) const
	{
		if (!local_shader_asset_map.contains(asset))
			assert(!"No shader assets registered(LocalShaderAssetManager::UnbindTexture)");

		local_shader_asset_map.at(asset)->UnbindTexture();
	}
} // cumulonimbus::shader_asset
