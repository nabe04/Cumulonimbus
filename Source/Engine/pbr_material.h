#pragma once
#include "shader_interop_renderer.h"
#include "shader_asset.h"

namespace cumulonimbus::shader_asset
{
	class PBRMaterial final : public ShaderAsset
	{
	public:
		explicit PBRMaterial();
		~PBRMaterial() override = default;

		void BindCBuffer()	 override;
		void UnbindCBuffer() override;

		void BindTexture()	 override;
		void UnbindTexture() override;

		void RenderImGui() override;

		/*
		 * brief : �}�e���A���̃p�����[�^�g�p���邩
		 * true  : MaterialCB(�R���X�^���g�o�b�t�@)�̃p�����[�^���V�F�[�_�[�Ŏg�p
		 * false : PBR�e�N�X�`���̒l���Q��
		 */
		void SetCustomMaterial(bool flg) const;

	private:
		bool use_custom_parameter = true;

		std::unique_ptr<buffer::ConstantBuffer<MaterialCB>> cb_pbr_material{ nullptr };

		/*
		 * brief : MaterialPath�ł̃f�t�H���g�̃t�@�C������ݒ�
		 *         �R���X�^���g�o�b�t�@�Ŏg�p����
		 */
		void SetDefaultMaterialFilename();
	};

} // cumulonimbus::shader_asset
