#include "file_explorer.h"

FileExplorer::FileExplorer() {
    current_path = fs::current_path();
}

void FileExplorer::run() {
    std::cout << "=== File Explorer Application ===" << std::endl;
    std::cout << "Current directory: " << current_path << std::endl;
    
    while(true) {
        show_menu();
        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();
        
        if(choice == 0) {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        
        handle_choice(choice);
    }
}

void FileExplorer::show_menu() {
    std::cout << "\n=== File Explorer Menu ===" << std::endl;
    std::cout << "Current directory: " << current_path << std::endl;
    std::cout << "1. List files (basic)" << std::endl;
    std::cout << "2. List files (detailed)" << std::endl;
    std::cout << "3. Navigate to directory" << std::endl;
    std::cout << "4. Copy file" << std::endl;
    std::cout << "5. Move file" << std::endl;
    std::cout << "6. Delete file" << std::endl;
    std::cout << "7. Create file" << std::endl;
    std::cout << "8. Create directory" << std::endl;
    std::cout << "9. Search files" << std::endl;
    std::cout << "10. Manage permissions" << std::endl;
    std::cout << "0. Exit" << std::endl;
}

void FileExplorer::list_files(bool detailed) {
    std::cout << "\nContents of " << current_path << ":" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    try {
        std::vector<fs::directory_entry> entries;
        for(const auto& entry : fs::directory_iterator(current_path)) {
            entries.push_back(entry);
        }
        
        std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
            bool a_is_dir = a.is_directory();
            bool b_is_dir = b.is_directory();
            if(a_is_dir != b_is_dir) {
                return a_is_dir > b_is_dir;
            }
            return a.path().filename() < b.path().filename();
        });
        
        for(const auto& entry : entries) {
            if(detailed) {
                display_file_info(entry);
            } else {
                std::string marker = entry.is_directory() ? "[DIR] " : "[FILE]";
                std::cout << marker << " " << entry.path().filename() << std::endl;
            }
        }
        
        std::cout << std::string(80, '-') << std::endl;
        std::cout << "Total: " << entries.size() << " items" << std::endl;
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Error accessing directory: " << ex.what() << std::endl;
    }
}

void FileExplorer::display_file_info(const fs::directory_entry& entry) {
    try {
        fs::file_status status = entry.status();
        fs::perms permissions = status.permissions();
        
        std::cout << get_permissions_string(permissions) << " ";
        std::cout << std::setw(3) << entry.hard_link_count() << " ";
        
        struct stat file_stat;
        if(stat(entry.path().c_str(), &file_stat) == 0) {
            struct passwd* pw = getpwuid(file_stat.st_uid);
            struct group* gr = getgrgid(file_stat.st_gid);
            std::cout << std::setw(8) << (pw ? pw->pw_name : "unknown") << " ";
            std::cout << std::setw(8) << (gr ? gr->gr_name : "unknown") << " ";
            
            if(entry.is_directory()) {
                std::cout << std::setw(8) << "<DIR>" << " ";
            } else {
                std::cout << std::setw(8) << format_file_size(entry.file_size()) << " ";
            }
            
            std::time_t mod_time = fs::last_write_time(entry.path());
            std::cout << std::put_time(std::localtime(&mod_time), "%Y-%m-%d %H:%M") << " ";
        }
        
        std::cout << entry.path().filename() << std::endl;
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Error getting info for: " << entry.path() << " - " << ex.what() << std::endl;
    }
}

std::string FileExplorer::get_permissions_string(fs::perms p) {
    std::string result(10, '-');
    
    result[0] = ((p & fs::perms::owner_read) != fs::perms::none ? 'r' : '-');
    result[1] = ((p & fs::perms::owner_write) != fs::perms::none ? 'w' : '-');
    result[2] = ((p & fs::perms::owner_exec) != fs::perms::none ? 'x' : '-');
    result[3] = ((p & fs::perms::group_read) != fs::perms::none ? 'r' : '-');
    result[4] = ((p & fs::perms::group_write) != fs::perms::none ? 'w' : '-');
    result[5] = ((p & fs::perms::group_exec) != fs::perms::none ? 'x' : '-');
    result[6] = ((p & fs::perms::others_read) != fs::perms::none ? 'r' : '-');
    result[7] = ((p & fs::perms::others_write) != fs::perms::none ? 'w' : '-');
    result[8] = ((p & fs::perms::others_exec) != fs::perms::none ? 'x' : '-');
    
    return result;
}

