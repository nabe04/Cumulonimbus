#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "rename_type_mapping.h"

namespace cumulonimbus::asset
{
	/**
	 * @remark : ����
	 * @remark : https://muddy-crafter-729.notion.site/Material-System-d0f117bbfe3c40be8569f26b90647498
	 */
	class Material final
	{
	public:
		template<class Archive>
		void serialize(Archive&& archive);

		/**
		 * @return : ���f���{���̐F�̎擾
		 */
		[[nodiscard]]
		const DirectX::SimpleMath::Vector4& GetColor()	   const { return color; }
		/**
		 * @return : Albedo�e�N�X�`���̃e�N�X�`��ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetAlbedoID()	   const { return albedo_id; }
		/**
		 * @return : Metallic Map�e�N�X�`���̃e�N�X�`��ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetMetallicID()  const { return metallic_id; }
		/**
		 * @return : Normal Map�e�N�X�`����ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetNormalID()    const { return normal_id; }
		/**
		 * @return : Height Map�e�N�X�`���̃e�N�X�`��ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetHeightID()    const { return height_id; }
		/**
		 * @return : AO(Ambient Occlusion) Map�e�N�X�`����ID�̎擾
		 */
		[[nodiscard]]
		const mapping::rename_type::UUID& GetOcclusionID() const { return occlusion_id; }
	private:
		DirectX::SimpleMath::Vector4 color{};		// ���f���{���̐F
		mapping::rename_type::UUID albedo_id{};		// Albedo�e�N�X�`���̃e�N�X�`��ID
		mapping::rename_type::UUID metallic_id{};	// Metallic Map�e�N�X�`���̃e�N�X�`��ID
		mapping::rename_type::UUID normal_id{};		// Normal Map�e�N�X�`����ID
		mapping::rename_type::UUID height_id{};		// Height Map�e�N�X�`���̃e�N�X�`��ID
		mapping::rename_type::UUID occlusion_id{};	// AO(Ambient Occlusion) Map�e�N�X�`����ID
	};
} // cumulonimbus::asset