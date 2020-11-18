//
// Created by simo on 09/11/20.
//

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
    FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, delay{delay} {
        for(auto &file : std::experimental::filesystem::recursive_directory_iterator(path_to_watch)) {
            paths_[file.path().string()] = std::experimental::filesystem::last_write_time(file);
        }
    }

    // Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
    int start(const std::function<int (std::string, FileStatus)> &action) {
        int returnVal = -2;
        while(running_) {
            // Wait for "delay" milliseconds
            std::this_thread::sleep_for(delay);


            auto it = paths_.begin();
            while (it != paths_.end()) {
                if (!std::experimental::filesystem::exists(it->first)) {
                    returnVal = action(it->first, FileStatus::erased);
                    it = paths_.erase(it);
                }
                else {
                    it++;
                }
            }
            if(returnVal !=-2) return returnVal;


            // Check if a file was created or modified
            for(auto &file : std::experimental::filesystem::recursive_directory_iterator(path_to_watch)) {
                auto current_file_last_write_time = std::experimental::filesystem::last_write_time(file);

                // File creation
                if(!contains(file.path().string())) {
                    paths_[file.path().string()] = current_file_last_write_time;
                    returnVal = action(file.path().string(), FileStatus::created);
                    // File modification
                } else {
                    if(paths_[file.path().string()] != current_file_last_write_time) {
                        paths_[file.path().string()] = current_file_last_write_time;
                        returnVal = action(file.path().string(), FileStatus::modified);
                    }
                }
            }
            if(returnVal !=-2) break;

        }
        return returnVal;

    }
private:
    std::unordered_map<std::string, std::experimental::filesystem::file_time_type> paths_;
    bool running_ = true;

    // Check if "paths_" contains a given key
    // If your compiler supports C++20 use paths_.contains(key) instead of this function
    bool contains(const std::string &key) {
        auto el = paths_.find(key);
        return el != paths_.end();
    }
};

#endif //CLIENTPDS_FILE_WATCHER_H
