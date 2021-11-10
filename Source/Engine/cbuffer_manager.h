#pragma once
#include <map>
#include <memory>

#include "rename_type_mapping.h"
#include "constant_buffer.h"

namespace cumulonimbus::buffer
{
	class ConstantBufferAssetBase
	{
	public:
		explicit ConstantBufferAssetBase() = default;
		virtual ~ConstantBufferAssetBase() = default;

		virtual void BindCBuffer() = 0;
		virtual void UnBindCBuffer() = 0;
	};

	template <class T>
	class ConstantBufferAsset final : public ConstantBufferAssetBase
	{
	public:
		explicit ConstantBufferAsset()  = default;
		~ConstantBufferAsset() override = default;

		void BindCBuffer()   override;
		void UnBindCBuffer() override;

	private:
		ConstantBuffer<T> cbuffer{};
	};

	class ConstantBufferManager
	{
	public:
		explicit ConstantBufferManager(ID3D11Device* device);
		~ConstantBufferManager() = default;

	private:
		std::map<mapping::rename_type::Hash, std::unique_ptr<ConstantBufferAssetBase>> cbuffer_assets{};

		/**
		 * @brief : コンスタントバッファの登録
		 */
		template<class T>
		void RegisterCBuffer(ID3D11Device* device);
	};
} // cumulonimbus::graphics