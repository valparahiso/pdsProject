//
// Created by mik on 11/10/20.
//

#ifndef PDSPROJECT_TCP_CONNECTION_H
#define PDSPROJECT_TCP_CONNECTION_H
#include <ctime>
#include <iostream>
#include <string>
#include <memory>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <openssl/md5.h>
#include <sys/mman.h>
#include "JSON_utility.h"
#include "db_utility.h"

class tcp_connection: public boost::enable_shared_from_this<tcp_connection> {
    public:
        typedef boost::shared_ptr<tcp_connection> pointer;
        static pointer create(boost::asio::io_context& io_context){
            return pointer(new tcp_connection(io_context));
        }

        boost::asio::ip::tcp::socket& socket();

        void start();

    private:
        tcp_connection(boost::asio::io_context& io_context): socket_(io_context){}


        void write_data(boost::property_tree::ptree JSON);


        void read_data();

        void handle_write_data(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);

        void handle_read_data(const boost::system::error_code& ec, std::size_t n);

        void login_fun();

        bool check_directory(boost::filesystem::path path);

        bool check_validity(boost::property_tree::ptree JSON_client);



        std::vector<boost::property_tree::ptree> to_file_from_bytes(std::string path_file);

        std::vector<boost::property_tree::ptree> send_file(std::string path_file, int bytes_to_transfer, std::vector<boost::property_tree::ptree> file_blocks, int offset);

        void create_file(std::string path, std::string name_file, std::vector<unsigned char> buffer);

        void set_login();

        void restore_fun();

        void check_validity_fun();

        void default_fun();

        void ask_file_fun();

        void file_received_fun();

        void sending_file_fun();


        boost::asio::ip::tcp::socket socket_;
        std::string input_buffer_;
        boost::filesystem::path path_;
        std::string command_;
        std::string username_;
        std::string password_;
        boost::property_tree::ptree JSON_server; //nostro file system
        boost::property_tree::ptree JSON_client; //filesystem del client
        std::vector<boost::property_tree::ptree> file_blocks;
        std::vector<boost::property_tree::ptree> files;



};



#endif //PDSPROJECT_TCP_CONNECTION_H
