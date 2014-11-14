#pragma once
#include <fbxsdk\fbxsdk_version.h>
#include "../model/Model.h"

namespace FBXSDK_NAMESPACE{
	class FbxScene;
}

namespace fbx{
	namespace parse{
		// FBX�`���̃��f����ėp�I�ȃN���X�i�̂���j�ɕϊ�
		std::shared_ptr<model::Model> parseModel(FBXSDK_NAMESPACE::FbxScene * scene);
	}
}