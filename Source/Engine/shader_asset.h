#pragma once
#include <string>

#include <cereal/cereal.hpp>

#include "constant_buffer.h"
#include "shader_interop_renderer.h"
#include "texture.h"

namespace cumulonimbus::shader_asset
{
	/*
	 * brief : Shader�Ŏg�p����e�N�X�`���̎��ʖ�
	 *		   (�t�@�C���� or ����̃e�N�X�`�����������O)
	 */
	struct MaterialPath
	{
		std::string albedo_map_name{};
		std::string normal_map_name{};
		std::string pbr_map_name{}; // r : roughness, g : metalness, b : specular
		std::string roughness_map_name{};
		std::string metalness_map_name{};
		std::string specular_map_name{};
		std::string ao_map_name{};	// ambient occlusion

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(albedo_map_name),
				CEREAL_NVP(normal_map_name),
				CEREAL_NVP(pbr_map_name),
				CEREAL_NVP(roughness_map_name),
				CEREAL_NVP(metalness_map_name),
				CEREAL_NVP(specular_map_name),
				CEREAL_NVP(ao_map_name)
			);
		}
	};

	/*
	 * brief : �V�F�[�_�[�����p�����[�^�̒���
	 *         (�R���X�^���g�o�b�t�@�̒l)
	 */
	class ShaderAsset
	{
	public:
		/*
		 * gbuff_shader_slot : shader_asset_mapping.h�ł̃V�F�[�_�[���̃}�N�����w��
		 */
		explicit ShaderAsset() = default; // for cereal;
		virtual ~ShaderAsset() = default;

		/*
		 * brief : �R���X�^���g�o�b�t�@�ƃe�N�X�`���̃o�C���h
		 */
		void BindCBufferAndTexture()
		{
			BindCBuffer();
			BindTexture();
		}
		/*
		 * brief : �R���X�^���g�o�b�t�@�ƃe�N�X�`���̃A���o�C���h
		 */
		void UnbindCBufferAndTexture()
		{
			UnbindCBuffer();
			UnbindTexture();
		}

		/*
		* brief : �R���X�^���g�o�b�t�@��GPU�ɑ��M�������ꍇ�Ɏg�p
		*	       (�V�F�[�_�[�����p�����[�^�̃Z�b�g)
		 */
		virtual void BindCBuffer() {}
		/*
		 * brief : �g�p�����R���X�^���g�o�b�t�@�̃��Z�b�g(�A���o�C���h)
		 */
		virtual void UnbindCBuffer() {}

		/*
		 * brief : MaterialPath�\���̂����e�N�X�`���̃t�@�C������
		 *		   ����TextureManager�N���X��shader resource view���Z�b�g
		 */
		virtual void BindTexture() {}
		/*
		 * brief : �g�p�����e�N�X�`���̃X���b�g�̃��Z�b�g(�A���o�C���h)
		 */
		virtual void UnbindTexture() {}
		
		/*
		 * brief : �e�X�̃V�F�[�_�[�����p�����[�^(constant buffer)�̕ҏW
		 */
		virtual void RenderImGui() {}

		void SetMaterialPath(const MaterialPath& mat_path);
		void SetAlbedoMapName(const std::string& name);
		void SetNormalMapName(const std::string& name);
		void SetPBRMapName(const std::string& name);
		void SetRoughnessMapName(const std::string& name);
		void SetMetalnessMapName(const std::string& name);
		void SetSpecularMapName(const std::string& name);
		void SetAOMapName(const std::string& name);

		[[nodiscard]] const MaterialPath& GetMaterialPath()		const;
		[[nodiscard]] const std::string& GetAlbedoMapName()		const;
		[[nodiscard]] const std::string& GetNormalMapName()		const;
		[[nodiscard]] const std::string& GetPBRMapName()		const;
		[[nodiscard]] const std::string& GetRoughnessMapName()	const;
		[[nodiscard]] const std::string& GetMetalnessMapName()  const;
		[[nodiscard]] const std::string& GetSpecularMapName()   const;
		[[nodiscard]] const std::string& GetAOMapName()			const;


		template <typename Archive>
		void serialize(Archive&& archive){}
	protected:
		MaterialPath material_path;
		
		/*
		 * brief			 : ImGui��ł̃V�F�[�_�[���g�p����e�N�X�`���̕ύX����
		 * material_filename : MaterialPath�̃����o�ϐ������e�N�X�`���̃t�@�C������n��
		 * combo_label		 : ImGui::Combo�ł̃��x����(�C�ӂŖ��O��ύX�ł���悤�ɂ��邽�ߕϐ��Ȃǂ��g�p�������ڋL�q)
		 * ��caution(1)		 : ���̊֐����Ńt�@�C������ύX����̂ŁA�ʂ̂Ƃ����
		 *					   �ύX���Ȃ�����
		 * ��caution(2)		 : ImGui��ł̕ҏW�Ȃ̂�ImGui�ȊO�̗p�r�Ŏg�p���Ȃ��悤�ɒ���
		 * ��caution(3)      : �p����Ŏg�p����e�N�X�`���̕ҏW(RenderImGui�֐���)�Ŏg�p
		 */
		void ModifyMaterialPath(std::string& material_filename, const std::string& combo_label);
	};

} // cumulonimbus::shader_asset

