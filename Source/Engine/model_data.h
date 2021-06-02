#pragma once

#include <string>
#include <vector>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "shader.h"

class ModelData
{
public:
	// Node = Boneと考えていい
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

	struct Material
	{
		DirectX::XMFLOAT4									color = { 0.8f, 0.8f, 0.8f, 1.0f };

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view = nullptr;	// シリアライズの対象外
		std::string											texture_filename = "";		// モデルを読み込んだ時にテクスチャを作成する

		template<class Archive>
		void serialize(Archive& archive, int version)
		{
			archive(
				CEREAL_NVP(color),
				CEREAL_NVP(texture_filename)
			);
		}
	};

	struct Subset
	{
		unsigned int		start_index = 0;
		unsigned int		index_count = 0;
		unsigned int		material_index = 0;
		Material*			material = nullptr;		// モデルを読み込んだ時にマテリアルを関連ずける

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
		XMFLOAT4 position{};
		XMFLOAT3 normal{};
		XMFLOAT2 tex{};		// UV coordinates
		XMFLOAT4 bone_weight = { 1.0f,0.0f,0.0f,0.0f };
		XMUINT4  bone_index = { 0,0,0,0 };

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
		float						seconds_length{};
		std::vector<Keyframe>		keyframes{};

		template<class Archive>
		void serialize(Archive& archive, int version)
		{
			archive(
				CEREAL_NVP(seconds_length),
				CEREAL_NVP(keyframes)
			);
		}
	};

	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(nodes),
			CEREAL_NVP(materials),
			CEREAL_NVP(meshes),
			CEREAL_NVP(animations),
			CEREAL_NVP(root_motion_node_index)
		);
	}

	std::vector<Node>		nodes;
	std::vector<Material>	materials;
	std::vector<Mesh>		meshes;
	std::vector<Animation>  animations;
	int						root_motion_node_index = -1;
};