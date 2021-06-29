#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <imgui.h>

namespace cumulonimbus::helper::imgui
{
	/*
	 * brief : ImGui::Image�̊ȈՔ�
	 *        (shader resource view �� �摜�̑傫�����w�肷��̂�)
	 */
	inline void Image(ID3D11ShaderResourceView*    srv,
				      DirectX::SimpleMath::Vector2 image_size = {100,100})
	{
		ImGui::Image((void*)srv, ImVec2(image_size.x, image_size.y));
	}
} // cumulonimbus::helper::imgui