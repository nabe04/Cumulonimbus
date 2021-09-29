#include "shader_asset_manager.h"

#include "standard_sprite.h"

namespace cumulonimbus::shader_asset
{
	ShaderAsset2DManager::ShaderAsset2DManager()
	{
		RegisterShaderAsset<shader_asset::StandardSpriteAsset>();
	}

	void ShaderAsset2DManager::BindCBuffer() const
	{
		shader_assets.at(current_asset_hash)->BindCBuffer();
	}

	void ShaderAsset2DManager::BindTexture() const
	{
		shader_assets.at(current_asset_hash)->UnbindCBuffer();
	}
} // cumulonimbus::shader_asset
