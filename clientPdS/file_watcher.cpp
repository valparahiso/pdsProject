//
// Created by simo on 09/11/20.
//

#include "file_watcher.h"

FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, delay{delay} {
    for(auto &file : std::experimental::filesystem::recursive_directory_iterator(path_to_watch)) {
        paths_[file.path().string()] = std::experimental::filesystem::last_write_time(file);
    }
}


// Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
int FileWatcher::start(const std::function<int (std::string, FileStatus)> &action) {
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


bool FileWatcher::contains(const std::string &key) {
    auto el = paths_.find(key);
    return el != paths_.end();
}