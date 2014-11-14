#include "DX11ModelData.h"

#include <tchar.h>
#include "../WICTextureLoader.h"

#include "DX11ThinWrapper.h"
#include "../model/Model.h"

#include "DirectXShaderLevelDefine.h"

#define INDEX_UINT16
#include "DXGI_FORMAT.h"

using namespace DirectX;


namespace dx11 {
	namespace {
		/*
		void loadVertexBuffers(
			model::Model * model, ID3D11Device * device, std::vector<std::shared_ptr<ID3D11Buffer>> * vertexBuffers
			) {
			vertexBuffers->reserve(model->getMeshCount());
			for (int i = 0; i < model->getMeshCount(); ++i) {
				vertexBuffers->push_back(d3::buffer::CreateVertexBuffer(
					device, (void*)model->getMesh(i).vertexList.data(),
					sizeof(model::element::Vertex) * model->getMesh(i).vertexList.size()
				));
			}
		}
		void loadIndexBuffers(
			model::Model * model, ID3D11Device * device, std::vector<std::shared_ptr<ID3D11Buffer>> * indexBuffers
			) {
			indexBuffers->reserve(model->getMeshCount());
			for (int i = 0; i < model->getMeshCount(); ++i) {
				indexBuffers->emplace_back(d3::buffer::CreateIndexBuffer(
					device, (void*)model->getMesh(i).indexList.data(),
					sizeof(INDEX_FORMAT_TYPE)* model->getMesh(i).indexList.size()
					));
			}
		}

		
		void loadVertexShaders(
			model::Model * model, ID3D11Device * device,
			std::vector<std::shared_ptr<ID3D11VertexShader>> * vertexShaders,
			std::vector<std::shared_ptr<ID3D11InputLayout>> * layouts
			) {
			vertexShaders->reserve(model->getMeshCount());
			layouts->reserve(model->getMeshCount());
			for (int i = 0; i < model->getMeshCount(); ++i) {
				auto vs = d3::shader::CompileShader(_T("Sample.hlsl"), "RenderVS", VERTEX_SHADER_PROFILE);
				vertexShaders->emplace_back(d3::shader::CreateVertexShader(device, vs.get()));


				D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
						{ "IN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "IN_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "IN_UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "IN_BONEINDEX", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "IN_BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};
				layouts->emplace_back(d3::shader::CreateInputLayout(device, layoutDesc, _countof(layoutDesc), vs.get()));
			}
		}
		
		void loadPixelShaders(
			model::Model * model, ID3D11Device * device,
			const std::vector<std::shared_ptr<ID3D11ShaderResourceView>> & textures,
			const std::vector<std::shared_ptr<ID3D11ShaderResourceView>> & normals,
			const std::vector<std::shared_ptr<ID3D11ShaderResourceView>> & speculars,
			std::vector<std::shared_ptr<ID3D11PixelShader>> * pixelShaders
		) {
			auto ps = d3::shader::CompileShader(_T("Sample.hlsl"), "RenderPS", PIXEL_SHADER_PROFILE);
			auto pixelShader = d3::shader::CreatePixelShader(device, ps.get());

			auto ps_tex = d3::shader::CompileShader(_T("Sample.hlsl"), "RenderPS_Tex", PIXEL_SHADER_PROFILE);
			auto pixelShader_tex = d3::shader::CreatePixelShader(device, ps_tex.get());

			auto ps_spec = d3::shader::CompileShader(_T("Sample.hlsl"), "RenderPS_Tex_spec", PIXEL_SHADER_PROFILE);
			auto pixelShader_tex_spec = d3::shader::CreatePixelShader(device, ps_spec.get());

			pixelShaders->reserve(model->getMeshCount());
			for (int i = 0; i < model->getMeshCount(); ++i) {
				if (textures[model->getMaterialID(model->getMesh(i).materialName)] && normals[model->getMaterialID(model->getMesh(i).materialName)]) {
					if (speculars[model->getMaterialID(model->getMesh(i).materialName)]) {
						pixelShaders->push_back(pixelShader_tex_spec);
					} else {
						pixelShaders->push_back(pixelShader_tex);
					}
				} else {
					pixelShaders->push_back(pixelShader);
				}
			}
		}

		void loadTexture(
			model::Model * model, ID3D11Device * device,
			std::vector<std::shared_ptr<ID3D11ShaderResourceView>> * textures
			) {
			textures->reserve(model->getMaterialCount());
			for (int i = 0; i < model->getMaterialCount(); ++i) {
				std::shared_ptr<ID3D11ShaderResourceView> tex;
				if (model->getMaterial(i).diffuseTextureName != "") {
					char filename[100];
					char ext[10];
					_splitpath_s(model->getMaterial(i).diffuseTextureName.c_str(), NULL, 0, NULL, 0, filename, 100, ext, 10);


					std::string p = filename;
					p += ext;
					std::basic_string<wchar_t> path(p.begin(), p.end());
					ID3D11ShaderResourceView * textureView;
					CreateWICTextureFromFile(device, path.c_str(), nullptr, &textureView);
					tex.reset(textureView, [](IUnknown * p) {
						p->Release();
					});
				}

				textures->push_back(tex);
			}
		}

		void loadBumpMap(
			model::Model * model, ID3D11Device * device,
			std::vector<std::shared_ptr<ID3D11ShaderResourceView>> * normals
			) {
			normals->reserve(model->getMaterialCount());

			for (int i = 0; i < model->getMaterialCount(); ++i) {
				std::shared_ptr<ID3D11ShaderResourceView> tex;
				if (model->getMaterial(i).normalTextureName != "") {
					char filename[100];
					char ext[10];
					_splitpath_s(model->getMaterial(i).normalTextureName.c_str(), NULL, 0, NULL, 0, filename, 100, ext, 10);


					std::string p = filename;
					p += ext;
					std::basic_string<wchar_t> path(p.begin(), p.end());
					ID3D11ShaderResourceView * textureView;
					CreateWICTextureFromFile(device, path.c_str(), nullptr, &textureView);
					tex.reset(textureView, [](IUnknown * p) {
						p->Release();
					});
				}

				normals->push_back(tex);
			}
		}

		void loadSpecularMap(
			model::Model * model, ID3D11Device * device,
			std::vector<std::shared_ptr<ID3D11ShaderResourceView>> * speculars
			) {
			speculars->reserve(model->getMaterialCount());

			for (int i = 0; i < model->getMaterialCount(); ++i) {
				std::shared_ptr<ID3D11ShaderResourceView> tex;
				if (model->getMaterial(i).specularTextureName != "") {
					char filename[100];
					char ext[10];
					_splitpath_s(model->getMaterial(i).specularTextureName.c_str(), NULL, 0, NULL, 0, filename, 100, ext, 10);

					std::string p = filename;
					p += ext;
					std::basic_string<wchar_t> path(p.begin(), p.end());
					ID3D11ShaderResourceView * textureView;
					CreateWICTextureFromFile(device, path.c_str(), nullptr, &textureView);
					tex.reset(textureView, [](IUnknown * p) {
						p->Release();
					});
				}

				speculars->push_back(tex);
			}
		}
		
		std::shared_ptr<ID3D11SamplerState> loadSampler(
			model::Model * model, ID3D11Device * device
			) {
			D3D11_SAMPLER_DESC samDesc;
			ZeroMemory(&samDesc, sizeof(samDesc));
			samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samDesc.MaxAnisotropy = 1;
			samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samDesc.MaxLOD = D3D11_FLOAT32_MAX;

			ID3D11SamplerState * sampler;
			device->CreateSamplerState(&samDesc, &sampler);
			return std::shared_ptr<ID3D11SamplerState>(sampler, [](IUnknown * p) {p->Release(); });
		}*/
	}
}


