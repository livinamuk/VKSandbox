#include "AssetManager.h"
#include "Util/Util.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <future>

namespace AssetManager {

    void LoadPendingAnimationsAsync() {
        for (Animation& animation : GetAnimations()) {
            if (animation.GetLoadingState() == LoadingState::AWAITING_LOADING_FROM_DISK) {
                animation.SetLoadingState(LoadingState::LOADING_FROM_DISK);
                AddItemToLoadLog(animation.GetFileInfo().path);
                //std::async(std::launch::async, LoadAnimation, &animation);
                LoadAnimation(&animation);
                //return;
            }
        }
    }

    void LoadAnimation(Animation* animation) {
        const FileInfo& fileInfo = animation->GetFileInfo();

        aiScene* m_pAnimationScene;
        Assimp::Importer m_AnimationImporter;

        // Try and load the animation
        const aiScene* tempAnimScene = m_AnimationImporter.ReadFile(fileInfo.path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        // Failed
        if (!tempAnimScene) {
            std::cout << "Could not load: " << fileInfo.path << "\n";
        }

        // Success
        m_pAnimationScene = new aiScene(*tempAnimScene);
        if (m_pAnimationScene) {
            animation->m_duration = (float)m_pAnimationScene->mAnimations[0]->mDuration;
            animation->m_ticksPerSecond = (float)m_pAnimationScene->mAnimations[0]->mTicksPerSecond;
            //std::cout << "Loaded animation: " << Filename << "\n";
        }
        // Some other error possibility
        else {
            std::cout << "Error parsing " << fileInfo.path << ": " << m_AnimationImporter.GetErrorString();
        }

        // need to create an animation clip.
        // need to fill it with animation poses.
        aiAnimation* aiAnim = m_pAnimationScene->mAnimations[0];

        // Resize the vector big enough for each pose
        int nodeCount = aiAnim->mNumChannels;
         // trying the assimp way now. coz why fight it.
        for (int n = 0; n < nodeCount; n++)
        {
            const char* nodeName = Util::CopyConstChar(aiAnim->mChannels[n]->mNodeName.C_Str());

            AnimatedNode animatedNode(nodeName);
            animation->m_NodeMapping.emplace(nodeName, n);

            for (unsigned int p = 0; p < aiAnim->mChannels[n]->mNumPositionKeys; p++)
            {
                SQT sqt;
                aiVectorKey pos = aiAnim->mChannels[n]->mPositionKeys[p];
                aiQuatKey rot = aiAnim->mChannels[n]->mRotationKeys[p];
                aiVectorKey scale = aiAnim->mChannels[n]->mScalingKeys[p];

                sqt.positon = glm::vec3(pos.mValue.x, pos.mValue.y, pos.mValue.z);
                sqt.rotation = glm::quat(rot.mValue.w, rot.mValue.x, rot.mValue.y, rot.mValue.z);
                sqt.scale = glm::vec3(scale.mValue.x, scale.mValue.y, scale.mValue.z);
                sqt.timeStamp = (float)aiAnim->mChannels[n]->mPositionKeys[p].mTime;

                animation->m_finalTimeStamp = std::max(animation->m_finalTimeStamp, sqt.timeStamp);

                animatedNode.m_nodeKeys.push_back(sqt);
            }
            animation->m_animatedNodes.push_back(animatedNode);
        }
        // Store it
        m_AnimationImporter.FreeScene();

        animation->SetLoadingState(LoadingState::LOADING_FROM_DISK);
    }

    Animation* AssetManager::GetAnimationByName(const std::string& name) {
        std::vector<Animation>& animations = GetAnimations();
        for (auto& animation : animations) {
            if (name == animation.GetName()) {
                return &animation;
            }
        }
        std::cout << "AssetManager::GetAnimationByName(const std::string& name) failed because '" << name << "' does not exist!\n";
        return nullptr;
    }

    Animation* AssetManager::GetAnimationByIndex(int index, bool printError) {
        std::vector<Animation>& animations = GetAnimations();
        if (index >= 0 && index < animations.size()) {
            return &animations[index];
        }
        else {
            if (printError) {
                std::cout << "AssetManager::GetAnimationByIndex(int index) failed because index '" << index << "' is out of range. Size is " << animations.size() << "!\n";
            }
            return nullptr;
        }
    }

    int AssetManager::GetAnimationIndexByName(const std::string& name) {
        std::vector<Animation>& animations = GetAnimations();
        for (int i = 0; i < animations.size(); i++) {
            if (name == animations[i].GetName()) {
                return i;
            }
        }
        std::cout << "AssetManager::GetAnimationIndexByName(const std::string& name) failed because '" << name << "' does not exist!\n";
        return -1;
    }
}