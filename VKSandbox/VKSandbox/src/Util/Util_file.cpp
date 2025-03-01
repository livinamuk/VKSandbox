#include "Util.h"
#include <fstream>
#include <string>

namespace Util {  
    int GetFileSize(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file) {
            return -1; 
        }
        return static_cast<int>(file.tellg());
    }

    bool FileExists(const std::string_view name) {
        struct stat buffer;
        return (stat(name.data(), &buffer) == 0);
    }

    bool RenameFile(const std::string& oldFilePath, const std::string& newFilePath) {
        if (!FileExists(oldFilePath)) {
            std::cout << "Util::RenameFile() failed because old path '" << oldFilePath << "' does not exist!\n";
            return false;
        }
        std::filesystem::path oldPath = oldFilePath;
        std::filesystem::path newPath = newFilePath;
        return std::filesystem::rename(oldPath, newPath), true;
    }

    std::string GetFileName(const std::string& filepath) {
        size_t pos = filepath.find_last_of("/\\");
        std::string filename = (pos == std::string::npos) ? filepath : filepath.substr(pos + 1);
        pos = filename.find_last_of('.');
        return (pos != std::string::npos) ? filename.substr(0, pos) : filename;
    }

    std::string RemoveFileExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        return (pos != std::string::npos) ? filename.substr(0, pos) : filename;
    }

    std::string GetFullPath(const std::filesystem::directory_entry& entry) {
        return entry.path().string();
    }

    std::string GetFileName(const std::filesystem::directory_entry& entry) {
        return entry.path().filename().string();
    }

    std::string GetFileNameWithoutExtension(const std::filesystem::directory_entry& entry) {
        return entry.path().stem().string();
    }

    std::string GetFileExtension(const std::filesystem::directory_entry& entry) {
        return entry.path().extension().string().substr(1);
    }

    std::vector<FileInfo> IterateDirectory(const std::string& directory, std::vector<std::string> extensions) {
        std::vector<FileInfo> fileInfoList;       
        if (!std::filesystem::exists(directory)) return fileInfoList;
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!std::filesystem::is_regular_file(entry)) continue;

            FileInfo fileInfo = { GetFullPath(entry), GetFileNameWithoutExtension(entry), GetFileExtension(entry), directory };

            if (extensions.empty() || std::find(extensions.begin(), extensions.end(), fileInfo.ext) != extensions.end()) {
                fileInfoList.push_back(fileInfo);
            }
        }
        return fileInfoList;
    }

    std::string GetFilename(const std::string& filepath) {
        std::string result = filepath.substr(filepath.rfind("/") + 1);
        return result.substr(0, result.length() - 4);
    }

    FileInfo GetFileInfoFromPath(const std::string& filepath) {
        FileInfo fileInfo;
        std::filesystem::path path(filepath);
        if (!FileExists(filepath)) {
            std::cout << "Util::GetFileInfoFromPath() Failed: path does not exist: " << filepath << ", warning returning an empty FileInfo\n";
            return fileInfo;
        }
        fileInfo.path = path.string();
        fileInfo.name = path.stem().string();
        fileInfo.ext = path.has_extension() ? path.extension().string().substr(1) : "";
        fileInfo.dir = path.parent_path().string();
        return fileInfo;
    }

}