#include "FBXParseMesh.h"
#include <algorithm>

#include <fbxsdk.h>
#include "FbxCommon.h"

using namespace FBXSDK_NAMESPACE;
using namespace DirectX;

namespace fbx{
	namespace parse{
		namespace mesh{
			namespace detail{
				namespace {
					namespace {
						std::vector<XMFLOAT2> MetaFunction(FbxGeometryElementUV*);
						std::vector<XMFLOAT3> MetaFunction(FbxGeometryElementNormal*);
						template<class T> inline void emplace_back(std::vector<XMFLOAT2> * v, const T & e) { v->emplace_back(e[0], e[1]); }
						template<class T> inline void emplace_back(std::vector<XMFLOAT3> * v, const T & e) { v->emplace_back(e[0], e[1], e[2]); }
						inline int distance(std::vector<int>::const_iterator a, std::vector<int>::const_iterator b) { return std::distance(a, b); }
						inline int distance(int a, int b) { return b - a; }

						template<class T> inline auto Access(T t) -> decltype(*t) { return *t; }
						inline int Access(int i) { return i; }
					}

					template<class T, class V> auto getGeometryElement(
						V begin, V end, T * element
						) -> decltype(MetaFunction(element)) {
						const auto& indexArray = element->GetIndexArray();
						const auto& directArray = element->GetDirectArray();
						const auto referenceMode = element->GetReferenceMode();
						assert((referenceMode == FbxGeometryElement::eDirect) || (referenceMode == FbxGeometryElement::eIndexToDirect));

						decltype(MetaFunction(element)) list;
						list.reserve(distance(begin, end));
						for (; begin != end; ++begin) {
							auto v = directArray.GetAt(
								(referenceMode == FbxGeometryElement::eDirect) ? Access(begin) : indexArray.GetAt(Access(begin))
								);
							emplace_back(&list, v);
						}
						return list;
					}
				}

				std::vector<int> getIndexList(FbxMesh* mesh) {
					auto polygonCount = mesh->GetPolygonCount();

					std::vector<int> indexList;
					indexList.reserve(polygonCount * 3);

					for (int i = 0; i < polygonCount; ++i) {
						indexList.emplace_back(mesh->GetPolygonVertex(i, 0));
						indexList.emplace_back(mesh->GetPolygonVertex(i, 1));
						indexList.emplace_back(mesh->GetPolygonVertex(i, 2));
					}

					return indexList;
				}

				std::vector<XMFLOAT3> getPositionList(FbxMesh* mesh, const std::vector<int>& indexList) {
					std::vector<XMFLOAT3> positionList;
					positionList.reserve(indexList.size());

					for (auto index : indexList) {
						auto controlPoint = mesh->GetControlPointAt(index);
						emplace_back(&positionList, controlPoint);

						assert(controlPoint[3] == 0.0);
					}

					return positionList;
				}

				// ByControlPoint  : ���_���W���X�g�Ɠ�����
				// ByPolygonVertex : ���_�C���f�b�N�X���X�g�Ɠ�����

				std::vector<XMFLOAT3> getNormalList(FbxMesh* mesh, const std::vector<int>& indexList) {
					assert(mesh->GetElementNormalCount() == 1); // �@���v�f��1�̂ݑΉ�
					auto element = mesh->GetElementNormal();
					switch (element->GetMappingMode()) {
					case FbxGeometryElement::eByControlPoint: return getGeometryElement(indexList.begin(), indexList.end(), element);
					case FbxGeometryElement::eByPolygonVertex: return getGeometryElement(0, mesh->GetPolygonCount() * 3, element);
					default: assert(true);
					}
				}

				std::vector<XMFLOAT2> getUVList(FbxMesh* mesh, const std::vector<int>& indexList, int uvNo) {
					if (uvNo + 1 > mesh->GetElementUVCount()) return std::vector<XMFLOAT2>();
					auto element = mesh->GetElementUV(uvNo);
					switch (element->GetMappingMode()) {
					case FbxGeometryElement::eByControlPoint: return getGeometryElement(indexList.begin(), indexList.end(), element);
					case FbxGeometryElement::eByPolygonVertex: return getGeometryElement(0, mesh->GetPolygonCount() * 3, element);
					default: assert(true);
					}
				}


