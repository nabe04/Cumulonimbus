#pragma once
#include <string>

#include <cereal/cereal.hpp>

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

	class ShaderAsset
	{
	public:
		explicit ShaderAsset() = default; // for cereal;
		virtual ~ShaderAsset() = default;

		/*
		 * brief : �R���X�^���g�o�b�t�@��GPU�ɑ��M�������ꍇ�Ɏg�p
		 *	       (�V�F�[�_�[�����p�����[�^�̃Z�b�g)
		 */
		virtual void BindCBuffer() {}
		/*
		 * brief : �g�p�����R���X�^���g�o�b�t�@�̃��Z�b�g
		 */
		virtual void UnbindCBuffer() {}
		/*
		 * brief : �e�X�̃V�F�[�_�[�����p�����[�^(constant buffer)�̕ҏW
		 */
		virtual void RenderImGui() {}

		/*
		 * brief : �g�p����e�N�X�`���̕ҏW
		 */
		virtual void RenderImGuiTextureCombo() {}

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
	private:
		MaterialPath material_path;
	};
} // cumulonimbus::shader_asset

