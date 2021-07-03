#pragma once
#include <d3d11.h>
#include <wrl.h>

namespace cumulonimbus::shader_system
{
	//HRESULT CreateVertexShaderFromCSO(ID3D11Device* device,
	//								  const char* cso_name,
	//								  ID3D11VertexShader** vertex_shader,
	//								  ID3D11InputLayout** input_layout,
	//								  D3D11_INPUT_ELEMENT_DESC* input_element_desc,
	//								  UINT num_elements);
	
	class VertexShader final
	{
	public:
		explicit VertexShader(const char* cso, D3D11_INPUT_ELEMENT_DESC* input_element_desc = nullptr, UINT num_elements = 0);
		~VertexShader() = default;
		VertexShader(VertexShader&) = delete;
		VertexShader& operator=(VertexShader&) = delete;

		void BindVS(ID3D11InputLayout* input_layout);
		void BindVS();
		void UnbindVS();

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> default_shader_object = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  default_input_layout = nullptr;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> shader_object = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  input_layout = nullptr;
	};
}