#include "debug_primitive.h"

#include "locator.h"
#include "shader_manager.h"

namespace cumulonimbus::graphics
{
	DebugPrimitiveBase::DebugPrimitiveBase(ID3D11Device* device)
	{
		cbuffer = std::make_unique<cumulonimbus::buffer::ConstantBuffer<DebugPrimitiveCB>>(device);
	}

	PrimitiveSphere::PrimitiveSphere(ID3D11Device* device)
		:DebugPrimitiveBase{device}
	{
		CreateVertexAndIndexBuffer(device);
	}

	void PrimitiveSphere::Render(ID3D11DeviceContext* immediate_context, const shader_system::ShaderManager& shader_manager)
	{
		shader_manager.BindShader(mapping::shader_assets::ShaderAsset3D::DebugPrimitive);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		cbuffer->Activate(immediate_context, CBSlot_DebugPrimitive, false, true);

		UINT stride = sizeof(primitive::Vertex);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		immediate_context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);

		cbuffer->Deactivate(immediate_context);
		shader_manager.UnbindShader(mapping::shader_assets::ShaderAsset3D::DebugPrimitive);
	}

	void PrimitiveSphere::BindCBuffer()
	{

	}

	void PrimitiveSphere::UnBindCBuffer()
	{

	}

	void PrimitiveSphere::CreateVertexAndIndexBuffer(ID3D11Device* device)
	{
		HRESULT hr = E_FAIL;
		const int segment_count = 36;

		{// Vertex Buffer�̍쐬
			primitive::Vertex vertex;
			// XY
			for (int i = 0; i <= segment_count; ++i)
			{
				const float angle0 = static_cast<float>(i) / static_cast<float>(segment_count) * DirectX::XM_2PI;
				vertex.position = DirectX::SimpleMath::Vector4(sinf(angle0), cosf(angle0), 0, 1);
				vertices.push_back(vertex);
			}
			// XZ
			for (int i = 0; i <= segment_count; ++i)
			{
				const float angle0 = static_cast<float>(i) / static_cast<float>(segment_count) * DirectX::XM_2PI;
				vertex.position = DirectX::SimpleMath::Vector4(sinf(angle0), 0, cosf(angle0), 1);
				vertices.push_back(vertex);
			}
			// YZ
			for (int i = 0; i <= segment_count; ++i)
			{
				const float angle0 = static_cast<float>(i) / static_cast<float>(segment_count) * DirectX::XM_2PI;
				vertex.position = DirectX::SimpleMath::Vector4(0, sinf(angle0), cosf(angle0), 1);
				vertices.push_back(vertex);
			}

			D3D11_BUFFER_DESC vertex_buffer_desc{};
			vertex_buffer_desc.Usage				= D3D11_USAGE_DEFAULT;
			vertex_buffer_desc.ByteWidth			= sizeof(primitive::Vertex) * static_cast<u_int>(vertices.size());
			vertex_buffer_desc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
			vertex_buffer_desc.CPUAccessFlags		= 0;
			vertex_buffer_desc.MiscFlags			= 0;
			vertex_buffer_desc.StructureByteStride	= 0;

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem			= vertices.data();
			init_data.SysMemPitch		= 0;
			init_data.SysMemSlicePitch	= 0;

			hr = device->CreateBuffer(&vertex_buffer_desc, &init_data, vertex_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Create Vertex buffer error(sky box)");
		}

		{// Index Buffer�̍쐬
			for (int i = 0; i < segment_count; ++i)
			{
				indices.push_back(static_cast<u_int>(i));
				indices.push_back(static_cast<u_int>(i + 1));
			}
			for (int i = 0; i < segment_count; ++i)
			{
				indices.push_back(static_cast<u_int>(i + segment_count + 1));
				indices.push_back(static_cast<u_int>(i + segment_count + 1 + 1));
			}
			for (int i = 0; i < segment_count; ++i)
			{
				indices.push_back(static_cast<u_int>(i + (segment_count + 1) * 2));
				indices.push_back(static_cast<u_int>(i + (segment_count + 1) * 2 + 1));
			}
			D3D11_BUFFER_DESC index_buffer_desc{};
			index_buffer_desc.Usage					= D3D11_USAGE_DEFAULT;
			index_buffer_desc.ByteWidth				= static_cast<UINT>(indices.size() )* sizeof(u_int);
			index_buffer_desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
			index_buffer_desc.CPUAccessFlags		= 0;
			index_buffer_desc.MiscFlags				= 0;
			index_buffer_desc.StructureByteStride	= 0;

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem			= indices.data();
			init_data.SysMemPitch		= 0;
			init_data.SysMemSlicePitch	= 0;
			hr = device->CreateBuffer(&index_buffer_desc, &init_data, index_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Create Index buffer error(sky box)");
		}

	}

	PrimitiveCapsule::PrimitiveCapsule(ID3D11Device* device)
		:DebugPrimitiveBase{ device }
	{
		CreateVertexAndIndexBuffer(device);
	}

	void PrimitiveCapsule::Render(ID3D11DeviceContext* immediate_context, const shader_system::ShaderManager& shader_manager)
	{
		using namespace DirectX::SimpleMath;
		const int segment_count = 18 + 1 + 18 + 1;
		struct LineSegment
		{
			Vector4 a, colorA, b, colorB;
		};

		//LineSegment line{};

		//Vector4 Eye = Vector4(0, 0, 0, 0);
		//Vector4 Unit = Vector4(0, 1, 0, 0);
		//const float radius = capsule.radius;

		//Vector4 Base = XMLoadFloat3(&capsule.base);
		//Vector4 Tip = XMLoadFloat3(&capsule.tip);
		//Vector4 Radius = DirectX::XMVectorReplicate(capsule.radius);
		//Vector4 Normal = DirectX::XMVector3Normalize(Tip - Base);
		//Vector4 Tangent = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Normal, Base - Eye));
		//Vector4 Binormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Tangent, Normal));
		//Vector4 LineEndOffset = Normal * Radius;
		//Vector4 A = Base + LineEndOffset;
		//Vector4 B = Tip - LineEndOffset;
		//Vector4 AB = Unit * Vector4(B - A).Length();
		//Matrix M = Matrix{ Tangent,Normal,Binormal, DirectX::XMVectorSetW(A, 1) };

		//for (int i = 0; i < segment_count * 2; i += 1)
		//{
		//	const float angle0 = DirectX::XM_PIDIV2 + (float)i / (float)segment_count * DirectX::XM_2PI;
		//	const float angle1 = DirectX::XM_PIDIV2 + (float)(i + 1) / (float)segment_count * DirectX::XM_2PI;
		//	Vector4 a, b;
		//	if (i < 18)
		//	{

		//		a = DirectX::XMVectorSet(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
		//		b = DirectX::XMVectorSet(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
		//	}
		//	else if (i == 18)
		//	{
		//		a = DirectX::XMVectorSet(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
		//		b = AB + Vector4(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
		//	}
		//	else if (i > 18 && i < 18 + 1 + 18)
		//	{
		//		a = AB + Vector4(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
		//		b = AB + Vector4(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
		//	}
		//	else
		//	{
		//		a = AB + Vector4(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
		//		b = DirectX::XMVectorSet(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
		//	}
		//	a = XMVector3Transform(a, M);
		//	b = XMVector3Transform(b, M);

		//	XMStoreFloat4(&line.a, a);
		//	XMStoreFloat4(&line.b, b);
		//	line.colorA = line.colorB = Vector4{ 1,1,1,1 };
		//	vertices.at(i).position = line.a;
		//	if(i != segment_count * 2 - 1)
		//		vertices.at(i + 1).position = line.b;
		//}

		shader_manager.BindShader(mapping::shader_assets::ShaderAsset3D::DebugPrimitive);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		cbuffer->Activate(immediate_context, CBSlot_DebugPrimitive, false, true);

		UINT stride = sizeof(primitive::Vertex);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		immediate_context->Draw(static_cast<UINT>(vertices.size()), 0);

		cbuffer->Deactivate(immediate_context);
		shader_manager.UnbindShader(mapping::shader_assets::ShaderAsset3D::DebugPrimitive);

		//immediate_context->UpdateSubresource(vertex_buffer.Get(),0,NULL,)
	}

	void PrimitiveCapsule::BindCBuffer()
	{



	}

	void PrimitiveCapsule::UnBindCBuffer()
	{
	}

	void PrimitiveCapsule::CreateVertexAndIndexBuffer(ID3D11Device* device)
	{
		using namespace DirectX::SimpleMath;
		const int segment_count = 18 + 1 + 18 + 1;

		struct LineSegment
		{
			Vector4 a, colorA, b, colorB;
		};

		LineSegment line{};

		Vector4 Eye  = Vector4(0, 0, 0, 0);
		Vector4 Unit = Vector4(0, 1, 0, 0);
		const float radius = capsule.radius;

		Vector4 Base = XMLoadFloat3(&capsule.base);
		Vector4 Tip = XMLoadFloat3(&capsule.tip);
		Vector4 Radius = DirectX::XMVectorReplicate(capsule.radius);
		Vector4 Normal = DirectX::XMVector3Normalize(Tip - Base);
		Vector4 Tangent = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Normal, Base - Eye));
		Vector4 Binormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Tangent, Normal));
		Vector4 LineEndOffset = Normal * Radius;
		Vector4 A = Base + LineEndOffset;
		Vector4 B = Tip - LineEndOffset;
		Vector4 AB = Unit * Vector4(B - A).Length();
		Matrix M = Matrix{ Tangent,Normal,Binormal, DirectX::XMVectorSetW(A, 1) };

		for (int i = 0; i < segment_count; i += 1)
		{
			const float angle0 = DirectX::XM_PIDIV2 + (float)i / (float)segment_count * DirectX::XM_2PI;
			const float angle1 = DirectX::XM_PIDIV2 + (float)(i + 1) / (float)segment_count * DirectX::XM_2PI;
			Vector4 a, b;
			if (i < 18)
			{

				a = DirectX::XMVectorSet(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
				b = DirectX::XMVectorSet(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
			}
			else if (i == 18)
			{
				a = DirectX::XMVectorSet(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
				b = AB + Vector4(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
			}
			else if (i > 18 && i < 18 + 1 + 18)
			{
				a = AB + Vector4(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
				b = AB + Vector4(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
			}
			else
			{
				a = AB + Vector4(sinf(angle0) * radius, cosf(angle0) * radius, 0, 1);
				b = DirectX::XMVectorSet(sinf(angle1) * radius, cosf(angle1) * radius, 0, 1);
			}
			a = XMVector3Transform(a, M);
			b = XMVector3Transform(b, M);

			XMStoreFloat4(&line.a, a);
			XMStoreFloat4(&line.b, b);
			line.colorA = line.colorB = Vector4{ 1,1,1,1 };
			vertices.emplace_back(primitive::Vertex(line.a));
			vertices.emplace_back(primitive::Vertex(line.b));
		}

		D3D11_BUFFER_DESC vertex_buffer_desc{};
		vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		vertex_buffer_desc.ByteWidth = sizeof(primitive::Vertex) * static_cast<u_int>(vertices.size());
		vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertex_buffer_desc.CPUAccessFlags = 0;
		vertex_buffer_desc.MiscFlags = 0;
		vertex_buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA init_data{};
		init_data.pSysMem = vertices.data();
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = 0;

		const HRESULT hr = device->CreateBuffer(&vertex_buffer_desc, &init_data, vertex_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"Create Vertex buffer error(sky box)");
	}
} // cumulonimbus::graphics
