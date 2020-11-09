//
// Created by simo on 09/11/20.
//

#ifndef UTILITIES_HASH_UTILITY_H
#define UTILITIES_HASH_UTILITY_H

#include <string>
#include <sstream>
#include <openssl/md5.h>
#include <iomanip>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <zconf.h>

namespace hash_utility {

    std::string calculate_password_hash(const std::string& password_);

    static std::string calculate_hash(const std::string& file_path);
}


#endif //UTILITIES_HASH_UTILITY_H
