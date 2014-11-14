#pragma once
#include <fbxsdk\fbxsdk_version.h>
#include "../model/Model.h"

namespace FBXSDK_NAMESPACE{
	class FbxScene;
}

namespace fbx{
	namespace parse{
		// FBX形式のモデルを汎用的なクラス（のつもり）に変換
		std::shared_ptr<model::Model> parseModel(FBXSDK_NAMESPACE::FbxScene * scene);
	}
}