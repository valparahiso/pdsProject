

#include "hash_utility.h"

std::string hash_utility::calculate_password_hash(const std::string& password_){
    std::ostringstream out;
    const char* password = password_.c_str();
    MD5_CTX md5;
    MD5_Init (&md5);
    MD5_Update (&md5, (const unsigned char *) password, password_.length());
    unsigned char buffer_md5[MD5_DIGEST_LENGTH];
    MD5_Final ( buffer_md5, &md5);
    for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
        out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +buffer_md5[i]; //The unary "+" performs an integral promotion to int.
    }
    return out.str();
}

std::string hash_utility::calculate_hash(const std::string& file_path){
    unsigned char result[MD5_DIGEST_LENGTH];
    int file_descriptor;
    unsigned long file_size;
    char* file_buffer;
    std::ostringstream out;

    file_descriptor = open(file_path.c_str(), O_RDONLY);
    if(file_descriptor < 0) return "";


    file_size = hash_utility::get_size_by_fd(file_descriptor);


    file_buffer = static_cast<char *>(mmap(0, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0));
    MD5((unsigned char*) file_buffer, file_size, result);
    munmap(file_buffer, file_size);
    for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
        out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +result[i];
    }

    close(file_descriptor);

    return out.str();

}


// Get the size of the file by its file descriptor
static unsigned long hash_utility::get_size_by_fd(int fd) {
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) exit(-1);
    return statbuf.st_size;
}