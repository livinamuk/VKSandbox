#include "File.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <chrono>
#include "../Util/Util.h"

#define PRINT_MODEL_HEADERS_ON_READ 0
#define PRINT_MODEL_HEADERS_ON_WRITE 0
#define PRINT_MESH_HEADERS_ON_READ 0
#define PRINT_MESH_HEADERS_ON_WRITE 0
#define PRINT_SKINNED_MODEL_HEADERS_ON_READ 0
#define PRINT_SKINNED_MODEL_HEADERS_ON_WRITE 0
#define PRINT_SKINNED_MESH_HEADERS_ON_READ 0
#define PRINT_SKINNED_MESH_HEADERS_ON_WRITE 0

/*
█▄ ▄█ █▀█ █▀▄ █▀▀ █   █▀▀
█ █ █ █ █ █ █ █▀▀ █   ▀▀█
▀   ▀ ▀▀▀ ▀▀  ▀▀▀ ▀▀▀ ▀▀▀ */

void File::ExportModel(const ModelData& modelData) {
    std::string outputPath = "res/models/" + modelData.name + ".model";
    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file for writing: " << outputPath << "\n";
        return;
    }

    // Fill the header
    ModelHeader modelHeader;
    modelHeader.version = 1;
    modelHeader.meshCount = modelData.meshCount;
    modelHeader.nameLength = modelData.name.size();
    modelHeader.timestamp = modelData.timestamp;
    modelHeader.aabbMin = modelData.aabbMin;
    modelHeader.aabbMax = modelData.aabbMax;
    std::memcpy(modelHeader.signature, "HELL_MODEL", 10);

    // Write the header
    file.write(modelHeader.signature, 10);
    file.write((char*)&modelHeader.version, sizeof(modelHeader.version));
    file.write((char*)&modelHeader.meshCount, sizeof(modelHeader.meshCount));
    file.write((char*)&modelHeader.nameLength, sizeof(modelHeader.nameLength));
    file.write(reinterpret_cast<char*>(&modelHeader.timestamp), sizeof(modelHeader.timestamp));
    file.write(reinterpret_cast<const char*>(&modelHeader.aabbMin), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&modelHeader.aabbMax), sizeof(glm::vec3));
#if PRINT_MODEL_HEADERS_ON_WRITE
    PrintModelHeader(modelHeader, "Wrote model header: " + outputPath);
#endif

    // Write the name
    file.write(modelData.name.data(), modelHeader.nameLength);

    // Write the mesh data
    for (const MeshData& meshData : modelData.meshes) {
        MeshHeader meshHeader;
        meshHeader.nameLength = (uint32_t)meshData.name.size();
        meshHeader.vertexCount = (uint32_t)meshData.vertices.size();
        meshHeader.indexCount = (uint32_t)meshData.indices.size();
        meshHeader.aabbMin = meshData.aabbMin;
        meshHeader.aabbMax = meshData.aabbMax;
        file.write((char*)&meshHeader.nameLength, sizeof(meshHeader.nameLength));
        file.write((char*)&meshHeader.vertexCount, sizeof(meshHeader.vertexCount));
        file.write((char*)&meshHeader.indexCount, sizeof(meshHeader.indexCount));
        file.write(meshData.name.data(), meshHeader.nameLength);
        file.write(reinterpret_cast<const char*>(&meshHeader.aabbMin), sizeof(glm::vec3));
        file.write(reinterpret_cast<const char*>(&meshHeader.aabbMax), sizeof(glm::vec3));
        file.write(reinterpret_cast<const char*>(meshData.vertices.data()), meshData.vertices.size() * sizeof(Vertex));
        file.write(reinterpret_cast<const char*>(meshData.indices.data()), meshData.indices.size() * sizeof(uint32_t));
#if PRINT_MESH_HEADERS_ON_WRITE
        PrintMeshHeader(meshHeader, "Wrote mesh: " + meshData.name);
#endif
    }
    file.close();
    std::cout << "Exported: " << outputPath << "\n";
}