#include <map>
#include "DX11GlobalDevice.h"

namespace dx11 {
	static std::map<std::pair<ID3D11VertexShader *, model::Model *>, std::shared_ptr<ID3D11InputLayout>> g_layoutStorage;
	static std::map<std::string, std::shared_ptr<ID3D11VertexShader>> g_vertexShaderStorage;
	static std::map<std::string, std::shared_ptr<ID3D11PixelShader>> g_pixelShaderStorage;
	static std::map<std::string, std::shared_ptr<ID3D11SamplerState>> g_samplerStorage;
	static std::map<std::string, std::shared_ptr<ID3D11ShaderResourceView>> g_textureStorage;
	static std::map<std::pair<int, model::Model *>, std::shared_ptr<ID3D11Buffer>> g_vertexBufferStorage;
	static std::map<std::pair<int, model::Model *>, std::shared_ptr<ID3D11Buffer>> g_indexBufferStorage;

	ID3D11ShaderResourceView * AccessTexture(const std::string & path) {
		if (g_textureStorage.count(path) == 0) {
			std::shared_ptr<ID3D11ShaderResourceView> tex;
			if (path != "") {
				char filename[100];
				char ext[10];
				_splitpath_s(path.c_str(), NULL, 0, NULL, 0, filename, 100, ext, 10);


				std::string p = filename;
				p += ext;
				std::basic_string<wchar_t> path(p.begin(), p.end());
				ID3D11ShaderResourceView * textureView;
				CreateWICTextureFromFile(AccessDX11Device(), path.c_str(), nullptr, &textureView);
				tex.reset(textureView, [](IUnknown * p) {
					p->Release();
				});
			}
			g_textureStorage[path] = tex;
		}

		return g_textureStorage[path].get();
	}

