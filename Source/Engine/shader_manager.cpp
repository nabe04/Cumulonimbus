#include "shader_manager.h"

#include <imgui.h>

#include "scene.h"

// Shaders
// 3D
#include "debug_primitive_shader.h"
#include "3d_standard.h"
#include "sample_shader.h"
#include "diffuse.h"
#include "phong.h"
#include "metal.h"
#include "toon.h"
#include "reflection_mapping.h"
#include "refraction_mapping.h"
#include "single_color.h"
// 2D
#include "standard_sprite.h"
#include "billboard.h"
// Other
#include "debug_collision.h"

namespace cumulonimbus::shader_system
{
	ShaderManager::ShaderManager()
	{
		using namespace mapping::shader_assets;
		// Shaderのコンパイル
		// 3D
		//RegistryShader<shader_system::DebugCollisionShader>(ShaderAsset3D::DebugCollision);
		//RegistryShader<shader_system::SampleShader>				(ShaderAsset3D::SampleShader);
		//RegistryShader<shader_system::DiffuseShader>			(ShaderAsset3D::Diffuse);
		//RegistryShader<shader_system::PhongShader>				(ShaderAsset3D::Phong);
		//RegistryShader<shader_system::MetalShader>				(ShaderAsset3D::Metal);
		//RegistryShader<shader_system::ToonShader>				(ShaderAsset3D::Toon);
		//RegistryShader<shader_system::ReflectionMappingShader>	(ShaderAsset3D::ReflectionMapping);
		//RegistryShader<shader_system::RefractionMappingShader>	(ShaderAsset3D::RefractionMapping);
		//RegistryShader<shader_system::SingleColorShader>		(ShaderAsset3D::SingleColor);
		// 2D
		RegistryShader2D<shader_system::StandardSpriteShader, shader_asset::StandardSpriteAsset>(ShaderAsset2D::Standard);
		RegistryShader2D<shader_system::BillboardShader		, shader_asset::BillboardAsset>		(ShaderAsset2D::Billboard);
		// その他のシェーダー(Local Shader)
		//RegisterShader<shader_system::DebugCollisionShader>     (LocalShaderAsset::Collision);
	}

	//-------------------  3D用シェーダーのBind,Unbind  ----------------------//
	void ShaderManager::BindShader(const mapping::shader_assets::ShaderAsset3D asset) const
	{
		shader3d_map.at(asset)->BindShader();
		//shader3d_map.at(mapping::shader_assets::ShaderAsset3D::SampleShader)->BindShader();
	}

	void ShaderManager::UnbindShader(const mapping::shader_assets::ShaderAsset3D asset) const
	{
		shader3d_map.at(asset)->UnbindShader();
	}

	//-------------------  2D用シェーダーのBind,Unbind  ----------------------//
	mapping::shader_assets::ShaderAsset2D ShaderManager::GetAsset2DFromConnector(const mapping::rename_type::Hash& hash) const
	{
		if (!connector_shader2d.contains(hash))
			assert(!"Don't have key value");

		return connector_shader2d.at(hash);
	}


	void ShaderManager::BindShader2D(const mapping::shader_assets::ShaderAsset2D asset)
	{
		if (!shader2d_map.contains(asset))
			assert(!"Don't have shader asset(ShaderManager::BindShader2D)");

		shader2d_map.at(asset)->BindShader();
	}

	void ShaderManager::UnbindShader2D(const mapping::shader_assets::ShaderAsset2D asset)
	{
		if (!shader2d_map.contains(asset))
			assert(!"Don't have shader asset(ShaderManager::BindShader2D)");

		shader2d_map.at(asset)->UnbindShader();
	}

	//------------------ モデル以外のシェーダーのBind,Unbind -----------------//
	void ShaderManager::BindLocalShader(mapping::shader_assets::LocalShaderAsset asset)
	{
		local_shader_map.at(asset)->BindShader();
	}

	void ShaderManager::UnbindLocalShader(mapping::shader_assets::LocalShaderAsset asset)
	{
		local_shader_map.at(asset)->UnbindShader();
	}
} // cumulonimbus::shader_system
