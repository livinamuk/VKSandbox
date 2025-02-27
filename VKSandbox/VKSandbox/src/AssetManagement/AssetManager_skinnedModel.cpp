#include "AssetManager.h"
#include "Util/Util.h"
#include "File/AssimpImporter.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h> 
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <future>
#include <numeric>
#include "HellTypes.h"

namespace AssetManager {

    void LoadPendingSkinnedModelsAsync() {
        for (SkinnedModel& skinnedModel : GetSkinnedModels()) {
            if (skinnedModel.GetLoadingState() == LoadingState::AWAITING_LOADING_FROM_DISK) {
                skinnedModel.SetLoadingState(LoadingState::LOADING_FROM_DISK);
                AddItemToLoadLog(skinnedModel.GetFileInfo().path);
                std::async(std::launch::async, LoadSkinnedModel, &skinnedModel);
                return;
            }
        }
    }
    
    void GrabSkeleton(std::vector<Node>& nodes, const aiNode* pNode, int parentIndex) {
        // Create the joint node
        Node node;
        node.name = Util::CopyConstChar(pNode->mName.C_Str());
        node.inverseBindTransform = Util::aiMatrix4x4ToGlm(pNode->mTransformation);
        node.parentIndex = parentIndex;

        // Determine the current node's index and push it
        int currentIndex = static_cast<int>(nodes.size());
        nodes.push_back(node);

        // Recursively process children using the current node's index as parentIndex
        for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
            GrabSkeleton(nodes, pNode->mChildren[i], currentIndex);
        }
    }

    void AssetManager::LoadSkinnedModel(SkinnedModel* skinnedModel) {

        const FileInfo& fileInfo = skinnedModel->GetFileInfo();
        std::string assetPath = "res/skinned_models/" + fileInfo.name + ".skinnedmodel";
        SkinnedModelData skinnedModelData = File::ImportSkinnedModel(assetPath);

        skinnedModel->Load(skinnedModelData);
        skinnedModel->SetLoadingState(LoadingState::LOADING_COMPLETE);

        return;



        int totalVertexCount = 0;
        int baseVertexLocal = 0;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(fileInfo.path, aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene) {
            std::cout << "Something fucked up loading your skinned model: " << fileInfo.path << "\n";
            std::cout << "Error: " << importer.GetErrorString() << "\n";
            return;
        }

        // Load bones
        int foundBoneCount = 0;
        for (int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* assimpMesh = scene->mMeshes[i];

            for (unsigned int j = 0; j < assimpMesh->mNumBones; j++) {
                const aiBone* bone = assimpMesh->mBones[j];
                std::string boneName = bone->mName.data;

                // If this bone isn't mapped yet, add it to the global list
                bool inserted = skinnedModel->m_boneMapping.find(boneName) != skinnedModel->m_boneMapping.end();
                if (!inserted) {

                    // Map bone name to index
                    unsigned int boneIndex = foundBoneCount++;
                    skinnedModel->m_boneMapping[boneName] = boneIndex;

                    // Store bone info
                    glm::mat4 boneOffset = Util::aiMatrix4x4ToGlm(bone->mOffsetMatrix);
                    skinnedModel->m_boneOffsets.push_back(boneOffset);
                }
            }
        }

        // Get vertex data
        for (int i = 0; i < scene->mNumMeshes; i++) {

            glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
            glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
            const aiMesh* assimpMesh = scene->mMeshes[i];
            int vertexCount = assimpMesh->mNumVertices;
            int indexCount = assimpMesh->mNumFaces * 3;
            std::string meshName = assimpMesh->mName.C_Str();
            std::vector<WeightedVertex> vertices;
            std::vector<uint32_t> indices;

            // Get vertices
            for (unsigned int j = 0; j < vertexCount; j++) {
                WeightedVertex vertex;
                vertex.position = { assimpMesh->mVertices[j].x, assimpMesh->mVertices[j].y, assimpMesh->mVertices[j].z };
                vertex.normal = { assimpMesh->mNormals[j].x, assimpMesh->mNormals[j].y, assimpMesh->mNormals[j].z };
                vertex.tangent = { assimpMesh->mTangents[j].x, assimpMesh->mTangents[j].y, assimpMesh->mTangents[j].z };
                vertex.uv = { assimpMesh->HasTextureCoords(0) ? glm::vec2(assimpMesh->mTextureCoords[0][j].x, assimpMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f, 0.0f) };
                vertices.push_back(vertex);
                aabbMin.x = std::min(aabbMin.x, vertex.position.x);
                aabbMin.y = std::min(aabbMin.y, vertex.position.y);
                aabbMin.z = std::min(aabbMin.z, vertex.position.z);
                aabbMax.x = std::max(aabbMax.x, vertex.position.x);
                aabbMax.y = std::max(aabbMax.y, vertex.position.y);
                aabbMax.z = std::max(aabbMax.z, vertex.position.z);
            }
            // Get indices
            for (unsigned int j = 0; j < assimpMesh->mNumFaces; j++) {
                const aiFace& Face = assimpMesh->mFaces[j];
                indices.push_back(Face.mIndices[0]);
                indices.push_back(Face.mIndices[1]);
                indices.push_back(Face.mIndices[2]);
            }

            // Get vertex weights and bone IDs
            std::vector<unsigned int> influenceCount(vertices.size(), 0);

            for (unsigned int i = 0; i < assimpMesh->mNumBones; i++) {
                std::string boneName = assimpMesh->mBones[i]->mName.data;
                unsigned int boneIndex = skinnedModel->m_boneMapping[boneName];

                for (unsigned int j = 0; j < assimpMesh->mBones[i]->mNumWeights; j++) {
                    unsigned int vertexIndex = assimpMesh->mBones[i]->mWeights[j].mVertexId;
                    float weight = assimpMesh->mBones[i]->mWeights[j].mWeight;
                    WeightedVertex& vertex = vertices[vertexIndex];

                    if (influenceCount[vertexIndex] < 4) {
                        switch (influenceCount[vertexIndex]) {
                            case 0:
                            vertex.boneID.x = boneIndex;
                            vertex.weight.x = weight;
                            break;
                            case 1:
                            vertex.boneID.y = boneIndex;
                            vertex.weight.y = weight;
                            break;
                            case 2:
                            vertex.boneID.z = boneIndex;
                            vertex.weight.z = weight;
                            break;
                            case 3:
                            vertex.boneID.w = boneIndex;
                            vertex.weight.w = weight;
                            break;
                        }
                        influenceCount[vertexIndex]++;
                    }
                }
            }

            // Ingore broken weights
            float threshold = 0.05f;
            for (unsigned int j = 0; j < vertices.size(); j++) {
                WeightedVertex& vertex = vertices[j];
                std::vector<float> validWeights;
                for (int i = 0; i < 4; ++i) {
                    if (vertex.weight[i] < threshold) {
                        vertex.weight[i] = 0.0f;
                    }
                    else {
                        validWeights.push_back(vertex.weight[i]);
                    }
                }
                float sum = std::accumulate(validWeights.begin(), validWeights.end(), 0.0f);
                int validIndex = 0;
                for (int i = 0; i < 4; ++i) {
                    if (vertex.weight[i] > 0.0f) {
                        vertex.weight[i] = validWeights[validIndex] / sum;
                        validIndex++;
                    }
                }
            }

            static std::mutex mutex;
            std::lock_guard<std::mutex> lock(mutex);
            skinnedModel->AddMeshIndex(AssetManager::CreateSkinnedMesh(meshName, vertices, indices, baseVertexLocal, aabbMin, aabbMax));
            totalVertexCount += vertices.size();
            baseVertexLocal += vertices.size();
        }
        skinnedModel->SetVertexCount(totalVertexCount);

        // Recursively grab the skeleton
        std::vector<Node>& nodes = skinnedModel->m_nodes;
        GrabSkeleton(nodes, scene->mRootNode, -1);

        // Mark loading as complete
        skinnedModel->SetLoadingState(LoadingState::LOADING_COMPLETE);
 
        // Cleanup
        importer.FreeScene();
    }

    void ExportMissingSkinnedModels() {

        // Scan for new obj and fbx and export custom model format
        for (FileInfo& fileInfo : Util::IterateDirectory("res/skinned_models_raw", { "obj", "fbx" })) {
            std::string assetPath = "res/skinned_models/" + fileInfo.name + ".skinnedmodel";

            // If the file exists but timestamps don't match, re-export
            if (Util::FileExists(assetPath)) {
                uint64_t lastModified = File::GetLastModifiedTime(fileInfo.path);
                SkinnedModelHeader modelHeader = File::ReadSkinnedModelHeader(assetPath);
                //if (modelHeader.timestamp != lastModified || fileInfo.name == "Knife") {
                if (modelHeader.timestamp != lastModified) {
                    File::DeleteFile(assetPath);
                    SkinnedModelData modelData = AssimpImporter::ImportSkinnedFbx(fileInfo.path);
                    File::ExportSkinnedModel(modelData);
                }
            }
            // File doesn't even exist yet, so export it
            else {
                SkinnedModelData modelData = AssimpImporter::ImportSkinnedFbx(fileInfo.path);
                File::ExportSkinnedModel(modelData);
            }
        }
    }

    SkinnedModel* AssetManager::GetSkinnedModelByName(const std::string& name) {
        std::vector<SkinnedModel>& skinnedModels = GetSkinnedModels();
        for (auto& skinnedModel : skinnedModels) {
            if (name == skinnedModel.GetName()) {
                return &skinnedModel;
            }
        }
        std::cout << "AssetManager::GetSkinnedModelByName(const std::string& name) failed because '" << name << "' does not exist!\n";
        return nullptr;
    }

    SkinnedModel* AssetManager::GetSkinnedModelByIndex(int index) {
        std::vector<SkinnedModel>& skinnedModels = GetSkinnedModels();
        if (index >= 0 && index < skinnedModels.size()) {
            return &skinnedModels[index];
        }
        else {
            std::cout << "AssetManager::GetSkinnedModelByIndex(int index) failed because index '" << index << "' is out of range. Size is " << skinnedModels.size() << "!\n";
            return nullptr;
        }
    }

    int AssetManager::GetSkinnedModelIndexByName(const std::string& name) {
        std::vector<SkinnedModel>& skinnedModels = GetSkinnedModels();
        for (int i = 0; i < skinnedModels.size(); i++) {
            if (name == skinnedModels[i].GetName()) {
                return i;
            }
        }
        std::cout << "AssetManager::GetSkinnedModelIndexByName(const std::string& name) failed because '" << name << "' does not exist!\n";
        return -1;
    }
}