#pragma once
#include <fbxsdk\fbxsdk_version.h>
#include "../model/Model.h"

namespace FBXSDK_NAMESPACE{
	class FbxMesh;
}

namespace fbx{
	namespace parse{
		namespace mesh{
			model::element::Mesh parseMesh(FBXSDK_NAMESPACE::FbxMesh* mesh);
		}
	}
}