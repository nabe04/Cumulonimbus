#include "shader_asset.h"

#include "locator.h"

namespace cumulonimbus::shader_asset
{
	void ShaderAsset::ModifyMaterialPath(std::string& material_filename, const std::string& combo_label)
	{
		locator::Locator::GetTextureResourceManager()->ModifyTextureFilename(material_filename, combo_label);
	}

	//----------------------  �Z�b�^�[  ------------------------//

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

	void ShaderAsset::SetPBRMapName(const std::string& name)
	{
		material_path.pbr_map_name = name;
	}

	void ShaderAsset::SetRoughnessMapName(const std::string& name)
	{
		material_path.roughness_map_name = name;
	}

	void ShaderAsset::SetMetalnessMapName(const std::string& name)
	{
		material_path.metalness_map_name = name;
	}

	void ShaderAsset::SetSpecularMapName(const std::string& name)
	{
		material_path.specular_map_name = name;
	}

	void ShaderAsset::SetAOMapName(const std::string& name)
	{
		material_path.ao_map_name = name;
	}


	//----------------------  �Q�b�^�[  ------------------------//

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

	const std::string& ShaderAsset::GetPBRMapName() const
	{
		return material_path.pbr_map_name;
	}

	const MaterialPath& ShaderAsset::GetMaterialPath() const
	{
		return material_path;
	}

	const std::string& ShaderAsset::GetMetalnessMapName() const
	{
		return material_path.metalness_map_name;
	}

	const std::string& ShaderAsset::GetSpecularMapName() const
	{
		return material_path.specular_map_name;
	}

	const std::string& ShaderAsset::GetAOMapName() const
	{
		return material_path.ao_map_name;
	}

} // cumulonimbus::shader_asset