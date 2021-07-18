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
	 * @brief				 : �}�e���A���ɓK������e�N�X�`���̃Z�b�g
	 * @param material_index : ModelData::materials(std::vector)�ւ̗v�f�ԍ�
	 * @param srv            : shader resource view
	 */
	void SetMaterialTexture(u_int material_index, ID3D11ShaderResourceView* srv)
	{
		model_data.materials.at(material_index).shader_resource_view = srv;
	}
	/**
	 * @brief					: �}�e���A���ɓK������e�N�X�`���t�@�C�����̃Z�b�g
	 * @param material_index	: ModelData::materials(std::vector)�ւ̗v�f�ԍ�
	 * @param filename          : �t�@�C����
	 */
	void SetMaterialFilename(u_int material_index, const std::string& filename)
	{
		model_data.materials.at(material_index).texture_filename = filename;
	}

	/**
	 * @brief					: ModelData::animations(std::vector)�̗v�f�ԍ�����
	 *							  �A�j���[�V�����̃L�[�t���[�������w��
	 * @param animation_index	: ModelData::animations(std::vector)�̗v�f�ԍ�
	 * @param frames			: �V�����L�[�t���[����
	 * @attention				: �ݒ肵���L�[�t���[�����ŏI�t���[���ɂȂ�
	 */
	void SetAnimationKeyFrame(u_int animation_index, u_int frames);
	/**
	 * @brief					: ModelData::animations(std::vector)�̃A�j���[�V����������
	 *							  �A�j���[�V�����̃L�[�t���[�������w��
	 * @param animation_name	: ���f�������A�j���[�V������
	 *							  (ModelData::Animation::animation_name)
	 * @param frames			: �V�����L�[�t���[����
	 * @attention				: �ݒ肵���L�[�t���[�����ŏI�t���[���ɂȂ�
	 * @attention				: animation_name�̖��O����v���Ȃ��ꍇ�A�T�[�V�����ŏ�����������
	 */
	void SetAnimationKeyFrame(const std::string& animation_name, u_int frames);

	/**
	 * @brief					: ModelData::animations(std::vector)�̗v�f�ԍ�����
	 *							  �A�j���[�V�����̍Đ����x���w��(�{��)
	 * @param animation_index   : ModelData::animations(std::vector)�̗v�f�ԍ�
	 * @param playback_speed    : �Đ����x(�{��)
	 */
	void SetAnimationPlaybackSpeed(u_int animation_index, float playback_speed);
	/**
	 * @brief					: ModelData::animations(std::vector)�̃A�j���[�V����������
	 *							  �A�j���[�V�����̃L�[�t���[�������w��
	 * @param animation_name    : ���f�������A�j���[�V������
	 * @param playback_speed    : �Đ����x(�{��)
	 * @attention				: animation_name�̖��O����v���Ȃ��ꍇ�A�T�[�V�����ŏ�����������
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
	std::string model_name{}; // FBX ���f����
	ModelData   model_data{}; // ���_���Ȃǂ��������f�[�^

	// Deserialize
	void Import(const char* filename);
};