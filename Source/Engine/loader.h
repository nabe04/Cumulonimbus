#pragma once
#include <filesystem>
#include <map>
#include <string>

#include "asset_sheet_manager.h"
#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	class Loader
	{
	public:
		explicit Loader() = default;
		virtual ~Loader() = default;
		virtual void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path) = 0;
		/**
		 * @brief : �w�肳�ꂽ�g���q�̓��[�h�\��
		 * @param extension : �t�@�C���̊g���q
		 * @return	: true -> �T�|�[�g���Ă���
		 * @return  : false -> �T�|�[�g���Ă��Ȃ�
		 */
		virtual bool Supported(std::filesystem::path extension) = 0;

	protected:
		/**
		 * @brief : �A�Z�b�g���Ǘ�����
		 * @remark key : UUID�𕶎���ɕϊ�������
		 * @remark value : �t�@�C���܂ł̃p�X
		 */
		std::map<mapping::rename_type::UUID, std::string> loader{};

		/**
		 * @param from : �R�s�[�Ώۂ̃t�@�C�����܂��̓t�H���_�܂ł̃p�X
		 * @param to   : �R�s�[��̃t�H���_�܂ł̃p�X
		 *					 (�� : ./Data/Assets/Texture/sample.png(���ۂɕۑ������p�X�ƃt�@�C����)
		 *						  -> ./Data/Assets/Texture(�R�s�[��̃t�H���_�܂ł̃p�X))
		 */
		template<class T>
		mapping::rename_type::UUID Convert(AssetSheetManager& sheet_manager, const std::filesystem::path& from,const std::filesystem::path& to) const
		{
			// �R�s�[��̃t�H���_�쐬&�R�s�[
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);

			const std::string copy_str = to.string() + "/" + from.filename().string();
			const std::filesystem::path copy_path{ copy_str };

			auto sheet = sheet_manager.GetSheet<T>().sheet;

			for (const auto& [key, value] : sheet_manager.GetSheet<T>().sheet)
			{
				if (copy_path.compare(value) == 0)
					return key;
			}

			mapping::rename_type::UUID id;
			while (true)
			{
				id = utility::GenerateUUID();
				if (sheet_manager.GetSheet<T>().sheet.contains(id))
					continue;
				break;
			}

			// �A�Z�b�g�V�[�g�̓o�^
			sheet_manager.GetSheet<T>().sheet.insert(std::make_pair(id, copy_path));
			return id;
		}
	};
} // cumulonimbus::asset

