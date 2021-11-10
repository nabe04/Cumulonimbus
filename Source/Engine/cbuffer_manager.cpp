#include "cbuffer_manager.h"

#include "generic.h"
// shaders
#include "debug_primitive_shader.h"
namespace cumulonimbus::buffer
{
	ConstantBufferManager::ConstantBufferManager(ID3D11Device* device)
	{
		RegisterCBuffer<DebugPrimitiveCB>(device);
	}

	template <class T>
	void ConstantBufferManager::RegisterCBuffer(ID3D11Device* device)
	{
		const mapping::rename_type::Hash hash_code = utility::GetHash<T>();
		if (cbuffer_assets.contains(hash_code))
			return;

		cbuffer_assets.emplace(hash_code, std::make_unique<ConstantBufferAsset<T>>(device));
	}

} // cumulonimbus::buffer