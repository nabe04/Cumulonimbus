#pragma once
#include <d3d11.h>
#include <wrl.h>

namespace cumulonimbus::shader_system
{
	//HRESULT CreatePixelShaderFromCSO(ID3D11Device* device, const char* cso_name, 
	//								 ID3D11PixelShader** pixel_shader);

	class PixelShader final
	{
	public:
		explicit PixelShader(const char* cso);
		~PixelShader() = default;
		PixelShader(PixelShader&) = delete;
		PixelShader& operator=(PixelShader&) = delete;

		void BindPS();
		void UnbindPS() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> default_shader_object = nullptr;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_object = nullptr;

	};
}