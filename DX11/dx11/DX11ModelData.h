#pragma once
/*
* DirectX11のモデル関連のリソースを一元管理する関数群
* 未完成
*/

#include <vector>
#include <memory>
#include <string>
#include <d3d11.h>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

namespace model{
	class Model;
}

namespace dx11{
	ID3D11ShaderResourceView * AccessTexture(const std::string & path);
	ID3D11SamplerState * AccessSampler(const std::string & type = "");
	ID3D11VertexShader * AccessVertexShader(const std::string & name, model::Model * model);
	ID3D11PixelShader * AccessPixelShader(const std::string & name);

	// model::Model内のVertexの型は統一されているものとする
	ID3D11InputLayout * AccessLayout(ID3D11VertexShader * vs, model::Model * model);
	unsigned int AccessVertexStride(model::Model * model);
	ID3D11Buffer * AccessVertexBuffer(int index, model::Model * model);
	ID3D11Buffer * AccessIndexBuffer(int index, model::Model * model);
	DXGI_FORMAT AccessIndexBufferType(model::Model * model);
	D3D11_PRIMITIVE_TOPOLOGY AccessPrimitiveTopology(model::Model * model);
}