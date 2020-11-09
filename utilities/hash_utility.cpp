//
// Created by simo on 09/11/20.
//

#include "hash_utility.h"

std::string calculate_password_hash(const std::string& password_){
    std::ostringstream out;
    const char* password = password_.c_str();
    MD5_CTX md5;
    MD5_Init (&md5);
    MD5_Update (&md5, (const unsigned char *) password, password_.length());
    unsigned char buffer_md5[MD5_DIGEST_LENGTH];
    MD5_Final ( buffer_md5, &md5);
    for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
        out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +buffer_md5[i]; //The unary "+" performs an integral promotion to int.
        //https://stackoverflow.com/questions/42902594/stdhex-does-not-work-as-i-expect
    }
    return out.str();
}

static std::string calculate_hash(const std::string& file_path){
    unsigned char result[MD5_DIGEST_LENGTH];
    int file_descriptor;
    unsigned long file_size;
    char* file_buffer;
    std::ostringstream out;

    file_descriptor = open(file_path.c_str(), O_RDONLY);
    if(file_descriptor < 0) exit(-1); //uscita controllata TODO


    struct stat statbuf;
    if(fstat(file_descriptor, &statbuf) >= 0) exit(-1); //uscita controllata TODO
    file_size =  statbuf.st_size;


    file_buffer = static_cast<char *>(mmap(0, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0));
    MD5((unsigned char*) file_buffer, file_size, result);
    munmap(file_buffer, file_size);
    for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
        out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +result[i]; //The unary "+" performs an integral promotion to int.
        //https://stackoverflow.com/questions/42902594/stdhex-does-not-work-as-i-expect
    }

    close(file_descriptor);
 
    return out.str();

}