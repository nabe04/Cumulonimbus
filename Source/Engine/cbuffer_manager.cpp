#include "cbuffer_manager.h"

#include "generic.h"
namespace cumulonimbus::buffer
{
	ConstantBufferManager::ConstantBufferManager(ID3D11Device* device)
	{

	}

	template <class T>
	void ConstantBufferManager::RegisterCBuffer(ID3D11Device* device)
	{
		const mapping::rename_type::Hash hash_code = utility::GetHash<T>();
		if (cbuffer_assets.contains(hash_code))
			return;

		cbuffer_assets.emplace(hash_code, std::make_unique<T>(device));
	}

} // cumulonimbus::buffer