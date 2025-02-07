#include "AssetManager.h"
#include "Util/Util.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h> 
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <future>
#include <numeric>

namespace AssetManager {

    void LoadPendingSkinnedModelsAsync() {
        for (SkinnedModel& skinnedModel : GetSkinnedModels()) {
            if (skinnedModel.GetLoadingState() == LoadingState::AWAITING_LOADING_FROM_DISK) {
                skinnedModel.SetLoadingState(LoadingState::LOADING_FROM_DISK);
                AddItemToLoadLog(skinnedModel.GetFileInfo().path);
                std::async(std::launch::async, LoadSkinnedModel, &skinnedModel);
                //LoadSkinnedModel(&skinnedModel);
                return;
            }
        }
    }
    
    void GrabSkeleton(SkinnedModel& skinnedModel, const aiNode* pNode, int parentIndex) {
        Joint joint;
        joint.m_name = Util::CopyConstChar(pNode->mName.C_Str());
        joint.m_inverseBindTransform = Util::aiMatrix4x4ToGlm(pNode->mTransformation);
        joint.m_parentIndex = parentIndex;
        parentIndex = (int)skinnedModel.m_joints.size(); // don't do your head in with why this works, just be thankful it does..well its actually pretty clear. if u look below
        skinnedModel.m_joints.push_back(joint);
        for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
            GrabSkeleton(skinnedModel, pNode->mChildren[i], parentIndex);
        }
    }

    void AssetManager::LoadSkinnedModel(SkinnedModel* skinnedModel) {

        static std::mutex mutex;
        const FileInfo& fileInfo = skinnedModel->GetFileInfo();
        int totalVertexCount = 0;
        int baseVertexLocal = 0;
        int boneCount = 0;

        skinnedModel->m_NumBones = 0;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(fileInfo.path, aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene) {
            std::cout << "Something fucked up loading your skinned model: " << fileInfo.path << "\n";
            std::cout << "Error: " << importer.GetErrorString() << "\n";
            return;
        }

        // Load bones
        glm::mat4 globalInverseTransform = glm::inverse(Util::aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));
        for (int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* assimpMesh = scene->mMeshes[i];
            for (unsigned int j = 0; j < assimpMesh->mNumBones; j++) {
                unsigned int boneIndex = 0;
                std::string boneName = (assimpMesh->mBones[j]->mName.data);
                // Created bone if it doesn't exist yet
                if (skinnedModel->m_BoneMapping.find(boneName) == skinnedModel->m_BoneMapping.end()) {
                    // Allocate an index for a new bone
                    boneIndex = skinnedModel->m_NumBones;
                    skinnedModel->m_NumBones++;
                    BoneInfo bi;
                    skinnedModel->m_BoneInfo.push_back(bi);
                    skinnedModel->m_BoneInfo[boneIndex].BoneOffset = Util::aiMatrix4x4ToGlm(assimpMesh->mBones[j]->mOffsetMatrix);
                    skinnedModel->m_BoneInfo[boneIndex].BoneName = boneName;
                    skinnedModel->m_BoneMapping[boneName] = boneIndex;
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
            for (unsigned int i = 0; i < assimpMesh->mNumBones; i++) {
                for (unsigned int j = 0; j < assimpMesh->mBones[i]->mNumWeights; j++) {
                    std::string boneName = assimpMesh->mBones[i]->mName.data;
                    unsigned int boneIndex = skinnedModel->m_BoneMapping[boneName];
                    unsigned int vertexIndex = assimpMesh->mBones[i]->mWeights[j].mVertexId;
                    float weight = assimpMesh->mBones[i]->mWeights[j].mWeight;
                    WeightedVertex& vertex = vertices[vertexIndex];

                    if (vertex.weight.x == 0) {
                        vertex.boneID.x = boneIndex;
                        vertex.weight.x = weight;
                    }
                    else if (vertex.weight.y == 0) {
                        vertex.boneID.y = boneIndex;
                        vertex.weight.y = weight;
                    }
                    else if (vertex.weight.z == 0) {
                        vertex.boneID.z = boneIndex;
                        vertex.weight.z = weight;
                    }
                    else if (vertex.weight.w == 0) {
                        vertex.boneID.w = boneIndex;
                        vertex.weight.w = weight;
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
            std::lock_guard<std::mutex> lock(mutex);
            skinnedModel->AddMeshIndex(AssetManager::CreateSkinnedMesh(meshName, vertices, indices, baseVertexLocal, aabbMin, aabbMax));
            totalVertexCount += vertices.size();
            baseVertexLocal += vertices.size();
        }
        skinnedModel->m_GlobalInverseTransform = globalInverseTransform;
        skinnedModel->m_vertexCount = totalVertexCount;
        GrabSkeleton(*skinnedModel, scene->mRootNode, -1);

        for (auto& joint : skinnedModel->m_joints) {
            if (skinnedModel->m_BoneMapping.find(joint.m_name) != skinnedModel->m_BoneMapping.end()) {
                unsigned int boneIndex = skinnedModel->m_BoneMapping[joint.m_name];
                joint.m_boneOffset = skinnedModel->m_BoneInfo[boneIndex].BoneOffset;
            }
        }

        importer.FreeScene();

        skinnedModel->SetLoadingState(LoadingState::LOADING_COMPLETE);

        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "Loaded " << skinnedModel->GetFileInfo().name << "\n";
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