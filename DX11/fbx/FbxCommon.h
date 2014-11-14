#pragma once
#include <DirectXMath.h>
#include <fbxsdk\fbxsdk_version.h>

namespace FBXSDK_NAMESPACE{
	class FbxAMatrix;
}

namespace fbx{
	DirectX::XMMATRIX convertFbx2XM(const FBXSDK_NAMESPACE::FbxAMatrix & baseposeMatrix);
}