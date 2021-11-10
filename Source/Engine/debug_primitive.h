#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

namespace cumulonimbus::graphics
{
	namespace primitive
	{
		/**
		 * @brief : Vertex Shaderにセットするプリミティブトポロジー
		 */
		struct Vertex
		{

		};
	} // primitive

	/**
	 * @brief : デバッグ用プリミティブの基底クラス
	 */
	class DebugPrimitiveBase
	{
	protected:
		D3D11_PRIMITIVE_TOPOLOGY primitive_topology{};

	public:
		virtual void BindCBuffer()   = 0;
		virtual void UnBindCBuffer() = 0;
	};
} // cumulonimbus::graphics