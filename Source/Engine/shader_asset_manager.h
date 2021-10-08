#pragma once
#include <cassert>
#include <map>
#include <memory>

#include "enum_state_map.h"
#include "generic.h"
#include "rename_type_mapping.h"
#include "shader_asset.h"
#include "shader_asset_mapping.h"

/*
 * ShaderAsset(主に各シェーダーのコンスタントバッファを扱う)
 * クラスの管理クラス
 */

namespace cumulonimbus::shader_asset
{
	class ShaderAsset3DManager final
	{

	};

	class ShaderAsset2DManager final
	{
	public:
		explicit ShaderAsset2DManager();
		~ShaderAsset2DManager() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(shader_assets),
				CEREAL_NVP(current_asset_hash)
			);
		}

		/**
		 * @brief : 現在セットされているシェーダーの
		 *			コンスタントバッファをセット(バインド)
		 */
		void BindCBuffer() const;

		/**
		 * @brief : 現在セットされているシェーダーの
		 *		    テクスチャのセット(バインド)
		 */
		void BindTexture() const;

		template<class T>
		[[nodiscard]]
		T* GetShaderAsset()
		{
			const auto& hash = utility::GetHash<T>();
			if (!shader_assets.contains(hash))
				assert(!"Shader asset does not exist(ShaderAsset2DManager::GetShaderAsset)");

			return static_cast<T*>(shader_assets.at(hash).get());
		}
		[[nodiscard]]
		const mapping::rename_type::Hash& GetHash() const { return current_asset_hash; }

		template<class T>
		void SetCurrentShaderAsset()
		{
			const auto& hash = utility::GetHash<T>();
			//	shader_assetsに登録されていない場合は処理を中断する
			if (!shader_assets.contains(hash))
				assert(!"Shader asset does not exist(ShaderAsset2DManager::GetShaderAsset)");
			current_asset_hash = hash;
		}

	private:
		std::map<mapping::rename_type::Hash, std::shared_ptr<shader_asset::ShaderAsset>> shader_assets{};
		mapping::rename_type::Hash current_asset_hash{}; // 現在のシェーダ

		template<class T>
		void RegisterShaderAsset()
		{
			const auto& hash = utility::GetHash<T>();
			//	既に登録されている場合は処理を抜ける
			if (shader_assets.contains(hash))
				return;

			shader_assets.emplace(hash, std::make_shared<T>());
		}
	};
} // cumulonimbus::shader_asset