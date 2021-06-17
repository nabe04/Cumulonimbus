#include "sprite_renderer.h"

#include "window.h"
#include "scene.h"
#include "sprite.h"
#include "anim_sprite.h"
#include "sprite_object.h"
#include "transform.h"
#include "transform_component.h"

#include "sampler.h"
#include "blend.h"
#include "depth_stencil.h"
#include "rasterizer.h"
#include "gaussian_blur.h"

SpriteRenderer::SpriteRenderer(ID3D11Device* device)
{
	cbuffer_transformation	= std::make_unique<buffer::ConstantBuffer<shader::CB_CoordinateTransformation>>(device);
	cbuffer_material		= std::make_unique<buffer::ConstantBuffer<shader::CB_Material>>(device);
	cbuffer_light			= std::make_unique<buffer::ConstantBuffer<shader::CB_Light>>(device);

	//-- DirectX States --//
	blend						= std::make_unique<Blend>(device);
	rasterizer					= std::make_unique<Rasterizer>(device);
	depth_stencil				= std::make_unique<DepthStencil>(device);
	samplers.at(Linear_Border)	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

	sprite_shader_manager = std::make_unique<shader::SpriteShaderManager>(device);
}

void SpriteRenderer::Render(ID3D11DeviceContext* immediate_context, const Entity* entity)
{
	if (!entity->HasComponent<SpriteObject>())
		return;

	auto* sprite_obj = entity->GetComponent<SpriteObject>();

	blend->Activate(immediate_context, sprite_obj->GetBlendState());
	rasterizer->Activate(immediate_context, sprite_obj->GetRasterizerState());
	depth_stencil->Activate(immediate_context, DepthStencilState::DepthTest_False);
	samplers.at(static_cast<int>(sprite_obj->GetSamplerState()))->Activate(immediate_context, 0);

	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	sprite_shader_manager->Activate(immediate_context, sprite_obj->GetShaderState());

	RenderSprite(immediate_context, entity);
	RenderAnim(immediate_context, entity);

	sprite_shader_manager->Deactivate(immediate_context, sprite_obj->GetShaderState());
}

void SpriteRenderer::RenderSprite(ID3D11DeviceContext* immediate_context, const Entity* entity)
{
	auto* sprite = entity->GetComponent<SpriteComponent>();
	if (!sprite)
		return;

	if (entity->HasComponent<AnimSpriteComponent>())
		return;

	auto* transform = entity->GetComponent<TransformComponent>();

	if (!transform)
		assert(!"Not found component(TransformComponenet)");

	immediate_context->PSSetShaderResources(0, 1, sprite->GetTexture()->GetTextureData()->texture_view.GetAddressOf());

	// 2Dに必要ない値のリセット
	transform->SetPositionZ(0.0f);
	transform->SetScaleZ(1.f);
	transform->SetWorldRotation_X(.0f);
	transform->SetWorldRotation_Y(.0f);

	std::array<XMFLOAT2, 4> pos{};
	// Left top
	pos[0].x = 0.0f;
	pos[0].y = 0.0f;
	// Right top
	pos[1].x = static_cast<float>(sprite->VariableWidth());
	pos[1].y = 0.0f;
	// Left bottom
	pos[2].x = 0.0f;
	pos[2].y = static_cast<float>(sprite->VariableHeight());
	// Right bottom
	pos[3].x = static_cast<float>(sprite->VariableWidth());
	pos[3].y = static_cast<float>(sprite->VariableHeight());

	// Rotation
	{
		const float sin_val = sinf(DirectX::XMConvertToRadians(transform->GetWorldRotation().z));
		const float cos_val = cosf(DirectX::XMConvertToRadians(transform->GetWorldRotation().z));

		for (auto& p : pos)
		{// Rotate with the pivot as the origin

			// Pivot adjustment
			p.x -= sprite->GetSrcPivot().x;
			p.y -= sprite->GetSrcPivot().y;

			// Scale ajustment
			p.x *= (transform->GetScale().x);
			p.y *= (transform->GetScale().y);

			// Rotate
			const float oldX = p.x;
			p.x = p.x * cos_val - p.y * sin_val;
			p.y = oldX * sin_val + p.y * cos_val;

			// Move back in position
			p.x += transform->GetPosition().x;
			p.y += transform->GetPosition().y;
		}
	}

	{
		RECT rec{};
		HWND desc_wnd_handle = entity->GetScene()->GetFramework()->GetWindow()->GetHWND(); //この関数でデスクトップのハンドルを取得

		GetClientRect(desc_wnd_handle, &rec);

		// No off-screen sprites are drawn
		for (int i = 0; i < 4; ++i)
		{
			if ((pos[i].x >= 0 && pos[i].x <= rec.right) && (pos[i].y >= 0 && pos[i].y <= rec.bottom))
			{
				break;
			}
			if (i == 4)
			{
				return;
			}
		}

		// NDC transform
		{
			for (auto& p : pos)
			{
				p.x = p.x * 2.0f / rec.right - 1.0f;
				p.y = -p.y * 2.0f / rec.bottom + 1.0f;
			}
		}
	}

	// Update vertex buffer
	std::array<shader::VertexSprite, 4> vertex{};
	vertex.at(0).position = DirectX::XMFLOAT4{ pos[0].x,pos[0].y,.0f,1.f };
	vertex.at(1).position = DirectX::XMFLOAT4{ pos[1].x,pos[1].y,.0f,1.f };
	vertex.at(2).position = DirectX::XMFLOAT4{ pos[2].x,pos[2].y,.0f,1.f };
	vertex.at(3).position = DirectX::XMFLOAT4{ pos[3].x,pos[3].y,.0f,1.f };
	vertex.at(0).color = sprite->Color();
	vertex.at(1).color = sprite->Color();
	vertex.at(2).color = sprite->Color();
	vertex.at(3).color = sprite->Color();
	vertex.at(0).texcoord = sprite->GetVariableTexcoords().at(0);
	vertex.at(1).texcoord = sprite->GetVariableTexcoords().at(1);
	vertex.at(2).texcoord = sprite->GetVariableTexcoords().at(2);
	vertex.at(3).texcoord = sprite->GetVariableTexcoords().at(3);

	immediate_context->UpdateSubresource(sprite->GetVertexBuffer(), 0, NULL, vertex.data(), 0, 0);

	// Set of Vertex Buffers
	UINT stride = sizeof(shader::VertexSprite);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, sprite->GetVertexBufferAddress(), &stride, &offset);

	immediate_context->Draw(4, 0);
}

