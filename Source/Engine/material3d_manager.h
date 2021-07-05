#pragma once
#include <unordered_map>
#include <memory>

#include "enum_state_map.h"
#include "shader_asset.h"
#include "shader_asset_mapping.h"

namespace cumulonimbus::shader_asset
{
	class Material3DManager
	{
	public:
		explicit Material3DManager();
		~Material3DManager() = default;

		/*
		 * brief : ���݃Z�b�g����Ă���V�F�[�_�[��
		 *		   �R���X�^���g�o�b�t�@�̃Z�b�g(�o�C���h)
		 */
		void BindCBuffer() const;
		/*
		 * brief : ���݂̃Z�b�g����Ă���V�F�[�_�[��
		 *		    �R���X�^���g�o�b�t�@�����Z�b�g(�A���o�C���h)
		 */
		void UnbindCBuffer() const;

		/*
		 * brief : ���݃Z�b�g����Ă���V�F�[�_�[��
		 *		   �e�N�X�`���̃Z�b�g(�o�C���h)
		 */
		void BindTexture() const;
		/*
		 * brief : ���݁@�Z�b�g����Ă���V�F�[�_�[��
		 *		   �e�N�X�`���̃��Z�b�g(�A���o�C���h)
		 */
		void UnbindTexture() const;


		/*
		 * brief : ShaderAsset������GBuffer�p�̃V�F�[�_�[�X���b�g�̃Z�b�g(�o�C���h)
		 *		   ���C�e�B���O���Ɏg�p
		 */
		void BindGBuffShaderSlot()   const;
		void UnbindGBuffShaderSlot() const;

		/*
		 * brief     : Shader�S�̂̎�ނ�ύX���邽�߂�ImGui
		 */
		void RenderImGuiComboShader();
		/*
		 * brief : mapping::shader_assets::ShaderAsset3D�Ŏw�肳�ꂽshader�X��
		 *		  ImGui(�p�����[�^)�\��
		 */
		void RenderImGuiShaderParameter();

		/*
		 * brief : shader_asset_map�ɓo�^����Ă���S�Ă�
		 *         ShaderAsset3D��material_path�̃Z�b�g
		 */
		void SetMaterialPathForAllShaderAsset3D(const shader_asset::MaterialPath& material_path);

		/*
		 * brief : asset�Ŏw�肳�ꂽShaderAsset3D��
		 *         material_path�̃Z�b�g
		 */
		void SetMaterialPath(mapping::shader_assets::ShaderAsset3D asset, const shader_asset::MaterialPath& material_path);

		[[nodiscard]] mapping::shader_assets::ShaderAsset3D GetCurrentAsset() const { return shader_assets.GetCurrentState(); }

	private:
		std::unordered_map<mapping::shader_assets::ShaderAsset3D, std::shared_ptr<shader_asset::ShaderAsset>> shader_asset_map{};
		EnumStateMap<mapping::shader_assets::ShaderAsset3D> shader_assets;

		template<typename T>
		void RegisterShaderAsset(mapping::shader_assets::ShaderAsset3D asset_type, u_int gbuffer_shader_slot)
		{
			if (shader_asset_map.contains(asset_type))
				return;

			shader_asset_map.emplace(asset_type, std::make_shared<T>(gbuffer_shader_slot));
		}
	};
}
