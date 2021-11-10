#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

namespace cumulonimbus::graphics
{
	namespace primitive
	{
		/**
		 * @brief : Vertex Shader�ɃZ�b�g����v���~�e�B�u�g�|���W�[
		 */
		struct Vertex
		{

		};
	} // primitive

	/**
	 * @brief : �f�o�b�O�p�v���~�e�B�u�̊��N���X
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