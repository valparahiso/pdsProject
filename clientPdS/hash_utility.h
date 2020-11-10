//
// Created by simo on 10/11/20.
//

#ifndef CLIENTPDS_HASH_UTILITY_H
#define CLIENTPDS_HASH_UTILITY_H


#include <string>
#include <sstream>
#include <openssl/md5.h>
#include <iomanip>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <zconf.h>
#include <iostream>

namespace hash_utility {

    std::string calculate_password_hash(const std::string& password_);

    std::string calculate_hash(const std::string& file_path);

    static unsigned long get_size_by_fd(int fd);
}

#endif //CLIENTPDS_HASH_UTILITY_H
