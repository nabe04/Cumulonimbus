#pragma once
#include <unordered_map>

#include "enum_state_map.h"
#include "shader_asset.h"
#include "shader_asset_mapping.h"

namespace cumulonimbus::shader_asset
{
	class Material3DManager final
	{
	public:
		explicit Material3DManager();
		~Material3DManager() = default;

		// �R���X�^���g�o�b�t�@�̃o�C���h�A�A���o�C���h
		void BindAsset(mapping::shader_assets::ShaderAsset3D asset);
		void UnbindAsset(mapping::shader_assets::ShaderAsset3D asset);

		/*
		 * brief     : Shader�S�̂̎�ނ�ύX���邽�߂�ImGui
		 */
		void RenderImGui();
		/*
		 * brief : mapping::shader_assets::ShaderAsset3D�Ŏw�肳�ꂽshader�X��
		 *		  ImGui(�p�����[�^)�\��
		 */
		void RenderImGui(mapping::shader_assets::ShaderAsset3D asset);

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
		std::unordered_map<mapping::shader_assets::ShaderAsset3D, shader_asset::ShaderAsset> shader_asset_map{};
		EnumStateMap<mapping::shader_assets::ShaderAsset3D> shader_assets;

		template<typename T>
		void RegisterShaderAsset(mapping::shader_assets::ShaderAsset3D asset_type)
		{
			if (shader_asset_map.contains(asset_type))
				return;

			shader_asset_map.emplace(asset_type, T());
		}
	};
}