ModelHeader File::ReadModelHeader(const std::string& filepath) {
    ModelHeader header{};
    std::ifstream file(filepath, std::ios::binary);

    // Check for invalid file
    if (!file.is_open()) {
        return header;
    }

    // Check for valid signature
    char fileSignature[10];
    file.read(fileSignature, 10);
    if (std::strncmp(fileSignature, "HELL_MODEL", 10) != 0) {
        return header;
    }

    // Read the rest of header
    file.read(reinterpret_cast<char*>(&header.version), sizeof(header.version));
    file.read(reinterpret_cast<char*>(&header.meshCount), sizeof(header.meshCount));
    file.read(reinterpret_cast<char*>(&header.nameLength), sizeof(header.nameLength));
    file.read(reinterpret_cast<char*>(&header.timestamp), sizeof(header.timestamp));
    file.read(reinterpret_cast<char*>(&header.aabbMin), sizeof(header.aabbMin));
    file.read(reinterpret_cast<char*>(&header.aabbMax), sizeof(header.aabbMax));
    return header;
}

ModelData File::ImportModel(const std::string& filepath) {
    ModelData modelData;
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filepath << "\n";
        return modelData;
    }
    ModelHeader modelHeader;

    // Read 10 bytes for HELL_MODEL
    char fileSignature[10];       
    file.read(fileSignature, 10); 

    // Read the rest of the header
    file.read((char*)&modelHeader.version, sizeof(modelHeader.version));
    file.read((char*)&modelHeader.meshCount, sizeof(modelHeader.meshCount));
    file.read((char*)&modelHeader.nameLength, sizeof(modelHeader.nameLength));
    file.read(reinterpret_cast<char*>(&modelHeader.timestamp), sizeof(modelHeader.timestamp));
    file.read(reinterpret_cast<char*>(&modelHeader.aabbMin), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&modelHeader.aabbMax), sizeof(glm::vec3));
#if PRINT_MODEL_HEADERS_ON_READ
    PrintModelHeader(modelHeader, "Read model header: '" + filepath + "'");
#endif
    // Read the name
    std::string modelName(modelHeader.nameLength, '\0');
    file.read(&modelName[0], modelHeader.nameLength);

    // Store header data
    modelData.meshCount = modelHeader.meshCount;
    modelData.meshes.resize(modelHeader.meshCount);
    modelData.name = modelName;
    modelData.aabbMin = modelHeader.aabbMin;
    modelData.aabbMax = modelHeader.aabbMax;
    modelData.timestamp = modelHeader.timestamp;

    // Load each mesh
    for (uint32_t i = 0; i < modelHeader.meshCount; ++i) {
        MeshHeader meshHeader;
        file.read((char*)&meshHeader.nameLength, sizeof(meshHeader.nameLength));
        file.read((char*)&meshHeader.vertexCount, sizeof(meshHeader.vertexCount));
        file.read((char*)&meshHeader.indexCount, sizeof(meshHeader.indexCount));
        std::string meshName(meshHeader.nameLength, '\0');
        file.read(&meshName[0], meshHeader.nameLength);
        file.read(reinterpret_cast<char*>(&meshHeader.aabbMin), sizeof(glm::vec3));
        file.read(reinterpret_cast<char*>(&meshHeader.aabbMax), sizeof(glm::vec3));
        MeshData& meshData = modelData.meshes[i];
        meshData.name = meshName;
        meshData.vertexCount = meshHeader.vertexCount;
        meshData.indexCount = meshHeader.indexCount;
        meshData.vertices.resize(meshData.vertexCount);
        meshData.indices.resize(meshData.indexCount);
        meshData.aabbMin = meshHeader.aabbMin;
        meshData.aabbMax = meshHeader.aabbMax;
        file.read(reinterpret_cast<char*>(meshData.vertices.data()), meshHeader.vertexCount * sizeof(Vertex));
        file.read(reinterpret_cast<char*>(meshData.indices.data()), meshHeader.indexCount * sizeof(uint32_t));
#if PRINT_MESH_HEADERS_ON_READ
        PrintMeshHeader(meshHeader, "Read mesh: " + meshData.name);
#endif
    }
    file.close();
    return modelData;
}

