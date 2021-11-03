#include "texture_loader.h"

#include <set>
#include <filesystem>

#include "asset_sheet_manager.h"
#include "cum_imgui_helper.h"
#include "texture.h"
#include "locator.h"

namespace
{
	// �e�N�X�`����ۑ�����܂ł̃p�X
	const std::filesystem::path save_parent_path = "./Data/Assets/Textures";
}

namespace cumulonimbus::asset
{
	TextureLoader::TextureLoader()
	{
		const DirectX::SimpleMath::Vector4 color = { 1.f,1.f,1.f,1.f };
		dummy_texture = std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(), color);
	}

	mapping::rename_type::UUID TextureLoader::Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to)
	{
		if (!equivalent(from.parent_path(), to))
		{
			// �R�s�[��̃t�H���_�쐬&�R�s�[
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet)
		{// �����A�Z�b�g�����݂��Ă����ꍇ���̃A�Z�b�g��ID(UUID)��Ԃ�
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)�̏d�����Ȃ����m�F
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.contains(id))
				continue;
			break;
		}

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	void TextureLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void TextureLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& from,

		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void TextureLoader::Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id)
	{
		// ���łɃe�N�X�`�������݂���ꍇ�͏����𔲂���
		if (textures.contains(id))
			return;

		// �e�N�X�`���̍쐬
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
									  asset_manager.GetAssetSheetManager().GetAssetFilename<Texture>(id).c_str()))
		);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
	}

	void TextureLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// �A�Z�b�g�����݂��Ȃ��ꍇ�����𔲂���
		if (!textures.contains(asset_id))
			return;

		// �e�N�X�`���g���q
		const std::string extension				= std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Texture>(asset_id) }.extension().string();
		// �ύX�O�̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Textures/"�ύX�O�̃e�N�X�`����"
		const std::filesystem::path before_path	= std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Texture>(asset_id) }.replace_extension();
		// �ύX��̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Textures/"�ύX��̃e�N�X�`����"
		const std::filesystem::path after_path	= before_path.parent_path().string() + "/" + changed_name;

		// �t�@�C�����̕ύX
		// �� : ./Data/Textures/"�ύX�O�̃e�N�X�`����" -> ./Data/Textures/"�ύX��̃e�N�X�`����"
		std::filesystem::rename(before_path.string() + extension,
								after_path.string()  + extension);

		// �A�Z�b�g�V�[�g���̃t�@�C���p�X�ύX(�� : ./Data/Materials/"�ύX��̃}�e���A����"/"�ύX��̃}�e���A����.mat")
		asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.at(asset_id) = after_path.string() + extension;
		// �A�Z�b�g�V�[�g�̕ۑ�
		asset_manager.Save();
	}

	void TextureLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID tex_id = asset_manager.GetAssetSheetManager().Search<Texture>(path);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!textures.contains(tex_id))
			return;

		DeleteTexture(tex_id, path);
		asset_manager.Save();
	}

	void TextureLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Texture>(asset_id);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!textures.contains(asset_id))
			return;

		DeleteTexture(asset_id, path);
		asset_manager.Save();
	}

	bool TextureLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".png" , ".PNG",
			".jpeg", ".JPEG",
			".tga" , ".TGA",
			".dds" , ".DDS"
		};
		return extensions.contains(extension);
	}

	void TextureLoader::CreateTexture(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet)
		{
			if (path.compare(value) == 0)
				return;
		}

		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.contains(id))
				continue;
			break;
		}

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.insert(std::make_pair(id, path.string()));

		// ���łɃe�N�X�`�������݂���ꍇ�͏����𔲂���
		if (textures.contains(id))
			return;

		// �e�N�X�`���̍쐬
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
				asset_manager.GetAssetSheetManager().GetAssetFilename<Texture>(id).c_str()))
		);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
	}

	Texture& TextureLoader::GetTexture(const mapping::rename_type::UUID& id)
	{
		if (!textures.contains(id))
			return *dummy_texture.get();
			//assert(!"Not found texture id(TextureLoader::GetTexture)");

		return *textures.at(id).get();
	}

	void TextureLoader::SelectableTexture(AssetManager& asset_manager, mapping::rename_type::UUID& tex_id)
	{
		std::filesystem::path texture_filename{};
		bool is_dummy = false;
		const asset::AssetSheet& texture_sheet = asset_manager.GetAssetSheetManager().GetSheet<asset::Texture>();
		if(texture_sheet.sheet.contains(tex_id))
		{
			texture_filename = texture_sheet.sheet.at(tex_id);
			texture_filename.filename().replace_extension();
			is_dummy = true;
		}
		else
		{
			texture_filename = "None";
		}

		if(ImGui::BeginCombo("Textures",texture_filename.string().c_str()))
		{
			ImGui::SliderFloat("##Size", &selectable_magnification, .5f, 5.f, "Size : %.2f");
			{// �_�~�[�e�N�X�`���p
				helper::imgui::Image({}, { selectable_image_size.x ,
										   selectable_image_size.y  });
				ImGui::SameLine();
				if(ImGui::Selectable(texture_filename.string().c_str(),
									 is_dummy,0,
									 {selectable_size.x,
									  selectable_size.y }))
				{
					tex_id = {""};
				}
				if(is_dummy)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			for (const auto& [id, tex_filepath] : texture_sheet.sheet)
			{
				helper::imgui::Image(id, { selectable_image_size.x,
										   selectable_image_size.y });
				ImGui::SameLine();
				const bool is_selected = (tex_id == id);
				std::filesystem::path tex_filename = tex_filepath;
				if (ImGui::Selectable(tex_filename.filename().replace_extension().string().c_str(),
									  is_selected, 0,
									  { selectable_size.x,
									    selectable_size.y }))
				{
					tex_id = id;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	void TextureLoader::DeleteTexture(const mapping::rename_type::UUID& tex_id, const std::filesystem::path& delete_path)
	{
		textures.erase(tex_id);
		// �t�@�C���̍폜
		std::filesystem::remove(delete_path);
	}

} // cumulonimbus::asset