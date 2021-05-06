#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fbxsdk.h>

#include "texture.h"
#include "model_data.h"

class FbxModelResource
{
public:
	explicit FbxModelResource(ID3D11Device* device, const char* import_filename, const char* dirname);
	~FbxModelResource();

	[[nodiscard]] const ModelData& GetModelData() const { return model_data; }
	[[nodiscard]] std::string GetModelName() { return model_name; }

private:
	// Deserialize
	void Import(const char* filename);

private:
	std::string model_name{};
	ModelData model_data{};

};