std::string FileExplorer::format_file_size(uintmax_t size) {
    std::ostringstream oss;
    if(size < 1024) {
        oss << size << "B";
    } else if(size < 1024 * 1024) {
        oss << std::fixed << std::setprecision(1) << (size / 1024.0) << "K";
    } else if(size < 1024 * 1024 * 1024) {
        oss << std::fixed << std::setprecision(1) << (size / (1024.0 * 1024.0)) << "M";
    } else {
        oss << std::fixed << std::setprecision(1) << (size / (1024.0 * 1024.0 * 1024.0)) << "G";
    }
    return oss.str();
}

void FileExplorer::handle_choice(int choice) {
    switch(choice) {
        case 1:
            list_files(false);
            break;
        case 2:
            list_files(true);
            break;
        case 3:
            navigate();
            break;
        case 4:
            copy_file();
            break;
        case 5:
            move_file();
            break;
        case 6:
            delete_file();
            break;
        case 7:
            create_file();
            break;
        case 8:
            create_directory();
            break;
        case 9:
            search_files();
            break;
        case 10:
            manage_permissions();
            break;
        default:
            std::cout << "Invalid choice!" << std::endl;
    }
}

void FileExplorer::navigate() {
    std::cout << "\n=== Navigation ===" << std::endl;
    std::cout << "Current directory: " << current_path << std::endl;
    std::cout << "1. Go to parent directory" << std::endl;
    std::cout << "2. Go to subdirectory" << std::endl;
    std::cout << "3. Go to home directory" << std::endl;
    std::cout << "4. Go to specific path" << std::endl;
    std::cout << "Choose option: ";
    
    int option;
    std::cin >> option;
    std::cin.ignore();
    
    try {
        switch(option) {
            case 1: {
                if(current_path.has_parent_path()) {
                    current_path = current_path.parent_path();
                    std::cout << "Moved to parent directory: " << current_path << std::endl;
                } else {
                    std::cout << "Already at root directory!" << std::endl;
                }
                break;
            }
            case 2: {
                std::cout << "Available directories:" << std::endl;
                std::vector<std::string> directories;
                for(const auto& entry : fs::directory_iterator(current_path)) {
                    if(entry.is_directory()) {
                        directories.push_back(entry.path().filename());
                        std::cout << "- " << entry.path().filename() << std::endl;
                    }
                }
                
                if(directories.empty()) {
                    std::cout << "No subdirectories available." << std::endl;
                    return;
                }
                
                std::cout << "Enter directory name: ";
                std::string dir_name;
                std::getline(std::cin, dir_name);
                
                fs::path new_path = current_path / dir_name;
                if(fs::exists(new_path) && fs::is_directory(new_path)) {
                    current_path = new_path;
                    std::cout << "Moved to: " << current_path << std::endl;
                } else {
                    std::cout << "Directory not found!" << std::endl;
                }
                break;
            }
            case 3: {
                const char* home_dir = getenv("HOME");
                if(home_dir) {
                    current_path = home_dir;
                    std::cout << "Moved to home directory: " << current_path << std::endl;
                } else {
                    std::cout << "Could not find home directory!" << std::endl;
                }
                break;
            }
            case 4: {
                std::cout << "Enter full path: ";
                std::string new_path_str;
                std::getline(std::cin, new_path_str);
                
                fs::path new_path(new_path_str);
                if(fs::exists(new_path) && fs::is_directory(new_path)) {
                    current_path = fs::canonical(new_path);
                    std::cout << "Moved to: " << current_path << std::endl;
                } else {
                    std::cout << "Path does not exist or is not a directory!" << std::endl;
                }
                break;
            }
            default:
                std::cout << "Invalid option!" << std::endl;
        }
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Navigation error: " << ex.what() << std::endl;
    }
}

