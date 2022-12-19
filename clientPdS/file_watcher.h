
#ifndef CLIENTPDS_FILE_WATCHER_H
#define CLIENTPDS_FILE_WATCHER_H
//#pragma once
#include <experimental/filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

// Define available file changes
enum class FileStatus {created, modified, erased};

class FileWatcher {
public:
    std::string path_to_watch;

    // Time interval at which we check the base folder for changes
    std::chrono::duration<int, std::milli> delay;

    // Keep a record of files from the base directory and their last modification time
    FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay);

    // Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
    int start(const std::function<int (std::string, FileStatus)> &action);


private:
    std::unordered_map<std::string, std::experimental::filesystem::file_time_type> paths_;
    bool running_ = true;

    // Check if "paths_" contains a given key
    // If your compiler supports C++20 use paths_.contains(key) instead of this function
    bool contains(const std::string &key);
};

#endif //CLIENTPDS_FILE_WATCHER_H
