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
	 * @brief : FBXモデルの情報保持クラス
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
			Microsoft::WRL::ComPtr<ID3D11Buffer>	vertex_buffer;	// シリアライズの対象外
			Microsoft::WRL::ComPtr<ID3D11Buffer>	index_buffer;	// シリアライズの対象外

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
			float						seconds_length{};		// キーフレームの長さ(秒) (num_key_frame * sampling_time)
			float						sampling_rate{};		// アニメーションのキーフレイムレイト
			float						sampling_time{};		// 1フレーム当たりのキーフレームの長さ(1 / sampling_rate)
			float						playback_speed{1.0f};	// アニメーションの再生速度(倍率)
			int							num_key_frame{};		// キーフレーム数
			int							default_key_frame{};	// デフォルトのキーフレーム
			std::string					animation_name{};		// アニメーションの名

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
		 * @brief : アニメーションキーフレームのリセット
		 * @remark : モデル読み込み時のdefault_key_frameがセットされる
		 * @param animation_index	: ModelData::animations(std::vector)の要素番号
		 */
		void ResetAnimationKeyFrame(u_int animation_index);
		/**
		 * @brief : アニメーションキーフレームのリセット
		 * @remark : モデル読み込み時のdefault_key_frameがセットされる
		 * @param animation_name	: モデルが持つアニメーション名
		 *							  (ModelData::Animation::animation_name)
		 */
		void ResetAnimationKeyFrame(const std::string& animation_name);

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
		std::vector<mapping::rename_type::UUID>	materials_id{}; // このIDを元にMaterialLoaderのmaterialsを取得
		int	root_motion_node_index{ -1 };

	};

	/**
	 * @brief : FBXモデル
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
		 * @brief : 「.model」形式としての保存(シリアライズ)
		 * @param path : 保存したいファイルまでのパス(拡張子を含まない)
		 */
		void Save(const std::filesystem::path& path);

	private:
		ModelData model_data{};

		void Load(const std::filesystem::path& path);
	};
} // cumulonimbus::asset

CEREAL_CLASS_VERSION(cumulonimbus::asset::ModelData, 0)
