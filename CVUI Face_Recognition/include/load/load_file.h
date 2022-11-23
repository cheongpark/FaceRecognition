#pragma once
#include <filesystem>

std::vector<std::filesystem::path> load_image_path(std::string folder);

std::string get_filename(std::filesystem::path& path);