void FileExplorer::copy_file() {
    std::cout << "\n=== Copy File ===" << std::endl;
    
    std::string source_name, dest_name;
    std::cout << "Enter source filename: ";
    std::getline(std::cin, source_name);
    std::cout << "Enter destination filename: ";
    std::getline(std::cin, dest_name);
    
    fs::path source_path = current_path / source_name;
    fs::path dest_path = current_path / dest_name;
    
    try {
        if(!fs::exists(source_path)) {
            std::cout << "Source file does not exist!" << std::endl;
            return;
        }
        
        if(fs::exists(dest_path)) {
            std::cout << "Destination file already exists. Overwrite? (y/n): ";
            char response;
            std::cin >> response;
            std::cin.ignore();
            if(response != 'y' && response != 'Y') {
                std::cout << "Copy cancelled." << std::endl;
                return;
            }
        }
        
        fs::copy(source_path, dest_path, fs::copy_options::overwrite_existing);
        std::cout << "File copied successfully!" << std::endl;
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Copy error: " << ex.what() << std::endl;
    }
}

void FileExplorer::move_file() {
    std::cout << "\n=== Move File ===" << std::endl;
    
    std::string source_name, dest_name;
    std::cout << "Enter source filename: ";
    std::getline(std::cin, source_name);
    std::cout << "Enter destination filename: ";
    std::getline(std::cin, dest_name);
    
    fs::path source_path = current_path / source_name;
    fs::path dest_path = current_path / dest_name;
    
    try {
        if(!fs::exists(source_path)) {
            std::cout << "Source file does not exist!" << std::endl;
            return;
        }
        
        if(fs::exists(dest_path)) {
            std::cout << "Destination file already exists. Overwrite? (y/n): ";
            char response;
            std::cin >> response;
            std::cin.ignore();
            if(response != 'y' && response != 'Y') {
                std::cout << "Move cancelled." << std::endl;
                return;
            }
        }
        
        fs::rename(source_path, dest_path);
        std::cout << "File moved successfully!" << std::endl;
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Move error: " << ex.what() << std::endl;
    }
}

void FileExplorer::delete_file() {
    std::cout << "\n=== Delete File ===" << std::endl;
    
    std::string filename;
    std::cout << "Enter filename to delete: ";
    std::getline(std::cin, filename);
    
    fs::path file_path = current_path / filename;
    
    try {
        if(!fs::exists(file_path)) {
            std::cout << "File does not exist!" << std::endl;
            return;
        }
        
        if(fs::is_directory(file_path)) {
            std::cout << "Warning: This is a directory. Delete recursively? (y/n): ";
            char response;
            std::cin >> response;
            std::cin.ignore();
            if(response == 'y' || response == 'Y') {
                fs::remove_all(file_path);
                std::cout << "Directory deleted successfully!" << std::endl;
            } else {
                std::cout << "Delete cancelled." << std::endl;
            }
        } else {
            std::cout << "Are you sure you want to delete '" << filename << "'? (y/n): ";
            char response;
            std::cin >> response;
            std::cin.ignore();
            if(response == 'y' || response == 'Y') {
                fs::remove(file_path);
                std::cout << "File deleted successfully!" << std::endl;
            } else {
                std::cout << "Delete cancelled." << std::endl;
            }
        }
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Delete error: " << ex.what() << std::endl;
    }
}

void FileExplorer::create_file() {
    std::cout << "\n=== Create File ===" << std::endl;
    
    std::string filename;
    std::cout << "Enter new filename: ";
    std::getline(std::cin, filename);
    
    fs::path file_path = current_path / filename;
    
    try {
        if(fs::exists(file_path)) {
            std::cout << "File already exists!" << std::endl;
            return;
        }
        
        std::ofstream file(file_path);
        if(file.is_open()) {
            file.close();
            std::cout << "File created successfully!" << std::endl;
        } else {
            std::cout << "Failed to create file!" << std::endl;
        }
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Create file error: " << ex.what() << std::endl;
    }
}

