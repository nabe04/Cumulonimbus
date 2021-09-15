#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <filesystem>

#include "rename_type_mapping.h"
#include "graphics_mapping.h"

namespace cumulonimbus::asset
{
	struct MaterialData
	{
		DirectX::SimpleMath::Vector4 color{ 1.f,1.f,1.f,1.f };	// モデル本来の色
		mapping::rename_type::UUID albedo_id{ "" };				// AlbedoテクスチャのテクスチャID
		mapping::rename_type::UUID metallic_id{ "" };			// Metallic MapテクスチャのテクスチャID
		mapping::rename_type::UUID normal_id{ "" };				// Normal MapテクスチャのID
		mapping::rename_type::UUID height_id{ "" };				// Height MapテクスチャのテクスチャID
		mapping::rename_type::UUID occlusion_id{ "" };			// AO(Ambient Occlusion) MapテクスチャのID

		template<class Archive>
		void serialize(Archive&& archive);
	};

	/**
	 * @remark : メモ
	 * @remark : https://muddy-crafter-729.notion.site/Material-System-d0f117bbfe3c40be8569f26b90647498
	 */
	class Material final
	{
	public:
		explicit Material() = default; // for cereal
		explicit Material(const MaterialData& data);
		~Material() = default;

		template<class Archive>
		void serialize(Archive&& archive);

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
		MaterialData material_data{};
	};
} // cumulonimbus::asset