void File::ExportSkinnedModel(const SkinnedModelData& skinnedModelData) {
    std::string outputPath = "res/skinned_models/" + skinnedModelData.name + ".skinnedmodel";
    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file for writing: " << outputPath << "\n";
        return;
    }

    // Populate header
    SkinnedModelHeader modelHeader;
    modelHeader.version = 1;
    modelHeader.meshCount = skinnedModelData.GetMeshCount();
    modelHeader.nodeCount = skinnedModelData.GetNodeCount();
    modelHeader.boneCount = skinnedModelData.GetBoneCount();
    modelHeader.vertexCount = skinnedModelData.vertexCount;
    modelHeader.indexCount = skinnedModelData.indexCount;
    modelHeader.nameLength = skinnedModelData.name.size();
    modelHeader.timestamp = skinnedModelData.timestamp;
    std::memcpy(modelHeader.signature, "HELL_SKINNED_MODEL", 18);

    // Write header
    file.write(modelHeader.signature, 18);
    file.write((char*)&modelHeader.version, sizeof(modelHeader.version));
    file.write((char*)&modelHeader.nameLength, sizeof(modelHeader.nameLength));
    file.write((char*)&modelHeader.vertexCount, sizeof(modelHeader.vertexCount));
    file.write((char*)&modelHeader.indexCount, sizeof(modelHeader.indexCount));
    file.write((char*)&modelHeader.meshCount, sizeof(modelHeader.meshCount));
    file.write((char*)&modelHeader.nodeCount, sizeof(modelHeader.nodeCount));
    file.write((char*)&modelHeader.boneCount, sizeof(modelHeader.boneCount));
    file.write(reinterpret_cast<char*>(&modelHeader.timestamp), sizeof(modelHeader.timestamp));

    // Write model name
    file.write(skinnedModelData.name.data(), modelHeader.nameLength);

    // Write nodes
    for (const Node& node : skinnedModelData.nodes) {
        uint32_t nodeNameLength = static_cast<uint32_t>(node.name.length());
        file.write(reinterpret_cast<const char*>(&nodeNameLength), sizeof(nodeNameLength));
        file.write(node.name.c_str(), nodeNameLength);
        file.write(reinterpret_cast<const char*>(&node.parentIndex), sizeof(node.parentIndex));
        file.write(reinterpret_cast<const char*>(&node.inverseBindTransform), sizeof(node.inverseBindTransform));
    }

    // Write the bone offsets
    for (const glm::mat4& boneOffset : skinnedModelData.boneOffsets) {;
        file.write(reinterpret_cast<const char*>(&boneOffset), sizeof(boneOffset));
    }

    // Write the bone mapping
    for (const auto& pair : skinnedModelData.boneMapping) {
        const std::string& boneName = pair.first;
        uint32_t nameLength = static_cast<uint32_t>(boneName.size());
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(boneName.data(), nameLength);
        file.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }

    #if PRINT_SKINNED_MODEL_HEADERS_ON_WRITE
    PrintSkinnedModelHeader(modelHeader, "Wrote skinned model header: " + skinnedModelData.name);
    #endif

    std::string name;
    std::vector<WeightedVertex> vertices;
    std::vector<uint32_t> indices;
    glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t localBaseVertex;

    // Write the mesh data
    for (const SkinnedMeshData& skinnedMeshData : skinnedModelData.meshes) {
        SkinnedMeshHeader skinnedMeshHeader;
        skinnedMeshHeader.nameLength = (uint32_t)skinnedMeshData.name.size();
        skinnedMeshHeader.vertexCount = (uint32_t)skinnedMeshData.vertices.size();
        skinnedMeshHeader.indexCount = (uint32_t)skinnedMeshData.indices.size();
        skinnedMeshHeader.localBaseVertex = skinnedMeshData.localBaseVertex;
        skinnedMeshHeader.aabbMin = skinnedMeshData.aabbMin;
        skinnedMeshHeader.aabbMax = skinnedMeshData.aabbMax;

        file.write((char*)&skinnedMeshHeader.nameLength, sizeof(skinnedMeshHeader.nameLength));
        file.write((char*)&skinnedMeshHeader.vertexCount, sizeof(skinnedMeshHeader.vertexCount));
        file.write((char*)&skinnedMeshHeader.indexCount, sizeof(skinnedMeshHeader.indexCount));
        file.write((char*)&skinnedMeshHeader.localBaseVertex, sizeof(skinnedMeshHeader.localBaseVertex));
        file.write(skinnedMeshData.name.data(), skinnedMeshHeader.nameLength);
        file.write(reinterpret_cast<const char*>(&skinnedMeshHeader.aabbMin), sizeof(glm::vec3));
        file.write(reinterpret_cast<const char*>(&skinnedMeshHeader.aabbMax), sizeof(glm::vec3));
        file.write(reinterpret_cast<const char*>(skinnedMeshData.vertices.data()), skinnedMeshData.vertices.size() * sizeof(WeightedVertex));
        file.write(reinterpret_cast<const char*>(skinnedMeshData.indices.data()), skinnedMeshData.indices.size() * sizeof(uint32_t));
        #if PRINT_SKINNED_MESH_HEADERS_ON_WRITE
            PrintSkinnedMeshHeader(skinnedMeshHeader, "Wrote skinned mesh: " + skinnedMeshData.name);
        #endif
    }
    file.close();
    std::cout << "Exported: " << outputPath << "\n";
}

