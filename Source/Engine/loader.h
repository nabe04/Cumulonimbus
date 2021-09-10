#pragma once
#include <filesystem>
#include <map>
#include <string>

#include "asset_sheet_manager.h"
#include "rename_type_mapping.h"

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

		virtual mapping::rename_type::UUID Convert(const std::filesystem::path& path) = 0;
	};
} // cumulonimbus::asset

