#pragma once
#include <map>
#include <string>
#include <filesystem>
#include <cassert>

#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	class Loader;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	struct AssetSheet
	{
		AssetSheet() = default;
		/**
		 * @brief : �A�Z�b�g�̃t�@�C���p�X�V�[�g
		 * @remark key : �A�Z�b�gID
		 * @remark value : �A�Z�b�g�܂ł̃t�@�C���p�X
		 */
		std::map<mapping::rename_type::UUID, std::string> sheet{};

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(sheet)
			);
		}
	};

	class AssetSheetManager final
	{
	public:
		explicit AssetSheetManager();
		~AssetSheetManager() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(sheets)
			);
		}

		/**
		 * @brief  : �C�ӂ̃N���X�̃A�Z�b�g�V�[�g�̎擾
		 * @remark : T�^�����݂��Ȃ��ꍇ�����𒆒f����
		 */
		template<class T>
		[[nodiscard]]
		AssetSheet& GetSheet()
		{
			const auto hash = utility::GetHash<T>();
			if (!sheets.contains(hash))
				assert(!"Not registered sheet(AssetSheet::GetSheet)");

			return sheets.at(hash);
		}

		[[nodiscard]]
		std::map<mapping::rename_type::Hash, AssetSheet> & GetSheets()
		{
			return sheets;
		}

		/**
		 * @brief : �C�ӂ̃N���X�̃A�Z�b�g�V�[�g�ɓo�^����Ă���
		 *			�t�@�C���p�X + �t�@�C���� + �g���q�̎擾
		 * @remark : �E������id��AssetSheet��sheet(std::map)�̃L�[�l�ɂȂ�
		 * @remark : �ET�^�����݂��Ȃ��ꍇ�����𒆒f����
		 * @param id : �A�Z�b�g��UUID�l
		 * @return : �A�Z�b�g�̃t�@�C���p�X + �t�@�C���� + �g���q
		 */
		template<class T>
		[[nodiscard]]
		std::string GetAssetFilename(const mapping::rename_type::UUID id) const
		{
			const auto hash = utility::GetHash<T>();
			if (!sheets.contains(hash))
				assert(!"Not registered sheet(AssetSheet::GetAssetFilename)");

			return sheets.at(hash).sheet.at(id);
		}

		/**
		 * @brief : �A�Z�b�g�V�[�g�ɓo�^���Ă���p�X����ID������
		 * @remark : �p�X���q�b�g���Ȃ������ꍇ�ł���O�����͏o�Ȃ�
		 * @remark : �p�X���q�b�g���Ȃ������ꍇ�u""�v���ς���
		 * @param path : �����������t�@�C���p�X
		 * @return : �����Ńq�b�g�����p�X��ID
		 */
		template<class T>
		[[nodiscard]]
		mapping::rename_type::UUID Search(const std::filesystem::path& path) const
		{
			for (const auto& [key, value] : sheets.at(utility::GetHash<T>()).sheet)
			{
				// �p�X�����݂���΃p�X�̎���ID��Ԃ�
				if (path.compare(value) == 0)
					return key;
			}

			return "";
		}

	private:
		/**
		 * @brief : (�Ǘ���������)�^���Ƃ̃V�[�g
		 * @remark key : �o�^����A�Z�b�g�N���X(Texture,Model�Ȃ�)�̃n�b�V���l
		 * @remark value : AssetSheet�\����
		 */
		std::map<mapping::rename_type::Hash, AssetSheet> sheets{};

		/**
		 * @breif : sheets�̓o�^
		 * @remark : T : �o�^����A�Z�b�g�N���X(Texture,Model�Ȃ�)
		 */
		template<class T>
		void Register();
	};
} // cumulonimbus::asset
