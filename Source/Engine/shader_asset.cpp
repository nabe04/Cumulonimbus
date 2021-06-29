#include "shader_asset.h"

namespace cumulonimbus::shader_asset
{
	void ShaderAsset::SetMaterialPath(const MaterialPath& mat_path)
	{
		material_path = mat_path;
	}

	void ShaderAsset::SetAlbedoMapName(const std::string& name)
	{
		material_path.albedo_map_name = name;
	}

	void ShaderAsset::SetNormalMapName(const std::string& name)
	{
		material_path.normal_map_name = name;
	}

	void ShaderAsset::SetRoughnessMapName(const std::string& name)
	{
		material_path.roughness_map_name = name;
	}

	const std::string& ShaderAsset::GetAlbedoMapName() const
	{
		return material_path.albedo_map_name;
	}

	const std::string& ShaderAsset::GetNormalMapName() const
	{
		return material_path.normal_map_name;
	}

	const std::string& ShaderAsset::GetRoughnessMapName() const
	{
		return material_path.roughness_map_name;
	}

	const MaterialPath& ShaderAsset::GetMaterialPath() const
	{
		return material_path;
	}
} // cumulonimbus::shader_asset