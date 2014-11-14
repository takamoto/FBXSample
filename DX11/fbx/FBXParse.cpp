#pragma once
#include <map>
#include <vector>
#include <fbxsdk.h>
#include "FBXParseMesh.h"
#include "FBXParseMaterial.h"

using namespace FBXSDK_NAMESPACE;
using namespace DirectX;

namespace fbx{
	namespace parse{
		std::shared_ptr<model::Model> parseModel(FbxScene * scene) {
			std::vector<model::element::Mesh> meshList;
			std::vector<model::element::Material> materialList;

			std::map<std::string, int> materialIdDictionary;
			std::map<std::string, int> nodeIdDictionary;

			// ノード名からノードIDを取得できるように辞書に登録
			auto nodeCount = scene->GetNodeCount();
			for (int i = 0; i < nodeCount; ++i) {
				nodeIdDictionary.emplace(scene->GetNode(i)->GetName(), i);
			}
			
			// シーンに含まれるマテリアルの解析
			auto materialCount = scene->GetMaterialCount();
			materialList.reserve(materialCount);
			for (int i = 0; i < materialCount; ++i) {
				auto & modelMaterial = material::parseMaterial(scene->GetMaterial(i));
				materialList.emplace_back(modelMaterial);
				materialIdDictionary.emplace(modelMaterial.materialName, i);
			}

			// シーンに含まれるメッシュの解析
			auto meshCount = scene->GetMemberCount<FbxMesh>();
			meshList.reserve(meshCount);
			for (int i = 0; i < meshCount; ++i) {
				meshList.emplace_back(
					mesh::parseMesh(scene->GetMember<FbxMesh>(i))
					);
			}

			return std::make_shared<model::Model>(meshList, materialList, nodeIdDictionary, materialIdDictionary);
		}
	}
}