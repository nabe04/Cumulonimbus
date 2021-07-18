#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <d3d11.h>
#include <cereal/cereal.hpp>

#include "model_data.h"
#include "material3d_manager.h"

class FbxModelResource final
{
public:
	explicit FbxModelResource() = default; // For cereal
	explicit FbxModelResource(ID3D11Device* device, const char* import_filename, const char* dirname);
	~FbxModelResource();

	[[nodiscard]] ModelData&		 GetModelData()		  { return model_data; }
	[[nodiscard]] const ModelData&   GetModelData() const { return model_data; }
	[[nodiscard]] const std::string& GetModelName() const { return model_name; }

	/**
	 * @brief				 : マテリアルに適応するテクスチャのセット
	 * @param material_index : ModelData::materials(std::vector)への要素番号
	 * @param srv            : shader resource view
	 */
	void SetMaterialTexture(u_int material_index, ID3D11ShaderResourceView* srv)
	{
		model_data.materials.at(material_index).shader_resource_view = srv;
	}
	/**
	 * @brief					: マテリアルに適応するテクスチャファイル名のセット
	 * @param material_index	: ModelData::materials(std::vector)への要素番号
	 * @param filename          : ファイル名
	 */
	void SetMaterialFilename(u_int material_index, const std::string& filename)
	{
		model_data.materials.at(material_index).texture_filename = filename;
	}

	/**
	 * @brief					: ModelData::animations(std::vector)の要素番号から
	 *							  アニメーションのキーフレーム数を指定
	 * @param animation_index	: ModelData::animations(std::vector)の要素番号
	 * @param frames			: 新しいキーフレーム数
	 * @attention				: 設定したキーフレームが最終フレームになる
	 */
	void SetAnimationKeyFrame(u_int animation_index, u_int frames);
	/**
	 * @brief					: ModelData::animations(std::vector)のアニメーション名から
	 *							  アニメーションのキーフレーム数を指定
	 * @param animation_name	: モデルが持つアニメーション名
	 *							  (ModelData::Animation::animation_name)
	 * @param frames			: 新しいキーフレーム数
	 * @attention				: 設定したキーフレームが最終フレームになる
	 * @attention				: animation_nameの名前が一致しない場合アサーションで処理が落ちる
	 */
	void SetAnimationKeyFrame(const std::string& animation_name, u_int frames);

	/**
	 * @brief					: ModelData::animations(std::vector)の要素番号から
	 *							  アニメーションの再生速度を指定(倍率)
	 * @param animation_index   : ModelData::animations(std::vector)の要素番号
	 * @param playback_speed    : 再生速度(倍率)
	 */
	void SetAnimationPlaybackSpeed(u_int animation_index, float playback_speed);
	/**
	 * @brief					: ModelData::animations(std::vector)のアニメーション名から
	 *							  アニメーションのキーフレーム数を指定
	 * @param animation_name    : モデルが持つアニメーション名
	 * @param playback_speed    : 再生速度(倍率)
	 * @attention				: animation_nameの名前が一致しない場合アサーションで処理が落ちる
	 */
	void SetAnimationPlaybackSpeed(const std::string& animation_name, float playback_speed);
	
	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(model_name),
			CEREAL_NVP(model_data)
		);
	}

private:
	std::string model_name{}; // FBX モデル名
	ModelData   model_data{}; // 頂点情報などが入ったデータ

	// Deserialize
	void Import(const char* filename);
};