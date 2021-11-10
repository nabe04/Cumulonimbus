#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <memory>

#include "debug_primitive_shader.h"
#include "constant_buffer.h"

namespace cumulonimbus::graphics
{
	namespace primitive
	{
		/**
		 * @brief : Vertex Shaderにセットするプリミティブトポロジー
		 */
		struct Vertex
		{
			DirectX::SimpleMath::Vector4 position;
		};
	} // primitive

	/**
	 * @brief : デバッグ用プリミティブの基底クラス
	 */
	class DebugPrimitiveBase
	{
	public:
		explicit DebugPrimitiveBase(ID3D11Device* device);

		virtual void Render(ID3D11DeviceContext* immediate_context) = 0;
		
		virtual void BindCBuffer() = 0;
		virtual void UnBindCBuffer() = 0;
	protected:
		std::unique_ptr<buffer::ConstantBuffer<DebugPrimitiveCB>> cbuffer{};

		D3D11_PRIMITIVE_TOPOLOGY primitive_topology{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer{};

		virtual void CreateVertexBuffer(ID3D11Device* device) = 0;
	};

	class PrimitiveSphere final : public DebugPrimitiveBase
	{
	public:
		explicit PrimitiveSphere(ID3D11Device* device);

		void Render(ID3D11DeviceContext* immediate_context) override;
		
		void BindCBuffer() override;
		void UnBindCBuffer() override;
	private:
		void CreateVertexBuffer(ID3D11Device* device) override;
	};
} // cumulonimbus::graphics