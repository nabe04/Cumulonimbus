#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fbxsdk.h>

#include "model_data.h"

class FbxModelResource
{
public:
	ModelData model_data{};

	//// Node = BoneÇ∆çlÇ¶ÇƒÇ¢Ç¢
	//struct Node
	//{
	//	std::string			name;
	//	int					parent_index;
	//	DirectX::XMFLOAT3	scale;
	//	DirectX::XMFLOAT4	rotate;
	//	DirectX::XMFLOAT3	translate;
	//};

	//struct Material
	//{
	//	DirectX::XMFLOAT4	color = { 0.8f, 0.8f, 0.8f, 1.0f };

	//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view = nullptr;
	//	std::string											texture_filename = nullptr;
	//};

	//struct Subset
	//{
	//	u_int		start_index = 0;
	//	u_int		index_count = 0;
	//	Material*   material   = nullptr;
	//};

	//struct Mesh
	//{
	//	Microsoft::WRL::ComPtr<ID3D11Buffer>	vertex_buffer;
	//	Microsoft::WRL::ComPtr<ID3D11Buffer>	index_buffer;
	//	std::vector<Subset>						subsets;

	//	int										node_index;
	//	std::vector<int>						node_indices;
	//	std::vector<DirectX::XMFLOAT4X4>		inverse_transforms;
	//};

	//struct NodeKeyData
	//{
	//	DirectX::XMFLOAT3	scale;
	//	DirectX::XMFLOAT4	rotate;
	//	DirectX::XMFLOAT3	translate;
	//};

	//struct Keyframe
	//{
	//	float						seconds;
	//	std::vector<NodeKeyData>	node_keys;
	//};
	//struct Animation
	//{
	//	float						seconds_length;
	//	std::vector<Keyframe>		keyframes;
	//};

private:
	int						root_motion_node_index = -1;

private:
	// Building node data
	void BuildNodes(FbxNode* fbx_node, int parent_node_index);
	void BuildNode(FbxNode* fbx_node, int parent_node_index);

	// Building mesh data
	void BuildMeshes(ID3D11Device* device, FbxNode* fbx_node);
	void BuildMesh(ID3D11Device* device, FbxNode* fbx_node, FbxMesh* fbx_mesh);

	// Building material data
	void BuildMaterials(ID3D11Device* device, const char* dirname, FbxScene* fbxScene);
	void BuildMaterial(ID3D11Device* device, const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial);

	// Building animation data
	void BuildAnimations(FbxScene* fbxScene);

	// Searching the Index
	int FindNodeIndex(const char* name);
	int FindMaterialIndex(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial);

public:
	explicit FbxModelResource(ID3D11Device* device, const char* fbx_filename, const char* ignoreRootMotionNodeName = {});
	~FbxModelResource();

	void AddAnimation(const char* fbx_filename);
};