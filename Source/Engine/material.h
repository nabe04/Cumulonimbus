#ifndef MATERIAL_H
#define MATERIAL_H

#ifdef __cplusplus
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "rename_type_mapping.h"
#include "graphics_mapping.h"
#include "enum_state_map.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(MaterialCB, CBSlot_Material)
{
	float4 mat_use_roughness_channel;
	float4 mat_use_metalness_channel;
	float4 mat_use_occlusion_channel;
	float  mat_roughness;
	float  mat_metalness;
	float2 mat_dummy;

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
	void save(Archive&& archive,const uint32_t version) const
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

namespace cumulonimbus::asset
{
	class AssetManager;
	class Texture;
	class MaterialLoader;

} // cumulonimbus::asset

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
	private:
		enum class ColorChannel
		{
			R,
			G,
			B,
			A,
			Custom,

			End
		};

	public:
		explicit Material() = default; // for cereal
		explicit Material(const mapping::rename_type::UUID& mat_id);
		explicit Material(const mapping::rename_type::UUID& mat_id, const MaterialData& data);
		explicit Material(const mapping::rename_type::UUID& mat_id, AssetManager& asset_manager, MaterialLoader& material_loader);
		//Material(Material& other);
		//Material& operator=(Material& other);
		~Material() = default;

		//template<class Archive>
		//void serialize(Archive&& archive);

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

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

		EnumStateMap<ColorChannel> roughness_channel{};
		EnumStateMap<ColorChannel> metalness_channel{};

		float roughness{};
		float metalness{};

		/**
		 * @brief : �}�e���A���̃e�N�X�`���I��
		 * @param tex : �ݒ肳��Ă���}�e���A���e�N�X�`��
		 * @param tex_id : �ݒ肳��Ă���e�N�X�`��ID
		 * @param tex_filename : �e�N�X�`����
		 * @param label_name : �e�N�X�`�������̃��x����
		 */
		void ImSelectTexture(
			Texture& tex, mapping::rename_type::UUID& mat_tex_id,
			const std::string& tex_filename, const std::string& label_name);
		/**
		 * @brief : �e�N�X�`���̎g�p����`�����l���̎w��
		 */
		void ImSelectColorChannel(
			EnumStateMap<ColorChannel>& color_channel,
			float4& use_channel, float& custom_param,
			const mapping::rename_type::UUID& mat_id,
			const std::string& label_name,
			float custom_param_range_min = 0.0f,
			float custom_param_range_max = 1.0f);
	};
} // cumulonimbus::asset

#endif // __cplusplus
#endif // MATERIAL_H