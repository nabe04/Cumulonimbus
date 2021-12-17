#ifndef MATERIAL_H
#define MATERIAL_H

#ifdef __cplusplus
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "rename_type_mapping.h"
#include "graphics_mapping.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(MaterialCB, CBSlot_Material)
{
	float4 mat_use_roughness_channel;
	float4 mat_use_metalness_channel;
	float4 mat_use_occlusion_channel;
	float mat_roughness;
	float mat_metalness;
	
#ifdef __cplusplus
	template<class Archive>
	void load(Archive&& archive,const uint32_t version)
	{
		archive(
			CEREAL_NVP(mat_roughness),
			CEREAL_NVP(mat_metalness),
			CEREAL_NVP(mat_use_roughness_channel),
			CEREAL_NVP(mat_use_metalness_channel),
			CEREAL_NVP(mat_use_occlusion_channel)
		);
	}

	template<class Archive>
	void save(Archive&& archive,const uint32_t version)
	{
		archive(
			CEREAL_NVP(mat_roughness),
			CEREAL_NVP(mat_metalness),
			CEREAL_NVP(mat_use_roughness_channel),
			CEREAL_NVP(mat_use_metalness_channel),
			CEREAL_NVP(mat_use_occlusion_channel)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus

CEREAL_CLASS_VERSION(MaterialCB, 0)

namespace cumulonimbus::asset
{
	struct MaterialData
	{
		DirectX::SimpleMath::Vector4 color{ 1.f,1.f,1.f,1.f };	// ���f���{���̐F
		// �e�N�X�`��ID
		mapping::rename_type::UUID albedo_id{ "" };				// Albedo�e�N�X�`���̃e�N�X�`��ID
		mapping::rename_type::UUID metallic_id{ "" };			// Metallic Map�e�N�X�`���̃e�N�X�`��ID
		mapping::rename_type::UUID roughness_id{ "" };			// Roughness Map�e�N�X�`���̃e�N�X�`��ID
		mapping::rename_type::UUID normal_id{ "" };				// Normal Map�e�N�X�`����ID
		mapping::rename_type::UUID height_id{ "" };				// Height Map�e�N�X�`���̃e�N�X�`��ID
		mapping::rename_type::UUID occlusion_id{ "" };			// AO(Ambient Occlusion) Map�e�N�X�`����ID

		template<class Archive>
		void serialize(Archive&& archive);

		//template<class Archive>
		//void load(Archive&& archive, uint32_t version);

		//template<class Archive>
		//void save(Archive&& archive, uint32_t version) const;
	};

	/**
	 * @remark : ����
	 * @remark : https://muddy-crafter-729.notion.site/Material-System-d0f117bbfe3c40be8569f26b90647498
	 */
	class Material final
	{
	public:
		explicit Material() = default; // for cereal
		explicit Material(const mapping::rename_type::UUID& mat_id);
		explicit Material(const mapping::rename_type::UUID& mat_id, const MaterialData& data);
		~Material() = default;

		template<class Archive>
		void serialize(Archive&& archive);

		/**
		 * @brief : �u.mat�v�`���ł̕ۑ�(�V���A���C�Y)
		 * @remark : �g���q���u.mat�v�t�@�C���o�Ȃ��ꍇ��O�������͂���
		 * @param path : �t�H���_�����܂ރt�@�C���p�X(���g���q���܂�)
		 */
		void Save(const std::filesystem::path& path);
		/**
		 * @brief : �u.mat�v�t�@�C���̃��[�h(�f�V���A���C�Y)
		 */
		void Load(const std::filesystem::path& path);

		void RenderImGui();

		/**
		 * �}�e���A���̎��e�N�X�`���A�p�����[�^���V�F�[�_�[�Ƀo�C���h
		 */
		void BindMaterial(mapping::graphics::ShaderStage shader_stage) const;
		/**
		 * �}�e���A���̎��e�N�X�`���A�p�����[�^�̃A���o�C���h
		 */
		void UnbindMaterial(mapping::graphics::ShaderStage shader_stage) const;

		/**
		 * @return : ���f���{���̐F�̎擾
		 */
		[[nodiscard]]
		const DirectX::SimpleMath::Vector4& GetColor()	   const { return material_data.color; }
		/**
		 * @return : Albedo�e�N�X�`���̃e�N�X�`��ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetAlbedoID()	   const { return  material_data.albedo_id; }
		/**
		 * @return : Metallic Map�e�N�X�`���̃e�N�X�`��ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetMetallicID()  const { return  material_data.metallic_id; }
		/**
		 * @return : Normal Map�e�N�X�`����ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetNormalID()    const { return  material_data.normal_id; }
		/**
		 * @return : Height Map�e�N�X�`���̃e�N�X�`��ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetHeightID()    const { return material_data.height_id; }
		/**
		 * @return : AO(Ambient Occlusion) Map�e�N�X�`����ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetOcclusionID() const { return  material_data.occlusion_id; }

		/**
		 * @brief : ���f���F�̃Z�b�g
		 */
		void SetColor(const DirectX::SimpleMath::Vector4& color) { material_data.color = color; }
		/**
		 * @brief : Albedo�e�N�X�`���̃e�N�X�`��ID�̃Z�b�g
		 */
		void SetAlbedoID(const mapping::rename_type::UUID& id) { material_data.albedo_id = id; }
		/**
		 * @brief : Metallic Map�e�N�X�`���̃e�N�X�`��ID�̃Z�b�g
		 */
		void SetMetallicID(const mapping::rename_type::UUID& id) { material_data.metallic_id = id; }
		/**
		 * @brief : Normal Map�e�N�X�`����ID�̃Z�b�g
		 */
		void SetNormalID(const mapping::rename_type::UUID& id) { material_data.normal_id = id; }
		/**
		 * @brief :  Height Map�e�N�X�`���̃e�N�X�`��ID�̃Z�b�g
		 */
		void SetHeightID(const mapping::rename_type::UUID& id) { material_data.height_id = id; }
		/**
		 * @brief : AO(Ambient Occlusion) Map�e�N�X�`����ID�̃Z�b�g
		 */
		void SetOcclusionID(const mapping::rename_type::UUID& id) { material_data.occlusion_id = id; }
	private:
		// �}�e���A���f�[�^
		MaterialData material_data{};
		// �����̃}�e���A��ID
		mapping::rename_type::UUID mat_id{};
		// �R���X�^���g�o�b�t�@
		std::unique_ptr<buffer::ConstantBuffer<MaterialCB>> cb_material{};
	};
} // cumulonimbus::asset

#endif // __cplusplus
#endif // MATERIAL_H