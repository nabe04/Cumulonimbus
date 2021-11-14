#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <memory>

#include "debug_primitive_shader.h"
#include "constant_buffer.h"
#include "primitive_data.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		class ShaderManager;
	} // shader_system
} // cumulonimbus

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
		virtual  ~DebugPrimitiveBase()								= default;
		DebugPrimitiveBase(DebugPrimitiveBase&)						= delete;
		DebugPrimitiveBase(DebugPrimitiveBase&&)					= delete;
		DebugPrimitiveBase(const DebugPrimitiveBase&)				= delete;
		DebugPrimitiveBase(const DebugPrimitiveBase&&)				= delete;
		DebugPrimitiveBase& operator=(DebugPrimitiveBase&)			= delete;
		DebugPrimitiveBase& operator=(DebugPrimitiveBase&&)			= delete;
		DebugPrimitiveBase& operator=(const DebugPrimitiveBase&)	= delete;
		DebugPrimitiveBase& operator=(const DebugPrimitiveBase&&)	= delete;

		virtual void Render(ID3D11DeviceContext* immediate_context, const shader_system::ShaderManager& shader_manager) = 0;

		virtual void BindCBuffer() = 0;
		virtual void UnBindCBuffer() = 0;
	protected:
		std::unique_ptr<cumulonimbus::buffer::ConstantBuffer<DebugPrimitiveCB>> cbuffer{};
		std::vector<primitive::Vertex> vertices;
		std::vector<u_int> indices;

		D3D11_PRIMITIVE_TOPOLOGY primitive_topology{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer{};

		virtual void CreateVertexAndIndexBuffer(ID3D11Device* device) = 0;
	};

	class PrimitiveSphere final : public DebugPrimitiveBase
	{
	public:
		explicit PrimitiveSphere(ID3D11Device* device);
		~PrimitiveSphere() override							= default;
		PrimitiveSphere(PrimitiveSphere&)					= delete;
		PrimitiveSphere(PrimitiveSphere&&)					= delete;
		PrimitiveSphere(const PrimitiveSphere&)				= delete;
		PrimitiveSphere(const PrimitiveSphere&&)			= delete;
		PrimitiveSphere& operator=(PrimitiveSphere&)		= delete;
		PrimitiveSphere& operator=(PrimitiveSphere&&)		= delete;
		PrimitiveSphere& operator=(const PrimitiveSphere&)	= delete;
		PrimitiveSphere& operator=(const PrimitiveSphere&&) = delete;

		void Render(ID3D11DeviceContext* immediate_context, const shader_system::ShaderManager& shader_manager) override;

		void BindCBuffer() override;
		void UnBindCBuffer() override;
	private:
		void CreateVertexAndIndexBuffer(ID3D11Device* device) override;
	};

	class PrimitiveCapsule final : public DebugPrimitiveBase
	{
	public:
		explicit PrimitiveCapsule(ID3D11Device* device);
		~PrimitiveCapsule() override							= default;
		PrimitiveCapsule(PrimitiveCapsule&)						= delete;
		PrimitiveCapsule(PrimitiveCapsule&&)					= delete;
		PrimitiveCapsule(const PrimitiveCapsule&)				= delete;
		PrimitiveCapsule(const PrimitiveCapsule&&)				= delete;
		PrimitiveCapsule& operator=(PrimitiveCapsule&)			= delete;
		PrimitiveCapsule& operator=(PrimitiveCapsule&&)			= delete;
		PrimitiveCapsule& operator=(const PrimitiveCapsule&)	= delete;
		PrimitiveCapsule& operator=(const PrimitiveCapsule&&)	= delete;

		void Render(ID3D11DeviceContext* immediate_context, const shader_system::ShaderManager& shader_manager) override;

		void BindCBuffer()   override;
		void UnBindCBuffer() override;
	private:
		cumulonimbus::primitive::Capsule capsule{ cumulonimbus::primitive::Sphere(DirectX::SimpleMath::Vector3(),20),30 };
		void CreateVertexAndIndexBuffer(ID3D11Device* device) override;
	};

} // cumulonimbus::graphics