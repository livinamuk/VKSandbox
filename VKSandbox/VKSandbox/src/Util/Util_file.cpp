#include "Util.h"

namespace Util {
    FileInfoOld GetFileInfo(std::string filepath) {
        std::string filename = GetFilename(filepath);
        std::string filetype = filepath.substr(filepath.length() - 3);
        std::string directory = filepath.substr(0, filepath.rfind("/") + 1);
        std::string materialType = "NONE";

        if (filename.length() > 5) {
            std::string query = filename.substr(filename.length() - 3);
            if (query == "ALB" || query == "RMA" || query == "NRM")
                materialType = query;
        }

        return { filepath, directory, filename, filetype, materialType };
    }

    FileInfoOld GetFileInfo(const std::filesystem::directory_entry& filepath) {
        const auto& path = filepath.path();
        const auto stem = path.has_stem() ? path.stem().string() : "";
        std::string filetype = path.has_extension() ? path.extension().string().substr(1) : "";

        auto get_material_type = [](std::string_view filename) -> std::string {
            if (filename.size() > 5) {
                filename.remove_prefix(filename.size() - 3);
                if (filename == "ALB" || filename == "RMA" || filename == "NRM") {
                    return std::string{ filename };
                }
            }
            return "NONE";
        };

        return { path.string(), path.parent_path().string(), stem, Lowercase(filetype), get_material_type(stem) };
    }

    bool FileExists(const std::string_view name) {
        struct stat buffer;
        return (stat(name.data(), &buffer) == 0);
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
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!std::filesystem::is_regular_file(entry))
                continue;

            FileInfo fileInfo = { GetFullPath(entry), GetFileNameWithoutExtension(entry), GetFileExtension(entry), directory };

            if (extensions.empty() || std::find(extensions.begin(), extensions.end(), fileInfo.ext) != extensions.end())
                fileInfoList.push_back(fileInfo);
        }
        return fileInfoList;
    }

    std::string GetFilename(const std::string& filepath) {
        std::string result = filepath.substr(filepath.rfind("/") + 1);
        return result.substr(0, result.length() - 4);
    }
}