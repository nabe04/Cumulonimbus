#include "shader_assets_component.h"

#include "3d_standard.h"
#include "diffuse.h"
#include "phong.h"
#include "metal.h"
#include "toon.h"
#include "reflection_mapping.h"
#include "refraction_mapping.h"
#include "single_color.h"

namespace cumulonimbus::component
{
	ShaderAssets3DComponent::ShaderAssets3DComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
		using namespace mapping::shader_assets;
		//çÏê¨ÇµÇΩShaderAssetÇÃìoò^
		RegisterShaderAsset<shader_asset::Standard3DAsset>(ShaderAsset3D::Standard);
		RegisterShaderAsset<shader_asset::DiffuseAsset>(ShaderAsset3D::Diffuse);
		RegisterShaderAsset<shader_asset::PhongAsset>(ShaderAsset3D::Phong);
		RegisterShaderAsset<shader_asset::MetalAsset>(ShaderAsset3D::Metal);
		RegisterShaderAsset<shader_asset::ToonAsset>(ShaderAsset3D::Toon);
		RegisterShaderAsset<shader_asset::ReflectionMappingAsset>(ShaderAsset3D::ReflectionMapping);
		RegisterShaderAsset<shader_asset::RefractionMappingAsset>(ShaderAsset3D::RefractionMapping);
		RegisterShaderAsset<shader_asset::SingleColorAsset>(ShaderAsset3D::SingleColor);
	}

	void ShaderAssets3DComponent::RenderImGui()
	{

	}

	void ShaderAssets3DComponent::RenderImGui(mapping::shader_assets::ShaderAsset3D asset)
	{
		shader_asset_map.at(asset)->RenderImGui();
	}

}