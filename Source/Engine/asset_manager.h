#pragma once
#include <memory>
#include <filesystem>

#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	class AssetSheetManager;
	class Loader;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	class AssetManager final
	{
	public:
		AssetManager();

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(sheet_manager),
				CEREAL_NVP(connector)
			);
		}

		void AddAsset(const std::filesystem::path& path);

		/**
		 * @breif : �A�Z�b�g�V�[�g�̕ۑ�
		 * @remark : save_path(������)���󔒂ɂ����ꍇ�f�t�H���g�̃p�X
		 *			(./Data/AssetSheet/asset_sheet.sheet)�ɕۑ������
		 * @param save_path : �ۑ���̃p�X(���g���q�u.sheet�v�܂�)
		 */
		void Save(const std::filesystem::path& save_path = {""});

		/**
		 * @brief : T�^��Loader�̎擾
		 * @remark : T�^��Loader�N���X����h�������N���X���w�肷��K�v������
		 * @return : T�^�̃|�C���^
		 */
		template<class T>
		T* GetLoader()
		{
			return static_cast<T*>(loaders.at(typeid(T).hash_code()).get());
		}

		[[nodiscard]]
		AssetSheetManager& GetAssetSheetManager() const { return *sheet_manager.get(); }

		/**
		 * @brief : Loader��connector�̓o�^
		 * @remark : AssetT : �o�^����A�Z�b�g�N���X(Texture,Model�Ȃ�)
		 * @remark : LoaderT : AssetT�^�C�v�ɑΉ�����Loader�N���X
		 */
		template<class AssetT,class LoaderT>
		void RegisterLoader()
		{
			// Loader�̓o�^
			if (!loaders.contains(utility::GetHash<LoaderT>()))
				loaders.insert(std::make_pair(utility::GetHash<LoaderT>(), std::make_unique<LoaderT>()));

			if (!connector.contains(utility::GetHash<AssetT>()))
				// �R�l�N�^�[�̓o�^
				connector.insert(std::make_pair(
					utility::GetHash<AssetT>(),
					utility::GetHash<LoaderT>()));
		}
	private:
		std::unique_ptr<AssetSheetManager>   sheet_manager{};
		/**
		 * @brief : �Ǘ�����Loader���Ƃ̃}�b�v
		 * @remark : key : �o�^����Loader�N���X�̃n�b�V���l
		 * @remark : value : Loader�̃|�C���^
		 */
		std::map<mapping::rename_type::Hash, std::unique_ptr<Loader>> loaders{};
		/**
		 * @brief : sheets�ŊǗ�����A�Z�b�g�̃n�b�V���l�ƌ��т�
		 *			AssetSheetManager::loaders�̃n�b�V���l�̃}�b�v
		 * @remark : AssetSheetManager::Load����Loader�ւ̃R�l�N�^�[�Ƃ��Ďg�p
		 * @remark : key : �A�Z�b�g�̃n�b�V���l
		 * @remark : value : Loader�̃n�b�V���l
		 */
		std::map<mapping::rename_type::Hash, mapping::rename_type::Hash> connector{};

		/**
		 * @brief : �A�Z�b�g�V�[�g�̓ǂݍ���
		 * @ramark : load_path(������)���󔒂ɂ����ꍇ�f�t�H���g�̃p�X�ɂ���
		 *			 �t�H���_�̍ŏ��Ƀq�b�g�����u.sheet�v�g���q�̃t�@�C����ǂݍ���
		 * @param load_path : �ǂݍ��ݐ�̃p�X(�g���q�u.sheet�v�܂�)
		 */
		void Load(const std::filesystem::path& load_path = { "" });

		/**
		 * @brief : sheet_manager�̏��(�A�Z�b�g�܂ł̃p�X)������
		 *			�A�Z�b�g��Loader�Ƀ��[�h
		 */
		void AssetLoad();
 	};
} // cumulonimbus::asset
