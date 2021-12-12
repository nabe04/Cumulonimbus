#include "material_loader.h"

#include "asset_sheet_manager.h"
#include "file_path_helper.h"
#include "locator.h"

namespace
{
	// �e�N�X�`����ۑ�����܂ł̃p�X
	const std::filesystem::path copy_dir = "./Data/Assets/Materials";
}


namespace cumulonimbus::asset
{
	MaterialLoader::MaterialLoader()
	{
		dummy_material = std::make_unique<Material>();
	}

	void MaterialLoader::Load(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, copy_dir);
		Load(asset_manager, id);
	}

	void MaterialLoader::Load(AssetManager& asset_manager, const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void MaterialLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// �A�Z�b�g�����݂��Ȃ��ꍇ�����𔲂���
		if (!materials.contains(asset_id))
			return;

		// �ύX�O�̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Materials/"�ύX�O�̃}�e���A����"
		const std::filesystem::path before_path			= std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Material>(asset_id) }.replace_extension();
		// �ύX�O�̐e�t�@�C���p�X(�g���q���܂܂Ȃ�) ./Data/Materials/
		const std::filesystem::path before_parent_path	= before_path.parent_path();

		// �d���m�F�ς݂̃t�@�C�����擾(�g���q���܂܂Ȃ�)
		// asset_name = �d���m�F�ς݂̃}�e���A����(�t�@�C�����̂݁A�g���q���܂܂Ȃ�)
		const std::string asset_name = CompareAndReName<Material>(asset_manager,
																  before_parent_path.string() + "/" +
																  changed_name +file_path_helper::GetMaterialExtension()
																  ).filename().replace_extension().string();

		// �ύX��̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Materials/"�ύX��̃}�e���A����"
		const std::filesystem::path after_path			= before_parent_path.string() + "/" + asset_name;

		// -- �t�@�C�����̕ύX --//
		// �u.mat�v�t�@�C���̃t�@�C�����ύX
		//  �� : ./Data/Materials/"�ύX�O�̃}�e���A����.mat" ->  ./Data/Materials/"�ύX��̃}�e���A����.mat"
		std::filesystem::rename(before_path.string() + file_path_helper::GetMaterialExtension(),
								after_path.string()  + file_path_helper::GetMaterialExtension());
		// �u.json�v�t�@�C���̃t�@�C�����ύX
		// �� : ./Data/Materials/"�ύX�O�̃}�e���A����.json" ->  ./Data/Materials/"�ύX��̃}�e���A����.json"
		std::filesystem::rename(before_path.string() + file_path_helper::GetJsonExtension(),
								after_path.string()  + file_path_helper::GetJsonExtension());

		// �A�Z�b�g�V�[�g���̃t�@�C���p�X�ύX(�� : ./Data/Materials/"�ύX��̃}�e���A����"/"�ύX��̃}�e���A����.mat")
		asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.at(asset_id) = after_path.string() + file_path_helper::GetMaterialExtension();
		// �A�Z�b�g�V�[�g�̕ۑ�
		asset_manager.Save();
	}

	void MaterialLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID mat_id = asset_manager.GetAssetSheetManager().Search<asset::Material>(path);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!materials.contains(mat_id))
			return;

		DeleteMaterial(mat_id, path);
		asset_manager.Save();
	}

	void MaterialLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Material>(asset_id);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!materials.contains(asset_id))
			return;

		DeleteMaterial(asset_id, path);
		asset_manager.Save();
	}

	bool MaterialLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".mat"
		};
		return extensions.contains(extension);
	}

	mapping::rename_type::UUID MaterialLoader::CreateMaterial(
		AssetManager& asset_manager, const std::filesystem::path& parent_path,
		const MaterialData& material_data, std::string material_name)
	{
		// �}�e���A�����������̏ꍇ�uNew Material�v�ɖ��O��ݒ�
		if(material_name == "")
		{
			const std::string path = parent_path.string() + "/Materials" + "/New Material" + file_path_helper::GetMaterialExtension();
			material_name = CompareAndReName<Material>(asset_manager, path).filename().replace_extension().string();
		}
		const std::string save_path{
			parent_path.string() + "/"
			+ "Materials" + "/"
			+ material_name + file_path_helper::GetMaterialExtension() };


		// �ۑ������}�e���A����ID���쐬
		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.contains(id))
				continue;
			break;
		}

		// �쐬�����}�e���A�����u.mat�v�`���ŕۑ�
		std::unique_ptr<Material>mat = std::make_unique<Material>(id, material_data);
		mat->Save(save_path);

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.insert(std::make_pair(id, save_path));
		// �}�e���A���̒ǉ�
		materials.insert(std::make_pair(
			id,
			std::move(mat))
		);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
		return id;
	}

	void MaterialLoader::Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id)
	{
		// ���łɃe�N�X�`�������݂���ꍇ�͏����𔲂���
		if (materials.contains(id))
			return;
		Material m{ id };
		m.Load(asset_manager.GetAssetSheetManager().GetAssetFilename<Material>(id));
		// �}�e���A���̍쐬
		materials.insert(std::make_pair(
			id,
			std::make_unique<Material>(m))
		);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
	}

	mapping::rename_type::UUID MaterialLoader::Convert(AssetManager& asset_manager,
		const std::filesystem::path& from, const std::filesystem::path& to)
	{
		// �R�s�[��̃t�H���_�쐬&�R�s�[
		std::filesystem::copy(
			from, to,
			std::filesystem::copy_options::recursive |
			std::filesystem::copy_options::overwrite_existing);

		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		auto sheet = asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet;

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet)
		{
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.contains(id))
				continue;
			break;
		}

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	Material& MaterialLoader::GetMaterial(const mapping::rename_type::UUID& mat_id)
	{
		if (!materials.contains(mat_id))
			return *dummy_material.get();
			//assert(!"Not found material id(MaterialLoader::GetMaterial)");
		return *materials.at(mat_id).get();
	}

	std::string MaterialLoader::GetMaterialName(const mapping::rename_type::UUID& mat_id) const
	{
		if (!materials.contains(mat_id))
			return { "" };

		const std::filesystem::path mat_path = locator::Locator::GetAssetManager()->GetAssetSheetManager().GetAssetFilename<Material>(mat_id);

		return mat_path.filename().replace_extension().string();
	}


	void MaterialLoader::DeleteMaterial(const mapping::rename_type::UUID& mat_id, const std::filesystem::path& delete_path)
	{
		// �o�^�����}�e���A�����̍폜
		materials.erase(mat_id);
		// �t�@�C��(.mat)�̍폜
		std::filesystem::remove(delete_path);
		// �t�@�C��(.json)�̍폜
		std::filesystem::path p = delete_path;
		p.replace_extension();
		std::filesystem::remove(p.string() + file_path_helper::GetJsonExtension());
	}

} // cumulonimbus::asset
