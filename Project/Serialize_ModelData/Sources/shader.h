#pragma once

#include <string>
#include <memory>

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

namespace shader
{
	//-----------------------------
	//
	//	Vertex data(3D) structure
	//
	//-----------------------------
	struct Vertex
	{
		DirectX::XMFLOAT4 position{};
		DirectX::XMFLOAT3 normal{};
		DirectX::XMFLOAT2 tex{};		// UV coordinates
		DirectX::XMFLOAT4 bone_weight = { 1.0f,0.0f,0.0f,0.0f };
		DirectX::XMUINT4  bone_index = { 0,0,0,0 };
	};

	//-----------------------------
	//
	//	Vertex data(2D) structure
	//
	//-----------------------------
	struct Vertex2D
	{
		DirectX::XMFLOAT2 position{};
		DirectX::XMFLOAT2 texcoord{};
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f };
	};
}




