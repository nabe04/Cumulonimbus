#pragma once
#include <filesystem>
#include <map>
#include <string>

#include "asset_manager.h"
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

		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		virtual void Load(AssetManager& asset_manager, const std::filesystem::path& path) = 0;
		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param from: ���[�h���ꂽ���f���̃t�@�C���p�X
		 * @param to : �R�s�[��̃t�@�C���p�X
		 */
		virtual void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) = 0;

		/**
		 * @brief : �I�[�o�[���[�h����Ă���Load�֐��̋��ʏ����L�q��
		 */
		virtual void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) = 0;
		
		/**
		 * @brief : �w�肳�ꂽ�g���q�̓��[�h�\��
		 * @param extension : �t�@�C���̊g���q
		 * @return	: true -> �T�|�[�g���Ă���
		 * @return  : false -> �T�|�[�g���Ă��Ȃ�
		 */
		virtual bool Supported(std::filesystem::path extension) = 0;
	protected:
		/**
		 * @param asset_manager :  AssetManager�N���X�̎Q��
		 * @param from : �R�s�[�Ώۂ̃t�@�C�����܂��̓t�H���_�܂ł̃p�X
		 * @param to   : �R�s�[��̃t�H���_�܂ł̃p�X
		 *					 (�� : ./Data/Assets/Texture/sample.png(���ۂɕۑ������p�X�ƃt�@�C����)
		 *						  -> ./Data/Assets/Texture(�R�s�[��̃t�H���_�܂ł̃p�X))
		 */
		virtual mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) = 0;

		/**
		 * @brief : �����t�H���_�K�w�̃t�@�C�������擾��
		 *			���O���d�����Ă����ꍇ�u�d����(�ԍ�).�g���q�v
		 *			�Ƃ����`�ɂ���
		 * @param asset_manager :  AssetManager�N���X�̎Q��
		 * @param path : �t�@�C���p�X(�g���q���܂�)
		 */
		template<class T>
		[[nodiscard]]
		std::filesystem::path CompareAndReName(const AssetManager& asset_manager, std::filesystem::path path) const
		{
			int no = 0;
			bool is_loop = true;
			const std::string exe = path.extension().string();
			std::string filename = path.replace_extension().string();
			if(asset_manager.GetAssetSheetManager().GetSheet<T>().sheet.size() == 0)
			{
				return filename;
			}

			while(is_loop)
			{
				for(const auto&[key,value] : asset_manager.GetAssetSheetManager().GetSheet<T>().sheet)
				{
					if (std::filesystem::path{ value }.parent_path().replace_extension().compare(filename) != 0)
					{
						is_loop = false;
					}
					else
					{
						is_loop = true;
						filename = path.string();
						filename += "(" + std::to_string(++no) + ")" + exe;
						break;
					}
				}
			}

			return filename;
		}
	};
} // cumulonimbus::asset

