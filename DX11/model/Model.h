#pragma once
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <DirectXMath.h>


namespace model{
	/*
	* モデルの内部データ形式
	*/
	namespace element{
		struct Vertex {
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 uv0;
			/*
			uint8_t boneIndex[4];
			DirectX::XMFLOAT4 boneWeight;
			*/
			bool operator < (const Vertex& v) const {
				return std::memcmp(this, &v, sizeof(Vertex)) < 0;
			}
		};

		struct Mesh {
			std::string nodeName;
			std::string materialName;

			std::vector<Vertex> vertexList;
			std::vector<uint16_t> indexList;

			DirectX::XMFLOAT4X4 invMeshBaseposeMatrix;
			DirectX::XMFLOAT4X4 meshBaseposeMatrix;
			std::vector<std::string> boneNodeNameList;
			std::vector<DirectX::XMFLOAT4X4> invBoneBaseposeMatrixList;
		};

		struct Material {
			std::string materialName;

			DirectX::XMFLOAT3 ambient;
			DirectX::XMFLOAT3 diffuse;
			DirectX::XMFLOAT3 emissive;
			double transparency;

			DirectX::XMFLOAT3 specular;
			double shininess;

			std::string diffuseTextureName;
			std::string normalTextureName;
			std::string specularTextureName;
			std::string falloffTextureName;
			std::string reflectionMapTextureName;
		};
	}

	/*
	* 汎用的な(つもりの)モデルデータ
	*/
	class Model {
	private:
		std::vector<element::Mesh> _meshList;
		std::map<std::string, int> _nodeIdDictionary;

		std::vector<element::Material> _materialList;
		std::map<std::string, int> _materialIdDictionary;
	public:
		Model(
			std::vector<element::Mesh> meshList,
			std::vector<element::Material> materialList,

			std::map<std::string, int> nodeIdDictionary,
			std::map<std::string, int> materialIdDictionary
		) : _meshList(meshList),
			_materialList(materialList),
			_nodeIdDictionary(nodeIdDictionary),
			_materialIdDictionary(materialIdDictionary) {};

		const element::Mesh & getMesh(int index) const { return _meshList[index]; };
		int getMeshCount() const { return _meshList.size(); }
		const element::Material & getMaterial(int index) const { return _materialList[index]; };
		int getMaterialID(const std::string & materialName) const { return _materialIdDictionary.at(materialName); }
		int getMaterialCount() const { return _materialList.size(); }
	};
	/*
	class Animation;
	DirectX::XMMATRIX GetMeshMatrix(
		const element::Mesh & modelMesh,
		const std::shared_ptr<Animation> & animation, float frame
	);

	void GetBoneMatrix(
		const element::Mesh & modelMesh,
		const std::shared_ptr<Animation> & animation, float frame,
		DirectX::XMFLOAT4X4* out_matrixList, int matrixCount
	);*/
}