SkinnedModelData File::ImportSkinnedModel(const std::string& filepath) {
    SkinnedModelData skinnedModelData;

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "ile::ImportSkinnedModel() failed: Could not open: " << filepath << "\n";
        return skinnedModelData;
    }
    
    SkinnedModelHeader skinnedModelHeader;

    // Read 18 bytes for HELL_SKINNED_MODEL
    char fileSignature[18];
    file.read(fileSignature, 18);

    // Read the rest of the header
    file.read((char*)&skinnedModelHeader.version, sizeof(skinnedModelHeader.version));
    file.read((char*)&skinnedModelHeader.nameLength, sizeof(skinnedModelHeader.nameLength));
    file.read((char*)&skinnedModelHeader.vertexCount, sizeof(skinnedModelHeader.vertexCount));
    file.read((char*)&skinnedModelHeader.indexCount, sizeof(skinnedModelHeader.indexCount));
    file.read((char*)&skinnedModelHeader.meshCount, sizeof(skinnedModelHeader.meshCount));
    file.read((char*)&skinnedModelHeader.nodeCount, sizeof(skinnedModelHeader.nodeCount));
    file.read((char*)&skinnedModelHeader.boneCount, sizeof(skinnedModelHeader.boneCount));
    file.read(reinterpret_cast<char*>(&skinnedModelHeader.timestamp), sizeof(skinnedModelHeader.timestamp));

    // Read the name
    std::string modelName(skinnedModelHeader.nameLength, '\0');
    file.read(&modelName[0], skinnedModelHeader.nameLength);

    #if PRINT_SKINNED_MODEL_HEADERS_ON_READ
    PrintSkinnedModelHeader(skinnedModelHeader, "Read model header: '" + modelName + "'");
    #endif

    // Store header data
    skinnedModelData.name = modelName;
    skinnedModelData.vertexCount = skinnedModelHeader.vertexCount;
    skinnedModelData.indexCount = skinnedModelHeader.indexCount;
    skinnedModelData.meshes.resize(skinnedModelHeader.meshCount);
    skinnedModelData.nodes.resize(skinnedModelHeader.nodeCount);
    skinnedModelData.boneOffsets.resize(skinnedModelHeader.boneCount);
    skinnedModelData.timestamp = skinnedModelHeader.timestamp;

    // Read the nodes
    for (uint32_t i = 0; i < skinnedModelHeader.nodeCount; i++) {
        uint32_t nodeNameLength = 0;
        file.read(reinterpret_cast<char*>(&nodeNameLength), sizeof(nodeNameLength));

        // Read node name
        std::string nodeName;
        nodeName.resize(nodeNameLength);
        file.read(&nodeName[0], nodeNameLength);

        // Read parent index
        int parentIndex = 0;
        file.read(reinterpret_cast<char*>(&parentIndex), sizeof(parentIndex));

        // Read the inverse bind transform
        glm::mat4 inverseBindTransform;
        file.read(reinterpret_cast<char*>(&inverseBindTransform), sizeof(inverseBindTransform));

        Node node;
        node.name = nodeName;
        node.parentIndex = parentIndex;
        node.inverseBindTransform = inverseBindTransform;
        skinnedModelData.nodes[i] = node;

        //std::cout << i << " " << nodeName << "\n";
        //std::cout << "parent Index: " << parentIndex << "\n";
        //std::cout << Util::Mat4ToString(inverseBindTransform) << "\n\n";
    }

    // Read the bone offsets
    for (uint32_t i = 0; i < skinnedModelHeader.boneCount; i++) {
        file.read(reinterpret_cast<char*>(&skinnedModelData.boneOffsets[i]), sizeof(glm::mat4));

        //std::cout << i << "\n" << Util::Mat4ToString(skinnedModelData.boneOffsets[i]) << "\n\n";
    }

    // Read the bone mapping
    for (uint32_t i = 0; i < skinnedModelHeader.boneCount; i++) {
        uint32_t nameLength = 0;
        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        // Read the bone name into a string
        std::string boneName;
        boneName.resize(nameLength);
        file.read(&boneName[0], nameLength);

        // Read the corresponding bone index
        unsigned int boneIndex = 0;
        file.read(reinterpret_cast<char*>(&boneIndex), sizeof(boneIndex));

        // Insert into the bone mapping
        skinnedModelData.boneMapping[boneName] = boneIndex;

        //std::cout << i << " " << boneName << ": " << boneIndex << "\n";
    }

    for (uint32_t i = 0; i < skinnedModelHeader.meshCount; i++) {
        // Read the header values for this mesh.
        uint32_t nameLength = 0;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        uint32_t localBaseVertex = 0;

        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        file.read(reinterpret_cast<char*>(&localBaseVertex), sizeof(localBaseVertex));

        // Read the mesh name.
        std::string meshName(nameLength, '\0');
        file.read(&meshName[0], nameLength);

        // Read the AABB values.
        glm::vec3 aabbMin;
        glm::vec3 aabbMax;
        file.read(reinterpret_cast<char*>(&aabbMin), sizeof(glm::vec3));
        file.read(reinterpret_cast<char*>(&aabbMax), sizeof(glm::vec3));

        // Now set up the mesh data.
        SkinnedMeshData& meshData = skinnedModelData.meshes[i];
        meshData.name = meshName;
        meshData.vertexCount = vertexCount;
        meshData.indexCount = indexCount;
        meshData.localBaseVertex = localBaseVertex;
        meshData.aabbMin = aabbMin;
        meshData.aabbMax = aabbMax;

        // Allocate space for vertices and indices.
        meshData.vertices.resize(vertexCount);
        meshData.indices.resize(indexCount);

        // Read the vertices and indices.
        file.read(reinterpret_cast<char*>(meshData.vertices.data()), vertexCount * sizeof(WeightedVertex));
        file.read(reinterpret_cast<char*>(meshData.indices.data()), indexCount * sizeof(uint32_t));


        //std::cout << i << ": " << meshData.name << " ";
        //std::cout << meshData.vertices.size() << " verts, ";
        //std::cout << meshData.indices.size() << " indices, ";
        //std::cout << Util::Vec3ToString(meshData.aabbMin) << " ";
        //std::cout << Util::Vec3ToString(meshData.aabbMax) << "\n";

        //#if PRINT_SKINNED_MESH_HEADERS_ON_READ
        //    PrintSkinnedMeshHeader(meshData, "Read skinned mesh: " + meshData.name);
        //#endif
    }



    return skinnedModelData;
}

