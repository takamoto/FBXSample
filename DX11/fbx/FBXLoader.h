#pragma once
#include <memory>
#include <fbxsdk\fbxsdk_version.h>

namespace FBXSDK_NAMESPACE{
	class FbxManager;
	class FbxScene;
}

namespace model{
	class Model;
}

namespace fbx{
	class FBXLoader {
	private:
		std::shared_ptr<FBXSDK_NAMESPACE::FbxManager> _manager;
		void Init();
	public:
		FBXLoader() { Init(); }
		//std::shared_ptr<FBXSDK_NAMESPACE::FbxScene> loadAnimation(const char * filename);

		// FBX�t�@�C����ėp�I�ȃ��f���N���X(�̂���)�ɕϊ�
		std::shared_ptr<model::Model> loadModel(const char * filename);
	};
}
