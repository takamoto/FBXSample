#include "FBXLoader.h"
#include <fbxsdk.h>

#include "FBXParse.h"

using namespace FBXSDK_NAMESPACE;

namespace fbx{
	namespace{
		namespace{
			// fbxはutf-8に変換しないと日本語pathを扱えない
			// まるぺけの改良版
			std::string getUTF8AbsFilePath(const char* filePath) {
				std::unique_ptr<char, decltype(&free)> fullPath(_fullpath(NULL, filePath, 0), free);
				char * utf8path;
				FbxAnsiToUTF8(fullPath.get(), utf8path);
				std::unique_ptr<char> utf8path_up(utf8path);
				return utf8path;
			}

			// nameを真面目につける必要があるかは不明
			std::shared_ptr<FbxScene> createScene(FbxManager * manager, const char * name) {
				return std::shared_ptr<FbxScene>(
					FbxScene::Create(manager, name), [](FbxScene * p) { p->Destroy(); }
				);
			}
		}

		std::shared_ptr<FbxImporter> createImporter(FbxManager * manager) {
			return std::shared_ptr<FbxImporter>(
				FbxImporter::Create(manager, ""), [](FbxImporter * p) { p->Destroy(); }
			);
		}

		// FBXファイルの読み込み
		std::shared_ptr<FbxScene> importScene(
			FbxManager * manager, FbxImporter * importer, const char * filename, const char * name
		) {
			auto path = getUTF8AbsFilePath(filename);
			if (!importer->Initialize(path.c_str(), -1, manager->GetIOSettings())) return false;

			/*{
				// FBX ファイルのバージョン取得。
				int major, minor, revision;
				importer->GetFileVersion(major, minor, revision);
				FBXSDK_printf("FBX VERSION %d %d %d\n", major, minor, revision);
			}*/

			auto scene = createScene(manager, name);
			importer->Import(scene.get());

			return scene;
		}

	}

	void FBXLoader::Init() {
		_manager = std::shared_ptr<FbxManager>(FbxManager::Create(), [](FbxManager *p) {
			p->Destroy();
		});
		_manager->SetIOSettings(FbxIOSettings::Create(_manager.get(), IOSROOT));

		// optional
		_manager->LoadPluginsDirectory(FbxGetApplicationDirectory().Buffer(), "dll"); // windowsのみ対応
	}

	/*
	std::shared_ptr<FbxScene> FBXLoader::loadAnimation(const char * filename) {
		auto importer = createImporter(_manager.get());
		auto animation = importScene(_manager.get(), importer.get(), filename, "animationScene");
		auto meshAxisSystem = animation->GetGlobalSettings().GetAxisSystem();
		if (meshAxisSystem != FbxAxisSystem::DirectX) {
			FbxAxisSystem::DirectX.ConvertScene(animation.get());
		}

		// アニメーションフレーム数取得
		auto animStackCount = importer->GetAnimStackCount();
		assert(animStackCount == 1);
		auto takeInfo = importer->GetTakeInfo(0);

		auto importOffset = takeInfo->mImportOffset;
		auto startTime = takeInfo->mLocalTimeSpan.GetStart();
		auto stopTime = takeInfo->mLocalTimeSpan.GetStop();

		auto animationStartFrame = (importOffset.Get() + startTime.Get()) / FbxTime::GetOneFrameValue(FbxTime::eFrames60);
		auto animationEndFrame = (importOffset.Get() + stopTime.Get()) / FbxTime::GetOneFrameValue(FbxTime::eFrames60);

		return animation;
	}*/

	std::shared_ptr<model::Model> FBXLoader::loadModel(const char * filename) {
		auto mesh = importScene(_manager.get(), createImporter(_manager.get()).get(), filename, "");
		auto meshAxisSystem = mesh->GetGlobalSettings().GetAxisSystem();
		if (meshAxisSystem != FbxAxisSystem::DirectX) {
			FbxAxisSystem::DirectX.ConvertScene(mesh.get());
		}

		// 三角形化
		FbxGeometryConverter(_manager.get()).Triangulate(mesh.get(), true);

		return parse::parseModel(mesh.get());
	}
}
