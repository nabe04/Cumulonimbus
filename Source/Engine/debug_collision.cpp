#include "debug_collision.h"

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		DebugCollisionShader::DebugCollisionShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::DebugCollision_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::DebugCollision_PS().c_str());
		}

	} // shader_system

	namespace shader_asset
	{
		DebugCollisionAsset::DebugCollisionAsset()
			:ShaderAsset{}
		{
			cb_debug_collision = std::make_unique<buffer::ConstantBuffer<DebugCollisionCB>>(locator::Locator::GetDx11Device()->device.Get());

			// ‰ŠúÝ’è
			cb_debug_collision->GetData().collider_color = DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f };
		}

		void DebugCollisionAsset::BindCBuffer()
		{
			cb_debug_collision->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_DebugCollision);
		}

		void DebugCollisionAsset::UnbindCBuffer()
		{
			cb_debug_collision->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}

	} // shader_asset

} // cumulonimbus
