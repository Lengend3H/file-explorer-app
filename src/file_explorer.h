#ifndef FILE_EXPLORER_H
#define FILE_EXPLORER_H

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <chrono>

namespace fs = std::filesystem;

class FileExplorer {
private:
    fs::path current_path;
    
public:
    FileExplorer();
    void run();
    
private:
    void list_files(bool detailed = false);
    void navigate();
    void show_menu();
    void handle_choice(int choice);
    void copy_file();
    void move_file();
    void delete_file();
    void create_file();
    void create_directory();
    void search_files();
    void manage_permissions();
    std::string get_permissions_string(fs::perms p);
    std::string format_file_size(uintmax_t size);
    void display_file_info(const fs::directory_entry& entry);
    std::time_t to_time_t(const fs::file_time_type& ftime);
};

#endif