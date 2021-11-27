#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "material.h"

namespace cumulonimbus::asset
{
	/**
	 * @brief : FBX���f���̏��ێ��N���X
	 */
	class ModelData final
	{
	public:
		struct Node
		{
			std::string			name;
			int					parent_index;
			DirectX::XMFLOAT3	scale;
			DirectX::XMFLOAT4	rotate;
			DirectX::XMFLOAT3	translate;

			template<class Archive>
			void serialize(Archive& archive, int version)
			{
				archive(
					CEREAL_NVP(name),
					CEREAL_NVP(parent_index),
					CEREAL_NVP(scale),
					CEREAL_NVP(rotate),
					CEREAL_NVP(translate)
				);
			}
		};

		struct Subset
		{
			unsigned int		start_index = 0;
			unsigned int		index_count = 0;
			unsigned int		material_index = 0;

			template<class Archive>
			void serialize(Archive& archive, int version)
			{
				archive(
					CEREAL_NVP(start_index),
					CEREAL_NVP(index_count),
					CEREAL_NVP(material_index)
				);
			}
		};

		struct Vertex
		{
			DirectX::XMFLOAT4 position{};
			DirectX::XMFLOAT3 normal{};
			DirectX::XMFLOAT2 tex{};		// UV coordinates
			DirectX::XMFLOAT4 bone_weight = { 1.0f,0.0f,0.0f,0.0f };
			DirectX::XMUINT4  bone_index = { 0,0,0,0 };

			template<class Archive>
			void serialize(Archive& archive, int version)
			{
				archive(
					CEREAL_NVP(position),
					CEREAL_NVP(normal),
					CEREAL_NVP(tex),
					CEREAL_NVP(bone_weight),
					CEREAL_NVP(bone_index)
				);
			}
		};

		struct Mesh
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer>	vertex_buffer;	// �V���A���C�Y�̑ΏۊO
			Microsoft::WRL::ComPtr<ID3D11Buffer>	index_buffer;	// �V���A���C�Y�̑ΏۊO

			int										material_count;
			std::string								mesh_name;

			std::vector<Vertex>						vertices;
			std::vector<UINT>						indices;
			std::vector<Subset>						subsets;

			int										node_index;
			std::vector<int>						node_indices;
			std::vector<DirectX::XMFLOAT4X4>		inverse_transforms;

			template<class Archive>
			void serialize(Archive& archive, int version)
			{
				archive(
					CEREAL_NVP(material_count),
					CEREAL_NVP(mesh_name),
					CEREAL_NVP(vertices),
					CEREAL_NVP(indices),
					CEREAL_NVP(subsets),
					CEREAL_NVP(node_index),
					CEREAL_NVP(node_indices),
					CEREAL_NVP(inverse_transforms)
				);
			}
		};

		struct NodeKeyData
		{
			DirectX::XMFLOAT3	scale;
			DirectX::XMFLOAT4	rotate;
			DirectX::XMFLOAT3	translate;

			template<class Archive>
			void serialize(Archive& archive, int version)
			{
				archive(
					CEREAL_NVP(scale),
					CEREAL_NVP(rotate),
					CEREAL_NVP(translate)
				);
			}
		};

		struct Keyframe
		{
			float						seconds;
			std::vector<NodeKeyData>	node_keys;

			template<class Archive>
			void serialize(Archive& archive, int version)
			{
				archive(
					CEREAL_NVP(seconds),
					CEREAL_NVP(node_keys)
				);
			}
		};

		struct Animation
		{
			std::vector<Keyframe>		keyframes{};
			float						seconds_length{};		// �L�[�t���[���̒���(�b) (num_key_frame * sampling_time)
			float						sampling_rate{};		// �A�j���[�V�����̃L�[�t���C�����C�g
			float						sampling_time{};		// 1�t���[��������̃L�[�t���[���̒���(1 / sampling_rate)
			float						playback_speed{1.0f};	// �A�j���[�V�����̍Đ����x(�{��)
			int							num_key_frame{};		// �L�[�t���[����
			int							default_key_frame{};	// �f�t�H���g�̃L�[�t���[��
			std::string					animation_name{};		// �A�j���[�V�����̖�

