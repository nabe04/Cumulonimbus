#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "rename_type_mapping.h"

namespace cumulonimbus::asset
{
	/**
	 * @remark : メモ
	 * @remark : https://muddy-crafter-729.notion.site/Material-System-d0f117bbfe3c40be8569f26b90647498
	 */
	class Material final
	{
	public:
		template<class Archive>
		void serialize(Archive&& archive);

		/**
		 * @return : モデル本来の色の取得
		 */
		[[nodiscard]]
		const DirectX::SimpleMath::Vector4& GetColor()	   const { return color; }
		/**
		 * @return : AlbedoテクスチャのテクスチャIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetAlbedoID()	   const { return albedo_id; }
		/**
		 * @return : Metallic MapテクスチャのテクスチャIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetMetallicID()  const { return metallic_id; }
		/**
		 * @return : Normal MapテクスチャのIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetNormalID()    const { return normal_id; }
		/**
		 * @return : Height MapテクスチャのテクスチャIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetHeightID()    const { return height_id; }
		/**
		 * @return : AO(Ambient Occlusion) MapテクスチャのIDの取得
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetOcclusionID() const { return occlusion_id; }
	private:
		DirectX::SimpleMath::Vector4 color{};		// モデル本来の色
		mapping::rename_type::UUID albedo_id{};		// AlbedoテクスチャのテクスチャID
		mapping::rename_type::UUID metallic_id{};	// Metallic MapテクスチャのテクスチャID
		mapping::rename_type::UUID normal_id{};		// Normal MapテクスチャのID
		mapping::rename_type::UUID height_id{};		// Height MapテクスチャのテクスチャID
		mapping::rename_type::UUID occlusion_id{};	// AO(Ambient Occlusion) MapテクスチャのID
	};
} // cumulonimbus::asset