#include "DX11Drawable.h"

#include "../model/Model.h"
#include "../fbx/FBXLoader.h"

#include "DX11ThinWrapper.h"
#include "DX11ModelData.h"


using namespace DirectX;

namespace dx11{
	struct ConstMeshParam {
		XMFLOAT4X4	mtxWorld;
	};

	struct ConstMaterialParam {
		XMFLOAT3 ambient;
		float a;
		XMFLOAT3 diffuse;
		float b;
		XMFLOAT3 emissive;
		float c;
		XMFLOAT3 specular;
		float d;
	};

	void Drawable::load(std::string path, ID3D11Device * device) {
		auto loader = fbx::FBXLoader(); // 本来はキャッシュする
		_model = path;
		_meshBuffer = d3::buffer::CreateConstantBuffer(
			device, nullptr, sizeof(ConstMeshParam), D3D11_CPU_ACCESS_WRITE
		);
		_materialBuffer = d3::buffer::CreateConstantBuffer(
			device, nullptr, sizeof(ConstMaterialParam), D3D11_CPU_ACCESS_WRITE
		);
		_model_sp = loader.loadModel(_model.c_str());
	}

	void Drawable::setMotion(std::string name) {
		
	}

	void Drawable::draw(ID3D11DeviceContext * context) {
		fbx::FBXLoader loader;
		auto model = _model_sp.get();
		for (int i = 0; i<model->getMeshCount(); ++i) {
			mapping(_meshBuffer.get(), context, [&](D3D11_MAPPED_SUBRESOURCE resource) {
				auto param = (ConstMeshParam*)(resource.pData);
				XMStoreFloat4x4(
					&param->mtxWorld, XMMatrixMultiplyTranspose(
					XMMatrixIdentity(), XMMatrixScaling(_scale,_scale,_scale))
				);
			});
			
			mapping(_materialBuffer.get(), context, [&](D3D11_MAPPED_SUBRESOURCE resource) {
				auto param = (ConstMaterialParam*)(resource.pData);
				auto & material = model->getMaterial(model->getMaterialID(model->getMesh(i).materialName));
				param->ambient = material.ambient;
				param->diffuse = material.diffuse;
				param->emissive = material.emissive;
				param->specular = material.specular;
			});

			ID3D11Buffer * meshBuffers[] = { _meshBuffer.get() };
			context->VSSetConstantBuffers(1, 1, meshBuffers);
			ID3D11Buffer * materialBuffers[] = { _materialBuffer.get() };
			context->PSSetConstantBuffers(2, 1, materialBuffers);

			// 各種設定
			// DX11ModelDataに処理を集めているがまだ中途半端
			UINT stride = AccessVertexStride(model);
			UINT offset = 0;
			auto vertexBuffer = AccessVertexBuffer(i, model);
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(AccessIndexBuffer(i, model), AccessIndexBufferType(model), 0);
			context->IASetPrimitiveTopology(AccessPrimitiveTopology(model));


			auto vs = AccessVertexShader("sample.hlsl", model);
			context->IASetInputLayout(AccessLayout(vs, model));
			context->VSSetShader(vs, NULL, 0);
			context->PSSetShader(AccessPixelShader("sample.hlsl"), NULL, 0);

			auto diffuse = AccessTexture(
				model->getMaterial(model->getMaterialID(model->getMesh(i).materialName)).diffuseTextureName
			);
			context->PSSetShaderResources(0, 1, &diffuse);

			auto normal = AccessTexture(
				model->getMaterial(model->getMaterialID(model->getMesh(i).materialName)).normalTextureName
			);
			context->PSSetShaderResources(1, 1, &normal);

			auto specular = AccessTexture(
				model->getMaterial(model->getMaterialID(model->getMesh(i).materialName)).specularTextureName
			);
			context->PSSetShaderResources(2, 1, &specular);

			auto sampler = AccessSampler();
			context->PSSetSamplers(0, 1, &sampler);

			context->DrawIndexed(model->getMesh(i).indexList.size(), 0, 0);
		}
	}

	void Drawable::setPosition(double x, double y, double z) {
		_position = XMFLOAT3(x,y,z);
	}
	void Drawable::setScale(double percent) {
		_scale = percent;
	}

}