			template<class Archive>
			void serialize(Archive& archive, int version)
			{
				archive(
					CEREAL_NVP(seconds_length),
					CEREAL_NVP(keyframes),
					CEREAL_NVP(sampling_rate),
					CEREAL_NVP(sampling_time),
					CEREAL_NVP(playback_speed),
					CEREAL_NVP(num_key_frame),
					CEREAL_NVP(animation_name)
				);
			}
		};

		template<class Archive>
		void save(Archive&& archive,const uint32_t version) const
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materials_id),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations),
				CEREAL_NVP(root_motion_node_index)
			);
		}

		template<class Archive>
		void load(Archive&& archive,const uint32_t version)
		{
			archive(
				CEREAL_NVP(nodes),
				CEREAL_NVP(materials_id),
				CEREAL_NVP(meshes),
				CEREAL_NVP(animations),
				CEREAL_NVP(root_motion_node_index)
			);
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
		 * @brief : �A�j���[�V�����L�[�t���[���̃��Z�b�g
		 * @remark : ���f���ǂݍ��ݎ���default_key_frame���Z�b�g�����
		 * @param animation_index	: ModelData::animations(std::vector)�̗v�f�ԍ�
		 */
		void ResetAnimationKeyFrame(u_int animation_index);
		/**
		 * @brief : �A�j���[�V�����L�[�t���[���̃��Z�b�g
		 * @remark : ���f���ǂݍ��ݎ���default_key_frame���Z�b�g�����
		 * @param animation_name	: ���f�������A�j���[�V������
		 *							  (ModelData::Animation::animation_name)
		 */
		void ResetAnimationKeyFrame(const std::string& animation_name);

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

		[[nodiscard]]
		const std::vector<Node>& GetNodes()	 const { return nodes; }
		[[nodiscard]]
		std::vector<Node>& GetNodes()			   { return nodes; }
		[[nodiscard]]
		const std::vector<Mesh>& GetMeshes() const { return meshes; }
		[[nodiscard]]
		std::vector<Mesh>& GetMeshes()			   { return meshes; }
		[[nodiscard]]
		const std::vector<mapping::rename_type::UUID>& GetMaterialsID() const { return materials_id; }
		[[nodiscard]]
		std::vector<mapping::rename_type::UUID>& GetMaterialsID() { return materials_id; }
		[[nodiscard]]
		const std::vector<Animation>& GetAnimations() const { return animations; }
		[[nodiscard]]
		std::vector<Animation>& GetAnimations() { return animations; }
	private:
		std::vector<Node>		nodes{};
		std::vector<Mesh>		meshes{};
		std::vector<Animation>  animations{};
		std::vector<mapping::rename_type::UUID>	materials_id{}; // ����ID������MaterialLoader��materials���擾
		int	root_motion_node_index{ -1 };

	};

	/**
	 * @brief : FBX���f��
	 */
	class Model final
	{
	public:
		explicit Model() = default; // for cereal
		explicit Model(const std::filesystem::path& path);
		~Model() = default;

		template<class Archive>
		void serialize(Archive&& archive);

		[[nodiscard]]
		ModelData& GetModelData()
		{
			return model_data;
		}
		[[nodiscard]]
		const ModelData& GetModelData() const
		{
			return model_data;
		}

		/**
		 * @brief : �u.model�v�`���Ƃ��Ă̕ۑ�(�V���A���C�Y)
		 * @param path : �ۑ��������t�@�C���܂ł̃p�X(�g���q���܂܂Ȃ�)
		 */
		void Save(const std::filesystem::path& path);

	private:
		ModelData model_data{};

		void Load(const std::filesystem::path& path);
	};
} // cumulonimbus::asset

CEREAL_CLASS_VERSION(cumulonimbus::asset::ModelData, 0)