SkinnedModelHeader File::ReadSkinnedModelHeader(const std::string& filepath) {
    SkinnedModelHeader header{};
    std::ifstream file(filepath, std::ios::binary);

    // Check for invalid file
    if (!file.is_open()) {
        std::cout << "File::ReadSkinnedModelHeader() failed: could not open " << filepath << "\n";
        return header;
    }

    // Check for valid signature
    char fileSignature[18];
    file.read(fileSignature, 18);
    if (std::strncmp(fileSignature, "HELL_SKINNED_MODEL", 18) != 0) {
        std::cout << "File::ReadSkinnedModelHeader() failed: 'HELL_SKINNED_MODEL' signature not found in header\n";
        return header;
    }

    // Read the rest of header
    file.read(reinterpret_cast<char*>(&header.version), sizeof(header.version));
    file.read(reinterpret_cast<char*>(&header.nameLength), sizeof(header.nameLength));
    file.read(reinterpret_cast<char*>(&header.vertexCount), sizeof(header.vertexCount));
    file.read(reinterpret_cast<char*>(&header.indexCount), sizeof(header.indexCount));
    file.read(reinterpret_cast<char*>(&header.meshCount), sizeof(header.meshCount));
    file.read(reinterpret_cast<char*>(&header.nodeCount), sizeof(header.nodeCount));
    file.read(reinterpret_cast<char*>(&header.boneCount), sizeof(header.boneCount));
    file.read(reinterpret_cast<char*>(&header.timestamp), sizeof(header.timestamp));
    return header;
}

