#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fbxsdk.h>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "model_data.h"

class BulidFBX
{
public:
	int			root_motion_node_index = -1;
	ModelData	model_data;

private:
	// Building node data
	void BuildNodes(FbxNode* fbx_node, int parent_node_index);
	void BuildNode(FbxNode* fbx_node, int parent_node_index);

	// Building mesh data
	void BuildMeshes(FbxNode* fbx_node);
	void BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh);

	// Building material data
	void BuildMaterials(const char* dirname, FbxScene* fbxScene);
	void BuildMaterial(const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial);

	// Building animation data
	void BuildAnimations(FbxScene* fbxScene);

	// Searching the Index
	int FindNodeIndex(const char* name);
	int FindMaterialIndex(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial);

public:
	explicit BulidFBX(const char* fbx_filename, const char* ignoreRootMotionNodeName = {});
	~BulidFBX();

	void AddAnimation(const char* filename);

	// Serialize
	void Export(const char* filename);

	void Load(const char* filename);
};