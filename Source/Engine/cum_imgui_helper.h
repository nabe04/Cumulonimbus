#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <imgui.h>

namespace cumulonimbus::helper::imgui
{
	/*
	 * brief : ImGui::Imageの簡易版
	 *        (shader resource view と 画像の大きさを指定するのみ)
	 */
	inline void Image(ID3D11ShaderResourceView*    srv,
				      DirectX::SimpleMath::Vector2 image_size = {100,100})
	{
		ImGui::Image((void*)srv, ImVec2(image_size.x, image_size.y));
	}
} // cumulonimbus::helper::imgui