/*
 ▀█▀   █ █▀█
  █  ▄▀  █ █
 ▀▀▀ ▀   ▀▀▀ */

void File::DeleteFile(const std::string& filepath) {
    try {
        if (std::filesystem::remove(filepath)) {
            // File deleted successfully
        }
        else {
            std::cout << "File::DeleteFile() failed to delete '" << filepath << "', file does not exist or could not be deleted!\n";
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "File::DeleteFile() exception: " << e.what() << "\n";
    }
}

/*
 ▀▀█▀▀ █ █▀▄▀█ █▀▀ █▀▀ ▀█▀ █▀▀█ █▀▄▀█ █▀▀█ █▀▀
   █   █ █ ▀ █ █▀▀ ▀▀█  █  █▄▄█ █ ▀ █ █▄▄█ ▀▀█
   ▀   ▀ ▀   ▀ ▀▀▀ ▀▀▀  ▀  ▀  ▀ ▀   ▀ ▀    ▀▀▀ */

uint64_t File::GetCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::string File::TimestampToString(uint64_t timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm timeStruct{};
    if (localtime_s(&timeStruct, &time) != 0) {
        return "Invalid Timestamp";
    }
    char buffer[26];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeStruct)) {
        return std::string(buffer);
    }
    return "Invalid Timestamp";
}

uint64_t File::GetLastModifiedTime(const std::string& filePath) {
    try {
        auto ftime = std::filesystem::last_write_time(filePath);
        auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
        return std::chrono::duration_cast<std::chrono::seconds>(systemTime.time_since_epoch()).count();
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error reading file timestamp: " << e.what() << "\n";
        return 0;
    }
}

/*
█▀▄ █▀▀ █▀▄ █ █ █▀▀
█ █ █▀▀ █▀▄ █ █ █ █
▀▀  ▀▀▀ ▀▀  ▀▀▀ ▀▀▀ */

void File::PrintModelHeader(ModelHeader header, const std::string& identifier) {
    std::cout << identifier << "\n";
    std::cout << " Version: " << header.version << "\n";
    std::cout << " Mesh Count: " << header.meshCount << "\n";
    std::cout << " Name Length: " << header.nameLength << "\n";
    std::cout << " Timestamp: " << header.timestamp << "\n\n";
    std::cout << " AABB min: " << Util::Vec3ToString(header.aabbMin) << "\n";
    std::cout << " AABB max: " << Util::Vec3ToString(header.aabbMax) << "\n\n";
}

