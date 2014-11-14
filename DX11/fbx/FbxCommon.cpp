#include "FbxCommon.h"

#include <fbxsdk.h>

#pragma comment( lib, "wininet.lib" )
#pragma comment( lib, "advapi32.lib" )
#pragma comment( lib, "libfbxsdk-md.lib" )

namespace fbx{
	DirectX::XMMATRIX convertFbx2XM(const FBXSDK_NAMESPACE::FbxAMatrix & baseposeMatrix) {
		DirectX::XMFLOAT4X4 invBaseposeMatrix;
		for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) invBaseposeMatrix.m[i][j] = baseposeMatrix[i][j];
		return DirectX::XMLoadFloat4x4(&invBaseposeMatrix);
	}
}