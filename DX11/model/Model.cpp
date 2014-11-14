#include "Model.h"

#include "../fbx/FbxCommon.h"
#include "Animation.h"

using namespace DirectX;
using namespace FBXSDK_NAMESPACE;

namespace model {
	/*
	DirectX::XMMATRIX GetMeshMatrix(
		const element::Mesh & modelMesh,
		const std::shared_ptr<Animation> & animation, float frame
		) {
		if (!animation) return XMLoadFloat4x4(&modelMesh.meshBaseposeMatrix);
		auto meshNode = animation->getNode(modelMesh.nodeName);
		if (!meshNode) return XMMatrixIdentity();

		FbxTime time;
		time.Set(FbxTime::GetOneFrameValue(FbxTime::eFrames60) * frame);

		return XMMatrixMultiply(
			fbx::convertFbx2XM(meshNode->EvaluateGlobalTransform(time)),
			XMLoadFloat4x4(&modelMesh.invMeshBaseposeMatrix)
			);
	}

	void GetBoneMatrix(
		const element::Mesh & modelMesh,
		const std::shared_ptr<Animation> & animation, float frame,
		XMFLOAT4X4* out_matrixList, int matrixCount
		) {
		if (modelMesh.boneNodeNameList.size() == 0) {
			XMStoreFloat4x4(out_matrixList, XMMatrixIdentity());
			return;
		}

		assert(modelMesh.boneNodeNameList.size() <= matrixCount);

		FbxTime time;
		time.Set(FbxTime::GetOneFrameValue(FbxTime::eFrames60) * frame);

		for (int i = 0; i < modelMesh.boneNodeNameList.size(); ++i) {
			auto& boneNodeName = modelMesh.boneNodeNameList[i];
			auto boneNode = animation->getNode(boneNodeName);
			XMStoreFloat4x4(
				&out_matrixList[i],
				XMMatrixMultiply(
				fbx::convertFbx2XM(boneNode->EvaluateGlobalTransform(time)),
				XMLoadFloat4x4(&modelMesh.invBoneBaseposeMatrixList[i])
				)
				);
		}
	}*/

}