	ID3D11SamplerState * AccessSampler(const std::string & type) {
		if (g_samplerStorage.count(type) == 0) {
			D3D11_SAMPLER_DESC samDesc;
			ZeroMemory(&samDesc, sizeof(samDesc));
			samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samDesc.MaxAnisotropy = 1;
			samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samDesc.MaxLOD = D3D11_FLOAT32_MAX;

			ID3D11SamplerState * sampler;
			AccessDX11Device()->CreateSamplerState(&samDesc, &sampler);

			g_samplerStorage[type] = std::shared_ptr<ID3D11SamplerState>(sampler, [](IUnknown * p) {p->Release(); });
		}

		return g_samplerStorage[type].get();
	}

	ID3D11VertexShader * AccessVertexShader(const std::string & name, model::Model * model) {
		if (g_vertexShaderStorage.count(name) == 0) {
			auto vs = d3::shader::CompileShader(_T("Sample.hlsl"), "RenderVS", VERTEX_SHADER_PROFILE);
			g_vertexShaderStorage[name] = d3::shader::CreateVertexShader(AccessDX11Device(), vs.get());

			D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
					{ "IN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "IN_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "IN_UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					//{ "IN_BONEINDEX", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					//{ "IN_BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			auto key = std::make_pair(g_vertexShaderStorage[name].get(), model);
			g_layoutStorage[key] = d3::shader::CreateInputLayout(AccessDX11Device(), layoutDesc, _countof(layoutDesc), vs.get());
		}

		return g_vertexShaderStorage[name].get();
	}

	ID3D11PixelShader * AccessPixelShader(const std::string & name) {
		if (g_pixelShaderStorage.count(name) == 0) {
			auto ps = d3::shader::CompileShader(_T("Sample.hlsl"), "RenderPS", PIXEL_SHADER_PROFILE);
			g_pixelShaderStorage[name] = d3::shader::CreatePixelShader(AccessDX11Device(), ps.get());
		}

		return g_pixelShaderStorage[name].get();
	}

	ID3D11InputLayout * AccessLayout(ID3D11VertexShader * vs, model::Model * model) {
		// model::Model“à‚ÌVertex‚ÌŒ^‚Í“ˆê‚³‚ê‚Ä‚¢‚é‚à‚Ì‚Æ‚·‚é
		auto key = std::make_pair(vs, model);
		return g_layoutStorage[key].get();
	}


	ID3D11Buffer * AccessVertexBuffer(int index, model::Model * model) {
		auto key = std::make_pair(index, model);
		if (g_vertexBufferStorage.count(key) == 0) {
			g_vertexBufferStorage[key] = d3::buffer::CreateVertexBuffer(
				AccessDX11Device(), (void*)model->getMesh(index).vertexList.data(),
				sizeof(model::element::Vertex) * model->getMesh(index).vertexList.size()
			);
		}

		return g_vertexBufferStorage[key].get();
	}

	ID3D11Buffer * AccessIndexBuffer(int index, model::Model * model) {
		auto key = std::make_pair(index, model);
		if (g_indexBufferStorage.count(key) == 0) {
			g_indexBufferStorage[key] = d3::buffer::CreateIndexBuffer(
				AccessDX11Device(), (void*)model->getMesh(index).indexList.data(),
				sizeof(INDEX_FORMAT_TYPE)* model->getMesh(index).indexList.size()
			);

		}

		return g_indexBufferStorage[key].get();
	}

	unsigned int AccessVertexStride(model::Model * model) {
		return  sizeof(model::element::Vertex);
	}
	DXGI_FORMAT AccessIndexBufferType(model::Model * model) {
		return INDEX_FORMAT_ENUM;
	}
	D3D11_PRIMITIVE_TOPOLOGY AccessPrimitiveTopology(model::Model * model) {
		return D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}