#pragma once

#include <string>
#include <string_view>

#include <d3d11.h>

#include <cereal/cereal.hpp>

#include "texture.h"
#include "model_data.h"

class FbxModelResource final
{
public:
	explicit FbxModelResource() = default; // For cereal
	explicit FbxModelResource(ID3D11Device* device, const char* import_filename, const char* dirname);
	~FbxModelResource();

	[[nodiscard]] const ModelData& GetModelData() const { return model_data; }
	[[nodiscard]] const std::string& GetModelName() const { return model_name; }

	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(model_name),
			CEREAL_NVP(model_data)
		);
	}

private:
	std::string model_name{};
	ModelData model_data{};

	// Deserialize
	void Import(const char* filename);
};