void FileExplorer::create_directory() {
    std::cout << "\n=== Create Directory ===" << std::endl;
    
    std::string dirname;
    std::cout << "Enter new directory name: ";
    std::getline(std::cin, dirname);
    
    fs::path dir_path = current_path / dirname;
    
    try {
        if(fs::exists(dir_path)) {
            std::cout << "Directory already exists!" << std::endl;
            return;
        }
        
        if(fs::create_directory(dir_path)) {
            std::cout << "Directory created successfully!" << std::endl;
        } else {
            std::cout << "Failed to create directory!" << std::endl;
        }
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Create directory error: " << ex.what() << std::endl;
    }
}

void FileExplorer::search_files() {
    std::cout << "\n=== Search Files ===" << std::endl;
    
    std::string search_term;
    std::cout << "Enter search term (filename or pattern): ";
    std::getline(std::cin, search_term);
    
    std::cout << "Searching for: " << search_term << std::endl;
    std::cout << "In directory: " << current_path << std::endl;
    
    try {
        bool found = false;
        for(const auto& entry : fs::recursive_directory_iterator(current_path)) {
            std::string filename = entry.path().filename().string();
            
            if(filename.find(search_term) != std::string::npos) {
                std::string type = entry.is_directory() ? "[DIR] " : "[FILE]";
                std::cout << type << " " << entry.path() << std::endl;
                found = true;
            }
        }
        
        if(!found) {
            std::cout << "No files or directories found matching: " << search_term << std::endl;
        }
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Search error: " << ex.what() << std::endl;
    }
}

void FileExplorer::manage_permissions() {
    std::cout << "\n=== Manage Permissions ===" << std::endl;
    
    std::string filename;
    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);
    
    fs::path file_path = current_path / filename;
    
    try {
        if(!fs::exists(file_path)) {
            std::cout << "File does not exist!" << std::endl;
            return;
        }
        
        fs::perms current_perms = fs::status(file_path).permissions();
        std::cout << "Current permissions: " << get_permissions_string(current_perms) << std::endl;
        
        std::cout << "\nPermission options:" << std::endl;
        std::cout << "1. Add read permission for all" << std::endl;
        std::cout << "2. Add write permission for all" << std::endl;
        std::cout << "3. Add execute permission for all" << std::endl;
        std::cout << "4. Remove read permission for all" << std::endl;
        std::cout << "5. Remove write permission for all" << std::endl;
        std::cout << "6. Remove execute permission for all" << std::endl;
        std::cout << "7. Set to read-only for all" << std::endl;
        std::cout << "8. Set custom permissions (octal)" << std::endl;
        std::cout << "Choose option: ";
        
        int option;
        std::cin >> option;
        std::cin.ignore();
        
        fs::perms new_perms = current_perms;
        
        switch(option) {
            case 1:
                new_perms |= (fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read);
                break;
            case 2:
                new_perms |= (fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write);
                break;
            case 3:
                new_perms |= (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec);
                break;
            case 4:
                new_perms &= ~(fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read);
                break;
            case 5:
                new_perms &= ~(fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write);
                break;
            case 6:
                new_perms &= ~(fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec);
                break;
            case 7:
                new_perms = fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read;
                break;
            case 8: {
                std::cout << "Enter octal permissions (e.g., 755): ";
                std::string octal_str;
                std::getline(std::cin, octal_str);
                try {
                    int octal_val = std::stoi(octal_str, nullptr, 8);
                    new_perms = static_cast<fs::perms>(octal_val);
                } catch(const std::exception& ex) {
                    std::cerr << "Invalid octal value!" << std::endl;
                    return;
                }
                break;
            }
            default:
                std::cout << "Invalid option!" << std::endl;
                return;
        }
        
        fs::permissions(file_path, new_perms);
        std::cout << "Permissions updated successfully!" << std::endl;
        std::cout << "New permissions: " << get_permissions_string(new_perms) << std::endl;
        
    } catch(const fs::filesystem_error& ex) {
        std::cerr << "Permission management error: " << ex.what() << std::endl;
    }
}