#pragma once
#include <fbxsdk.h>
#include <memory>
#include <map>
#include <string>

namespace model{
	/*
	class Animation {
	private:
		std::shared_ptr<FbxScene> _animation;

		std::map<std::string, int> _nodeIdDictionaryAnimation;
		//float _animationStartFrame;
		//float _animationEndFrame;
	public:
		Animation(std::shared_ptr<FbxScene> animation) {
			_animation = animation;

			// ƒm[ƒh–¼‚©‚çƒm[ƒhID‚ðŽæ“¾‚Å‚«‚é‚æ‚¤‚ÉŽ«‘‚É“o˜^
			auto nodeCount = animation->GetNodeCount();
			for (int i = 0; i < nodeCount; ++i) {
				auto fbxNode = animation->GetNode(i);
				_nodeIdDictionaryAnimation.insert({ fbxNode->GetName(), i });
			}
		}

		FbxNode * getNode(const std::string & nodeName) {
			auto nodeId = _nodeIdDictionaryAnimation.at(nodeName);
			return _animation->GetNode(nodeId);
		}
	};*/
}