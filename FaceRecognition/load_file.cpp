#include "load_file.h"

//jpg, jpeg, png만 허용하게 함 / 반환 타입 filesystem::path
std::vector<std::filesystem::path> load_image_path(std::string folder) {
    std::vector<std::filesystem::path> path;

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / folder))
        if (!entry.is_directory() && (entry.path().extension() == ".jpg" ||
            entry.path().extension() == ".png" ||
            entry.path().extension() == ".jpeg"))
            path.push_back(entry.path());

    return path;
}

std::string get_filename(std::filesystem::path& path) {
    return path.filename().string().substr(0, path.filename().string().find('.'));
}