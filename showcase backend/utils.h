#pragma once
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void save_user_data(const std::string& fio, const std::string& specialty,
    const std::string& subject_scores, const std::string& study_type) {
    std::ofstream outfile("users.txt", std::ios_base::app);
    outfile << fio << "," << specialty << "," << subject_scores << "," << study_type << "\n";
    outfile.close();
}

void save_uploaded_file(const std::string& filename, const std::string& file_content) {
    std::string file_path = "uploads/" + filename;

    // Создаем папку для загрузок, если её нет
    fs::create_directories("uploads");

    std::ofstream ofs(file_path, std::ios::binary);
    ofs.write(file_content.c_str(), file_content.size());
    ofs.close();
}