void SpriteRenderer::RenderAnim(ID3D11DeviceContext* immediate_context, const Entity* entity)
{
	auto* sprite = entity->GetComponent<AnimSpriteComponent>();
	if (!sprite)
		return;

	auto* transform = entity->GetComponent<TransformComponent>();

	if (!transform)
		assert(!"Not found component(TransformComponenet)");

	immediate_context->PSSetShaderResources(0, 1, sprite->GetTexture()->GetTextureData()->texture_view.GetAddressOf());

	// 2Dに必要ない値のリセット
	transform->SetPositionZ(0.0f);
	transform->SetScaleZ(1.f);
	transform->SetWorldRotation_X(.0f);
	transform->SetWorldRotation_Y(.0f);

	std::array<XMFLOAT2, 4> pos{};
	// Left top
	pos[0].x = 0.0f;
	pos[0].y = 0.0f;
	// Right top
	pos[1].x = static_cast<float>(sprite->VariableWidth() / sprite->GetNumClip().x);
	pos[1].y = 0.0f;
	// Left bottom
	pos[2].x = 0.0f;
	pos[2].y = static_cast<float>(sprite->VariableHeight() / sprite->GetNumClip().y);
	// Right bottom
	pos[3].x = static_cast<float>(sprite->VariableWidth() / sprite->GetNumClip().x);
	pos[3].y = static_cast<float>(sprite->VariableHeight() / sprite->GetNumClip().y);

	// Rotation
	{
		const float sin_val = sinf(DirectX::XMConvertToRadians(transform->GetWorldRotation().z));
		const float cos_val = cosf(DirectX::XMConvertToRadians(transform->GetWorldRotation().z));

		for (auto& p : pos)
		{// Rotate with the pivot as the origin

			// Pivot adjustment
			p.x -= sprite->GetAnimPivot().x;
			p.y -= sprite->GetAnimPivot().y;

			// Scale ajustment
			p.x *= (transform->GetScale().x);
			p.y *= (transform->GetScale().y);

			// Rotate
			const float oldX = p.x;
			p.x = p.x * cos_val - p.y * sin_val;
			p.y = oldX * sin_val + p.y * cos_val;

			// Move back in position
			p.x += transform->GetPosition().x;
			p.y += transform->GetPosition().y;
		}
	}

	{
		RECT rec{};
		HWND desc_wnd_handle = entity->GetScene()->GetFramework()->GetWindow()->GetHWND(); //この関数でデスクトップのハンドルを取得

		GetClientRect(desc_wnd_handle, &rec);

		// No off-screen sprites are drawn
		for (int i = 0; i < 4; ++i)
		{
			if ((pos[i].x >= 0 && pos[i].x <= rec.right) && (pos[i].y >= 0 && pos[i].y <= rec.bottom))
			{
				break;
			}
			if (i == 4)
			{
				return;
			}
		}

		// NDC transform
		{
			for (auto& p : pos)
			{
				p.x = p.x * 2.0f / rec.right - 1.0f;
				p.y = -p.y * 2.0f / rec.bottom + 1.0f;
			}
		}
	}

	// Update vertex buffer
	std::array<shader::VertexSprite, 4> vertex{};
	vertex.at(0).position = DirectX::XMFLOAT4{ pos[0].x,pos[0].y,.0f,1.f };
	vertex.at(1).position = DirectX::XMFLOAT4{ pos[1].x,pos[1].y,.0f,1.f };
	vertex.at(2).position = DirectX::XMFLOAT4{ pos[2].x,pos[2].y,.0f,1.f };
	vertex.at(3).position = DirectX::XMFLOAT4{ pos[3].x,pos[3].y,.0f,1.f };
	vertex.at(0).color = sprite->Color();
	vertex.at(1).color = sprite->Color();
	vertex.at(2).color = sprite->Color();
	vertex.at(3).color = sprite->Color();
	vertex.at(0).texcoord = sprite->GetAnimTexcoord().at(AnimSpriteComponent::TexSide::LeftTop);
	vertex.at(1).texcoord = sprite->GetAnimTexcoord().at(AnimSpriteComponent::TexSide::RightTop);
	vertex.at(2).texcoord = sprite->GetAnimTexcoord().at(AnimSpriteComponent::TexSide::LeftBottom);
	vertex.at(3).texcoord = sprite->GetAnimTexcoord().at(AnimSpriteComponent::TexSide::RightBottom);

	immediate_context->UpdateSubresource(sprite->GetVertexBuffer(), 0, NULL, vertex.data(), 0, 0);

	// Set of Vertex Buffers
	UINT stride = sizeof(shader::VertexSprite);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, sprite->GetVertexBufferAddress(), &stride, &offset);

	immediate_context->Draw(4, 0);
}