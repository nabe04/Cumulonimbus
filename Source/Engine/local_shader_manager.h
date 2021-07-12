#pragma once
#include <unordered_map>
#include <memory>

#include "shader_asset.h"
#include "shader_asset_mapping.h"

namespace cumulonimbus::shader_asset
{
	class LocalShaderAssetManager final
	{
	public:
		explicit LocalShaderAssetManager();
		~LocalShaderAssetManager() = default;

		/*
		 * brief : �w���LocalShaderAsset�����R���X�^���g�o�b�t�@�̃f�[�^�̃Z�b�g
		 */
		template<typename T>
		void SetCBufferData(mapping::shader_assets::LocalShaderAsset asset)
		{
			//local_shader_asset_map.at(asset)->
		}

		/*
		 * brief : ���݃Z�b�g����Ă���V�F�[�_�[��
		 *		   �R���X�^���g�o�b�t�@�̃Z�b�g(�o�C���h)
		 */
		void BindCBuffer(mapping::shader_assets::LocalShaderAsset asset) const;
		/*
		 * brief : ���݂̃Z�b�g����Ă���V�F�[�_�[��
		 *		    �R���X�^���g�o�b�t�@�����Z�b�g(�A���o�C���h)
		 */
		void UnbindCBuffer(mapping::shader_assets::LocalShaderAsset asset) const;

		/*
		 * brief : ���݃Z�b�g����Ă���V�F�[�_�[��
		 *		   �e�N�X�`���̃Z�b�g(�o�C���h)
		 */
		void BindTexture(mapping::shader_assets::LocalShaderAsset asset) const;
		/*
		 * brief : ���݁@�Z�b�g����Ă���V�F�[�_�[��
		 *		   �e�N�X�`���̃��Z�b�g(�A���o�C���h)
		 */
		void UnbindTexture(mapping::shader_assets::LocalShaderAsset asset) const;

	private:
		std::unordered_map<mapping::shader_assets::LocalShaderAsset, std::unique_ptr<shader_asset::ShaderAsset>> local_shader_asset_map{};

		template<typename T>
		void RegisterLocalShaderAsset(mapping::shader_assets::LocalShaderAsset asset_type)
		{
			if (local_shader_asset_map.contains(asset_type))
				return;

			local_shader_asset_map.emplace(asset_type, std::make_unique<T>());
		}
	};
} // cumulonimbus::shader_asset