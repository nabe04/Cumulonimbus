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
		DirectX::SimpleMath::Vector4 color{ 1.f,1.f,1.f,1.f };	// モデル本来の色
		// テクスチャID
		mapping::rename_type::UUID albedo_id{ "" };				// AlbedoテクスチャのテクスチャID
		mapping::rename_type::UUID metallic_id{ "" };			// Metallic MapテクスチャのテクスチャID
		mapping::rename_type::UUID roughness_id{ "" };			// Roughness MapテクスチャのテクスチャID
		mapping::rename_type::UUID normal_id{ "" };				// Normal MapテクスチャのID
		mapping::rename_type::UUID height_id{ "" };				// Height MapテクスチャのテクスチャID
		mapping::rename_type::UUID occlusion_id{ "" };			// AO(Ambient Occlusion) MapテクスチャのID

		template<class Archive>
		void serialize(Archive&& archive);

		//template<class Archive>
		//void load(Archive&& archive, uint32_t version);

		//template<class Archive>
		//void save(Archive&& archive, uint32_t version) const;
	};

	/**
	 * @remark : メモ
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
		 * @brief : 「.mat」形式での保存(シリアライズ)
		 * @remark : 拡張子が「.mat」ファイル出ない場合例外処理がはしる
		 * @param path : フォルダ名を含むファイルパス(※拡張子を含む)
		 */
		void Save(const std::filesystem::path& path);
		/**
		 * @brief : 「.mat」ファイルのロード(デシリアライズ)
		 */
		void Load(const std::filesystem::path& path);

		void RenderImGui();

		/**
		 * マテリアルの持つテクスチャ、パラメータをシェーダーにバインド
		 */
		void BindMaterial(mapping::graphics::ShaderStage shader_stage) const;
		/**
		 * マテリアルの持つテクスチャ、パラメータのアンバインド
		 */
		void UnbindMaterial(mapping::graphics::ShaderStage shader_stage) const;

		/**
		 * @return : モデル本来の色の取得
		 */
		[[nodiscard]]
		const DirectX::SimpleMath::Vector4& GetColor()	   const { return material_data.color; }
		/**
		 * @return : AlbedoテクスチャのテクスチャIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetAlbedoID()	   const { return  material_data.albedo_id; }
		/**
		 * @return : Metallic MapテクスチャのテクスチャIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetMetallicID()  const { return  material_data.metallic_id; }
		/**
		 * @return : Normal MapテクスチャのIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetNormalID()    const { return  material_data.normal_id; }
		/**
		 * @return : Height MapテクスチャのテクスチャIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetHeightID()    const { return material_data.height_id; }
		/**
		 * @return : AO(Ambient Occlusion) MapテクスチャのIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetOcclusionID() const { return  material_data.occlusion_id; }

		/**
		 * @brief : モデル色のセット
		 */
		void SetColor(const DirectX::SimpleMath::Vector4& color) { material_data.color = color; }
		/**
		 * @brief : AlbedoテクスチャのテクスチャIDのセット
		 */
		void SetAlbedoID(const mapping::rename_type::UUID& id) { material_data.albedo_id = id; }
		/**
		 * @brief : Metallic MapテクスチャのテクスチャIDのセット
		 */
		void SetMetallicID(const mapping::rename_type::UUID& id) { material_data.metallic_id = id; }
		/**
		 * @brief : Normal MapテクスチャのIDのセット
		 */
		void SetNormalID(const mapping::rename_type::UUID& id) { material_data.normal_id = id; }
		/**
		 * @brief :  Height MapテクスチャのテクスチャIDのセット
		 */
		void SetHeightID(const mapping::rename_type::UUID& id) { material_data.height_id = id; }
		/**
		 * @brief : AO(Ambient Occlusion) MapテクスチャのIDのセット
		 */
		void SetOcclusionID(const mapping::rename_type::UUID& id) { material_data.occlusion_id = id; }
	private:
		// マテリアルデータ
		MaterialData material_data{};
		// 自分のマテリアルID
		mapping::rename_type::UUID mat_id{};
		// コンスタントバッファ
		std::unique_ptr<buffer::ConstantBuffer<MaterialCB>> cb_material{};

		EnumStateMap<ColorChannel> roughness_channel{};
		EnumStateMap<ColorChannel> metalness_channel{};

		float roughness{};
		float metalness{};

		/**
		 * @brief : マテリアルのテクスチャ選択
		 * @param tex : 設定されているマテリアルテクスチャ
		 * @param tex_id : 設定されているテクスチャID
		 * @param tex_filename : テクスチャ名
		 * @param label_name : テクスチャ説明のラベル名
		 */
		void ImSelectTexture(
			Texture& tex, mapping::rename_type::UUID& mat_tex_id,
			const std::string& tex_filename, const std::string& label_name);
		/**
		 * @brief : テクスチャの使用するチャンネルの指定
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