void File::PrintMeshHeader(MeshHeader header, const std::string& identifier) {
    std::cout << identifier << "\n";
    std::cout << " Name Length: " << header.nameLength << "\n";
    std::cout << " Vertex Count: " << header.vertexCount << "\n";
    std::cout << " Index Count: " << header.indexCount << "\n";
    std::cout << " AABB min: " << Util::Vec3ToString(header.aabbMin) << "\n";
    std::cout << " AABB max: " << Util::Vec3ToString(header.aabbMax) << "\n\n";
}

void File::PrintSkinnedModelHeader(SkinnedModelHeader header, const std::string& identifier) {
    std::cout << identifier << "\n";
    std::cout << " Version: " << header.version << "\n";
    std::cout << " Name Length: " << header.nameLength << "\n";
    std::cout << " Vertex Count: " << header.vertexCount << "\n";
    std::cout << " Index Count: " << header.indexCount << "\n";
    std::cout << " Mesh Count: " << header.meshCount << "\n";
    std::cout << " Mesh Node Count: " << header.nodeCount << "\n";
    std::cout << " Mesh Bone Count: " << header.boneCount << "\n";
    std::cout << " Timestamp: " << header.timestamp << "\n\n";
}

void File::PrintSkinnedMeshHeader(SkinnedMeshHeader header, const std::string& identifier) {
    std::cout << identifier << "\n";
    std::cout << " Name Length: " << header.nameLength << "\n";
    std::cout << " Vertex Count: " << header.vertexCount << "\n";
    std::cout << " Index Count: " << header.indexCount << "\n";
    std::cout << " Local Base Vertex: " << header.localBaseVertex << "\n";
    std::cout << " AABB min: " << Util::Vec3ToString(header.aabbMin) << "\n";
    std::cout << " AABB max: " << Util::Vec3ToString(header.aabbMax) << "\n\n";
}


void File::ExportMeshDataToOBJ(const std::string& filepath, const MeshData& mesh) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << "\n";
        return;
    }
    for (const auto& vertex : mesh.vertices) {
        file << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
    }
    for (const auto& vertex : mesh.vertices) {
        file << "vn " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << "\n";
    }
    for (const auto& vertex : mesh.vertices) {
        file << "vt " << vertex.uv.x << " " << vertex.uv.y << "\n";
    }
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        file << "f "
            << mesh.indices[i] + 1 << "/" << mesh.indices[i] + 1 << "/" << mesh.indices[i] + 1 << " "
            << mesh.indices[i + 1] + 1 << "/" << mesh.indices[i + 1] + 1 << "/" << mesh.indices[i + 1] + 1 << " "
            << mesh.indices[i + 2] + 1 << "/" << mesh.indices[i + 2] + 1 << "/" << mesh.indices[i + 2] + 1 << "\n";
    }
    file.close();
    std::cout << "Exported OBJ: " << filepath << "\n";
}

void File::ExportSkinnedMeshDataToOBJ(const std::string& filepath, const SkinnedMeshData& mesh) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << "\n";
        return;
    }
    for (const auto& vertex : mesh.vertices) {
        file << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
    }
    for (const auto& vertex : mesh.vertices) {
        file << "vn " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << "\n";
    }
    for (const auto& vertex : mesh.vertices) {
        file << "vt " << vertex.uv.x << " " << vertex.uv.y << "\n";
    }
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        file << "f "
            << mesh.indices[i] + 1 << "/" << mesh.indices[i] + 1 << "/" << mesh.indices[i] + 1 << " "
            << mesh.indices[i + 1] + 1 << "/" << mesh.indices[i + 1] + 1 << "/" << mesh.indices[i + 1] + 1 << " "
            << mesh.indices[i + 2] + 1 << "/" << mesh.indices[i + 2] + 1 << "/" << mesh.indices[i + 2] + 1 << "\n";
    }
    file.close();
    std::cout << "Exported OBJ: " << filepath << "\n";
}