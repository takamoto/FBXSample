#pragma once
#include <string>
#include <memory>
#include <vector>
#include <DirectXMath.h>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D10Blob;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace model{
	class Model;
}

// DirectX11でモデルを描画するためのバッファとかモデル状態の管理クラス
namespace dx11{
	class Drawable {
	public:
		Drawable() = default;
		Drawable(std::string path, ID3D11Device * device) {load(path,device);}

		void load(std::string path, ID3D11Device * device);
		void setMotion(std::string name);
		void draw(ID3D11DeviceContext * context);
		void setPosition(double x, double y, double z);
		void setScale(double percent);
	private:
		std::shared_ptr<model::Model> _model_sp;
		std::shared_ptr<ID3D11Buffer> _meshBuffer;
		std::shared_ptr<ID3D11Buffer> _materialBuffer;
		DirectX::XMFLOAT3 _position;
		std::string _model;
		double _scale;
	};
}
