#pragma once
#include <string>

#include <cereal/cereal.hpp>

#include "constant_buffer.h"
#include "shader_interop_renderer.h"
#include "texture.h"

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
	 * brief : シェーダーが持つパラメータの調整
	 *         (コンスタントバッファの値)
	 */
	class ShaderAsset
	{
	public:
		/*
		 * gbuff_shader_slot : shader_asset_mapping.hでのシェーダー毎のマクロを指定
		 */
		explicit ShaderAsset() = default; // for cereal;
		virtual ~ShaderAsset() = default;

		/*
		 * brief : コンスタントバッファとテクスチャのバインド
		 */
		void BindCBufferAndTexture()
		{
			BindCBuffer();
			BindTexture();
		}
		/*
		 * brief : コンスタントバッファとテクスチャのアンバインド
		 */
		void UnbindCBufferAndTexture()
		{
			UnbindCBuffer();
			UnbindTexture();
		}

		/*
		* brief : コンスタントバッファをGPUに送信したい場合に使用
		*	       (シェーダーが持つパラメータのセット)
		 */
		virtual void BindCBuffer() {}
		/*
		 * brief : 使用したコンスタントバッファのリセット(アンバインド)
		 */
		virtual void UnbindCBuffer() {}

		/*
		 * brief : MaterialPath構造体が持つテクスチャのファイル名を
		 *		   元にTextureManagerクラスのshader resource viewをセット
		 */
		virtual void BindTexture() {}
		/*
		 * brief : 使用したテクスチャのスロットのリセット(アンバインド)
		 */
		virtual void UnbindTexture() {}
		
		/*
		 * brief : 各々のシェーダーが持つパラメータ(constant buffer)の編集
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
		 * brief			 : ImGui上でのシェーダーが使用するテクスチャの変更処理
		 * material_filename : MaterialPathのメンバ変数が持つテクスチャのファイル名を渡す
		 * combo_label		 : ImGui::Comboでのラベル名(任意で名前を変更できるようにするため変数などを使用せず直接記述)
		 * ※caution(1)		 : この関数内でファイル名を変更するので、別のところで
		 *					   変更しないこと
		 * ※caution(2)		 : ImGui上での編集なのでImGui以外の用途で使用しないように注意
		 * ※caution(3)      : 継承先で使用するテクスチャの編集(RenderImGui関数内)で使用
		 */
		void ModifyMaterialPath(std::string& material_filename, const std::string& combo_label);
	};

} // cumulonimbus::shader_asset