				struct BoneWeight {
					uint8_t boneIndex[4];
					DirectX::XMFLOAT4 boneWeight;
				};
				void getWeight(
					FbxMesh* mesh, const std::vector<int>& indexList,
					std::vector<BoneWeight>& boneWeightList, std::vector<std::string>& boneNodeNameList,
					std::vector<XMFLOAT4X4>& invBaseposeMatrixList
					) {

					auto skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
					if (skinCount == 0) {
						return;
					}

					// �X�L����1�����Ή����Ȃ�
					assert(skinCount <= 1);

					auto controlPointsCount = mesh->GetControlPointsCount();
					using TmpWeight = std::pair<int, float>;
					std::vector<std::vector<TmpWeight>> tmpBoneWeightList(controlPointsCount);

					auto skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
					auto clusterCount = skin->GetClusterCount();

					for (int i = 0; i < clusterCount; ++i) {
						auto cluster = skin->GetCluster(i);

						// eNormalize�����Ή����Ȃ�
						assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

						boneNodeNameList.emplace_back(cluster->GetLink()->GetName());

						auto indexCount = cluster->GetControlPointIndicesCount();
						auto indices = cluster->GetControlPointIndices();
						auto weights = cluster->GetControlPointWeights();

						for (int j = 0; j < indexCount; ++j) {
							auto controlPointIndex = indices[j];
							tmpBoneWeightList[controlPointIndex].push_back({ i, weights[j] });
						}


						// �y�[�X�|�[�Y�̋t�s����쐬���Ă���
						XMFLOAT4X4 invBaseposeMatrix;
						XMStoreFloat4x4(&invBaseposeMatrix, convertFbx2XM(cluster->GetLink()->EvaluateGlobalTransform().Inverse()));
						invBaseposeMatrixList.emplace_back(invBaseposeMatrix);
					}


					// �R���g���[���|�C���g�ɑΉ������E�F�C�g���쐬
					std::vector<BoneWeight> boneWeightListControlPoints;
					for (auto& tmpBoneWeight : tmpBoneWeightList) {
						// �E�F�C�g�̑傫���Ń\�[�g
						std::sort(tmpBoneWeight.begin(), tmpBoneWeight.end(),
							[](const TmpWeight& weightA, const TmpWeight& weightB) { return weightA.second > weightB.second; }
						//[](const TmpWeight& weightA, const TmpWeight& weightB){ return weightA.second < weightB.second; }
						);

						// 1���_��4��葽���E�F�C�g������U���Ă���Ȃ�e�������Ȃ����͖̂�������
						while (tmpBoneWeight.size() > 4) {
							tmpBoneWeight.pop_back();
						}

						// 4�ɖ����Ȃ��ꍇ�̓_�~�[��}��
						while (tmpBoneWeight.size() < 4) {
							tmpBoneWeight.push_back({ 0, 0.0f });
						}

						BoneWeight weight;
						for (int i = 0; i < 4; ++i) {
							weight.boneIndex[i] = tmpBoneWeight[i].first;
						}
						weight.boneWeight = DirectX::XMFLOAT4(
							tmpBoneWeight[0].second, tmpBoneWeight[1].second, tmpBoneWeight[4].second, tmpBoneWeight[3].second
							);
						XMStoreFloat4(&weight.boneWeight, XMVector4Normalize(XMLoadFloat4(&weight.boneWeight)));

						boneWeightListControlPoints.emplace_back(weight);
					}

					// �C���f�b�N�X�œW�J
					for (auto index : indexList) {
						boneWeightList.emplace_back(boneWeightListControlPoints[index]);
					}
				}
			}

			model::element::Mesh parseMesh(FbxMesh* mesh) {
				assert(mesh != nullptr);

				auto node = mesh->GetNode();

				// �}�e���A�����P�̂��̂����Ή����Ȃ�
				assert(node->GetMaterialCount() == 1);

				model::element::Mesh modelMesh;
				modelMesh.nodeName = node->GetName();
				modelMesh.materialName = node->GetMaterial(0)->GetName();

				// �y�[�X�|�[�Y�̋t�s����쐬���Ă���
				XMStoreFloat4x4(&modelMesh.invMeshBaseposeMatrix, convertFbx2XM(node->EvaluateGlobalTransform().Inverse()));

				auto translation = node->LclTranslation.Get();
				XMStoreFloat4x4(&modelMesh.meshBaseposeMatrix, XMMatrixTranslation(translation[0], translation[1], translation[2]));

				// �C���f�b�N�X�擾
				auto indexList = detail::getIndexList(mesh);

				// �S�ēW�J������Ԃ̒��_�擾
				auto positionList = detail::getPositionList(mesh, indexList);
				auto normalList = detail::getNormalList(mesh, indexList);
				auto uv0List = detail::getUVList(mesh, indexList, 0);

				std::vector<detail::BoneWeight> boneWeightList;
				//getWeight(mesh, indexList, boneWeightList, modelMesh.boneNodeNameList, modelMesh.invBoneBaseposeMatrixList);

				// �O�̂��߃T�C�Y�`�F�b�N
				assert(indexList.size() == positionList.size());
				assert(indexList.size() == normalList.size());
				//assert((indexList.size() == boneWeightList.size()) || (boneWeightList.size() == 0));

				// ���_���C���^�[���[�u�h�A���C��
				std::vector<model::element::Vertex> modelVertexList;
				modelVertexList.reserve(indexList.size());

				for (int i = 0; i < indexList.size(); ++i) {
					model::element::Vertex vertex;
					vertex.position = positionList[i];
					vertex.normal = normalList[i];
					vertex.uv0 = (uv0List.size() == 0) ? XMFLOAT2(0.0f, 0.0f) : uv0List[i];
					/*
					if (boneWeightList.size() > 0) {
						for (int j = 0; j < 4; ++j) vertex.boneIndex[j] = boneWeightList[i].boneIndex[j];
						vertex.boneWeight = boneWeightList[i].boneWeight;
					} else {
						for (int j = 0; j < 4; ++j) vertex.boneIndex[j] = 0;
						vertex.boneWeight = XMFLOAT4(1, 0, 0, 0);
					}*/

					modelVertexList.emplace_back(vertex);
				}

				// �d�����_�������ăC���f�b�N�X�쐬
				auto& modelVertexListOpt = modelMesh.vertexList;
				modelVertexListOpt.reserve(modelVertexList.size());
				auto& modelIndexList = modelMesh.indexList;
				modelIndexList.reserve(indexList.size());

				std::map<model::element::Vertex, int> exist; // �����������p
				for (auto& vertex : modelVertexList) {
					if (exist.count(vertex) == 0) {
						modelIndexList.emplace_back(modelVertexListOpt.size());

						exist.emplace(vertex, modelVertexListOpt.size());
						modelVertexListOpt.emplace_back(vertex);
					} else {
						modelIndexList.emplace_back(exist[vertex]);
					}
				}

				return modelMesh;
			}
		}
	}
}