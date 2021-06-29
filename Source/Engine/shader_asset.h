#pragma once
#include <string>

#include <cereal/cereal.hpp>

namespace cumulonimbus::shader_asset
{
	/*
	 * brief : Shaderで使用するテクスチャの識別名
	 *		   (ファイル名 or 特定のテクスチャをさす名前)
	 */
	struct MaterialPath
	{
		std::string albedo_map_name{};
		std::string normal_map_name{};
		std::string roughness_map_name{};

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(albedo_map_name),
				CEREAL_NVP(normal_map_name),
				CEREAL_NVP(roughness_map_name)
			);
		}
	};

	class ShaderAsset
	{
	public:
		explicit ShaderAsset() = default; // for cereal;
		virtual ~ShaderAsset() = default;

		/*
		 * brief : コンスタントバッファをGPUに送信したい場合に使用
		 *	       (シェーダーが持つパラメータのセット)
		 */
		virtual void BindCBuffer()	 = 0;
		/*
		 * brief : 使用したコンスタントバッファのリセット
		 */
		virtual void UnbindCBuffer() = 0;
		virtual void RenderImGui()	 = 0;

		void SetMaterialPath(const MaterialPath& mat_path);
		void SetAlbedoMapName(const std::string& name);
		void SetNormalMapName(const std::string& name);
		void SetRoughnessMapName(const std::string& name);

		[[nodiscard]] const MaterialPath& GetMaterialPath() const;
		[[nodiscard]] const std::string& GetAlbedoMapName()    const;
		[[nodiscard]] const std::string& GetNormalMapName()    const;
		[[nodiscard]] const std::string& GetRoughnessMapName() const;


		template <typename Archive>
		void serialize(Archive&& archive){}
	private:
		MaterialPath material_path;
	};
} // cumulonimbus::shader_asset

