#include "material.h"

namespace cumulonimbus::asset
{
	template <class Archive>
	void Material::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(color),
			CEREAL_NVP(albedo_id),
			CEREAL_NVP(metallic_id),
			CEREAL_NVP(normal_id),
			CEREAL_NVP(height_id),
			CEREAL_NVP(occlusion_id)
		);
	}
} // cumulonimbus::asset
