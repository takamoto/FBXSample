#include "FBXParseMaterial.h"
#include <string>
#include <fbxsdk.h>

using namespace FBXSDK_NAMESPACE;
using namespace DirectX;

namespace fbx{
	namespace parse{
		namespace material{
			namespace detail{
				std::string getTexture(const FbxProperty & property) {
					int fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();
					if (0 >= fileTextureCount) return "";

					for (int j = 0; fileTextureCount > j; j++) {
						auto texture = property.GetSrcObject<FbxFileTexture>(j);
						if (!texture) continue;

						return texture->GetRelativeFileName();
					}
					return "";
				}

				XMFLOAT3 getProperty(
					const FbxSurfaceMaterial * material,
					const char * pPropertyName,
					const char * pFactorPropertyName,
					XMFLOAT3 default = XMFLOAT3(0, 0, 0)
					) {
					auto property = material->FindProperty(pPropertyName);
					auto factor = material->FindProperty(pFactorPropertyName);
					if (property.IsValid() && factor.IsValid()) {
						auto f = factor.Get<FbxDouble>();
						auto _ = property.Get<FbxDouble3>();
						return XMFLOAT3(_[0] * f, _[1] * f, _[2] * f);
					}
					return default;
				}

			}

			model::element::Material parseMaterial(FbxSurfaceMaterial* material) {
				model::element::Material modelMaterial;
				modelMaterial.materialName = material->GetName();

				modelMaterial.emissive = detail::getProperty(material, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
				modelMaterial.ambient = detail::getProperty(material, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
				modelMaterial.diffuse = detail::getProperty(material, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, XMFLOAT3(1, 1, 1));
				modelMaterial.specular = detail::getProperty(material, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);

				auto transparency = material->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
				modelMaterial.transparency = (transparency.IsValid()) ? transparency.Get<FbxDouble>() : 1;
				auto shininess = material->FindProperty(FbxSurfaceMaterial::sShininess);
				modelMaterial.shininess = (shininess.IsValid()) ? shininess.Get<FbxDouble>() : 1;


				modelMaterial.diffuseTextureName = detail::getTexture(material->FindProperty(FbxSurfaceMaterial::sDiffuse));
				modelMaterial.normalTextureName = detail::getTexture(material->FindProperty(FbxSurfaceMaterial::sBump));
				modelMaterial.reflectionMapTextureName = detail::getTexture(material->FindProperty(FbxSurfaceMaterial::sReflection));
				modelMaterial.specularTextureName = detail::getTexture(material->FindProperty(FbxSurfaceMaterial::sSpecular));
				//modelMaterial.falloffTextureName = detail::getTexture(material->FindProperty(FbxSurfaceMaterial::));


				// CGFX‚Ì‚Ý‘Î‰ž
				auto implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_HLSL);
				if (implementation != nullptr) {
					auto rootTable = implementation->GetRootTable();
					auto entryCount = rootTable->GetEntryCount();

					for (int i = 0; i < entryCount; ++i) {
						auto entry = rootTable->GetEntry(i);

						auto fbxProperty = material->FindPropertyHierarchical(entry.GetSource());
						if (!fbxProperty.IsValid()) {
							fbxProperty = material->RootProperty.FindHierarchical(entry.GetSource());
						}

						auto textureCount = fbxProperty.GetSrcObjectCount<FbxTexture>();
						if (textureCount > 0) {
							std::string src = entry.GetSource();

							for (int j = 0; j < fbxProperty.GetSrcObjectCount<FbxFileTexture>(); ++j) {
								auto tex = fbxProperty.GetSrcObject<FbxFileTexture>(j);
								std::string texName = tex->GetFileName();
								texName = texName.substr(texName.find_last_of('/') + 1);

								if (src == "Maya|DiffuseTexture") {
									modelMaterial.diffuseTextureName = texName;
								} else if (src == "Maya|NormalTexture") {
									modelMaterial.normalTextureName = texName;
								} else if (src == "Maya|SpecularTexture") {
									modelMaterial.specularTextureName = texName;
								} else if (src == "Maya|FalloffTexture") {
									modelMaterial.falloffTextureName = texName;
								} else if (src == "Maya|ReflectionMapTexture") {
									modelMaterial.reflectionMapTextureName = texName;
								}
							}
						}
					}

					return modelMaterial;
				}

				return modelMaterial;
			}
		}
	}
}