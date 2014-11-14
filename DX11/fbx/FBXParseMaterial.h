#pragma once
#include <fbxsdk\fbxsdk_version.h>
#include "../model/Model.h"

namespace FBXSDK_NAMESPACE{
	class FbxSurfaceMaterial;
}

namespace fbx{
	namespace parse{
		namespace material{
			model::element::Material parseMaterial(FBXSDK_NAMESPACE::FbxSurfaceMaterial* material);
		}
	}
}