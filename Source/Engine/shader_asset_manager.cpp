#include "shader_asset_manager.h"

// 2D Shader Asset
#include "standard_sprite.h"
#include "billboard.h"

namespace cumulonimbus::shader_asset
{
	ShaderAsset2DManager::ShaderAsset2DManager()
	{
		RegisterShaderAsset<shader_asset::StandardSpriteAsset>();
		RegisterShaderAsset<shader_asset::BillboardAsset>();
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
