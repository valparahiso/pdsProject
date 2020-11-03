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
#include <sqlite3.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <openssl/md5.h>
#include <sys/mman.h>

class tcp_connection: public boost::enable_shared_from_this<tcp_connection> {
    public:
        typedef boost::shared_ptr<tcp_connection> pointer;
        static pointer create(boost::asio::io_context& io_context){
            return pointer(new tcp_connection(io_context));
        }

        boost::asio::ip::tcp::socket& socket(){
            return socket_;
        }

        void start(){
            std::cout<<"- Starting server . . ."<<std::endl;
            read_data();
        }

    private:
        tcp_connection(boost::asio::io_context& io_context): socket_(io_context){}


        void write_data(boost::property_tree::ptree JSON)
        {
            std::ostringstream JSON_string;
            write_json(JSON_string, JSON);
            std::string data = JSON_string.str() + "/";
            std::cout<<"MESSAGGIO CHE STO PER INVIARE: "<<data<<std::endl;
            // Start an asynchronous operation to send a heartbeat message.
            boost::asio::async_write(socket_, boost::asio::buffer(data),
                                     boost::bind(&tcp_connection::handle_write_data,
                                                 shared_from_this(),
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred));
        }


        void read_data()
        {
            // Start an asynchronous operation to read a newline-delimited message.
            boost::asio::async_read_until(socket_,
                                          boost::asio::dynamic_buffer(input_buffer_), '/',
                                          boost::bind(&tcp_connection::handle_read_data, shared_from_this(), boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred));


        }

        void handle_write_data(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/){
            read_data();

        }

        void handle_read_data(const boost::system::error_code& ec, std::size_t n)
        {
            if (!ec)
            {
                // Extract the newline-delimited message from the buffer.
                std::string line(input_buffer_.substr(0, n - 1));
                input_buffer_.erase(0, n);

                // Empty messages are heartbeats and so ignored.
                if (!line.empty())
                {
                    std::cout<<"MESSAGGIO RICEVUTO : "<<line<<std::endl;
                    std::istringstream ss(line);
                    read_json(ss, JSON_client);

                    if(JSON_client.get("connection", "connection_error") == "login"){
                        username_ = JSON_client.get("login.username", "NO_USERNAME");
                        password_ = JSON_client.get("login.password", "NO_PASSWORD");

                        std::cout << "USERNAME: "<<username_<<"     PASSWORD: " << password_ <<" . . . ."<<std::endl;
                        if(login_db(username_, password_)){
                            JSON_client.put("connection", "logged");
                            command_ = JSON_client.get("dir_and_command.command", "NO_COMMAND");
                            path_ = boost::filesystem::path(username_ + "/" + JSON_client.get("dir_and_command.directory", "NO_DIRECTORY"));
                            std::cout << "PATH: "<<path_.string()<<"     COMMAND: " << command_ <<" . . . ."<<std::endl;
                            if(command_ == "restore" || command_ == "check_validity"){
                                if(check_directory(path_)){
                                    //boost::property_tree::write_json(std::cout, JSON.get_child("data"));
                                    if(!JSON_client.get_child("data").empty() && check_validity(JSON_client.get_child("data"))){
                                        JSON_client.put("connection", "directory_valid");
                                        write_data(JSON_client);
                                    } else if(command_ == "check_validity"){
                                        JSON_client.put("connection", "directory_invalid");
                                        write_data(JSON_client);
                                    } else {
                                        //RESTORE
                                        if(JSON_client.get_child("data").empty()){
                                            std::cout<<"CARTELLA NON PRESENTE SUL CLIENT"<<std::endl;
                                            JSON_server = JSON_client;
                                            JSON_server.put_child("data",create_data_json(path_, 1));
                                            JSON_server.put("connection", "empty_data");
                                            write_data(JSON_server);
                                        }
                                    }
                                } else {
                                    JSON_client.put("connection", "directory_error");
                                    write_data(JSON_client);
                                    //RILASCIARE RISORSE SOCKET
                                }
                            } else if(command_ == "default"){
                                //IMPLEMENTARE DEFAULT
                                std::cout<<"DEFAULT !!! "<<std::endl;
                            }
                        } else {
                            JSON_client.put("connection", "login_error");
                            write_data(JSON_client);
                            //RILASCIARE RISORSE SOCKET.
                        }
                    } else if(JSON_client.get("connection", "connection_error") == "ask_file"){
                        std::cout<<"CLIENT CHIEDE UN FILE"<<std::endl;
                        std::string path = "";
                        for(int i=0; i<JSON_client.get<int>("size"); i++){
                            path += JSON_client.get<std::string>(std::to_string(i)) + "/";
                        }
                        path += JSON_client.get<std::string>("file_name");
                        std::cout<<"PERCORSO FILE : " << path<<std::endl;
                        to_file_from_bytes(path);
                    }

                }

            }
            else
            {
                std::cout<<"Error on receive: "<<std::endl;
                std::cout << "Error number " << ec <<std::endl;
                std::cout << "Error message " << ec.message() <<std::endl;
            }
        }

        bool login_db(std::string username, std::string password){
            sqlite3 *db;
            int rc;
            char *zErrMsg = 0;

            rc = sqlite3_open("db/server.db", &db);

            if( rc )
            {
                std::cout<<"- Can't open database: "<<sqlite3_errmsg(db)<<std::endl;
            }
            else
            {
                std::cout<<"- Opening database . . . .     ";
            }

            std::string query_string = "select count(*) from user where username = '" + username + "' and password = '" + password + "'";
            const char *query = query_string.c_str();

            rc = sqlite3_exec(db, query, check_login, nullptr, &zErrMsg);
            if( rc!=SQLITE_OK )
            {
                std::cout<<"SQL error: "<<sqlite3_errmsg(db)<<"\n";
                sqlite3_free(zErrMsg);
                sqlite3_close(db);
                return false;
            }

            sqlite3_close(db);
            return true;
        }

        static int check_login(void *NotUsed, int argc, char **argv, char **azColName)
        {
            if(std::string(argv[0]) == "1"){
                std::cout<<"Utente presente nel DB . . . ."<<std::endl;
                return 0;
            } else {
                std::cout<<"Utente NON presente nel DB . . . ."<<std::endl;
                return 1;
            }
        }

        bool check_directory(boost::filesystem::path path){
            if(exists(path)){
                if(is_directory(path)){
                    return true;
                }
            }
            return false;
        }

        bool check_validity(boost::property_tree::ptree JSON_client){
            JSON_client.sort();
            JSON_server = create_data_json(path_, 0);
            JSON_server.sort();
            return (JSON_server == JSON_client);
        }

        boost::property_tree::ptree create_data_json(boost::filesystem::path path, int option){

            boost::property_tree::ptree directory;

            for (auto& entry : boost::filesystem::directory_iterator(path)){
                if(is_regular_file(entry.path())){
                    if(option==0)
                        directory.put(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), calculate_hash(entry.path().string()));
                    else
                        directory.put(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), "X");
                } else {
                    directory.add_child(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), create_data_json(entry.path(), option));
                }
            }
            directory.sort();
            return directory;

        }

        static unsigned long get_size_by_fd(int fd) {
            struct stat statbuf;
            if(fstat(fd, &statbuf) < 0) exit(-1); //uscita controllata TODO
            return statbuf.st_size;
        }

        static std::string calculate_hash(std::string file_path){
            unsigned char result[MD5_DIGEST_LENGTH];
            int file_descriptor;
            unsigned long file_size;
            char* file_buffer;
            std::ostringstream out;

            file_descriptor = open(file_path.c_str(), O_RDONLY);
            if(file_descriptor < 0) exit(-1); //uscita controllata TODO

            file_size = get_size_by_fd(file_descriptor);

            file_buffer = static_cast<char *>(mmap(0, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0));
            MD5((unsigned char*) file_buffer, file_size, result);
            munmap(file_buffer, file_size);
            for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
                out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +result[i]; //The unary "+" performs an integral promotion to int.
                //https://stackoverflow.com/questions/42902594/stdhex-does-not-work-as-i-expect
            }

            return out.str();

        }

        void to_file_from_bytes(std::string path_file){
            std::ifstream size(path_file, std::ios::ate | std::ios::binary);
            int file_size = size.tellg();
            size.close();
            if(file_size == -1){
                std::cout<<"FILE INESITENTE "<<std::endl;
                return;
            }

            send_file(path_file, file_size);
        }

        void send_file(std::string path_file, int bytes_to_transfer){

            std::cout<<"LEGGO DAL FILE PARTENDO DALL'INDICE "<<0<<" MANCANO DA LEGGERE " << bytes_to_transfer << " bytes" <<std::endl;
            std::ifstream input(path_file, std::ios::in | std::ios::binary);

            boost::property_tree::ptree file_asked;
            file_asked.put("connection", "sending_file");
            file_asked.put("status", "sending");
            int size = 1024;
            if(bytes_to_transfer >= 1024){
                bytes_to_transfer -= 1024;
            } else {
                if(bytes_to_transfer != 0) {
                    size = bytes_to_transfer;
                    bytes_to_transfer = 0;
                    file_asked.put("status", "last");
                } else {
                    input.close();
                    return;
                }
            }


            std::vector<unsigned char> buffer(size);
            //std::cout<<"LEGGO DAL FILE PARTENDO DALL'INDICE "<<index<<" UNA SIZE DI " << buffer.size() << " bytes" <<std::endl;
            input.read((char*)&buffer[0], buffer.size());

            file_asked.put("data", std::string(buffer.begin(), buffer.end()));
            std::ostringstream JSON_string;
            write_json(JSON_string, file_asked);
            std::string data = JSON_string.str() + "/";
            std::cout<<"MESSAGGIO : "<<data<<std::endl;

            boost::asio::async_write(socket_, boost::asio::buffer(data),
                                     boost::bind(&tcp_connection::send_file,
                                                 shared_from_this(),
                                                 path_file,
                                                 bytes_to_transfer));
        }


        void create_file(std::string path, std::string name_file, std::vector<unsigned char> buffer){
            std::ofstream output(path + "/" + name_file, std::ios::binary);
            output.write((char*) &buffer[0], buffer.size());
            output.close();
        }

        boost::asio::ip::tcp::socket socket_;
        std::string input_buffer_;
        boost::filesystem::path path_;
        std::string command_;
        std::string username_;
        std::string password_;
        boost::property_tree::ptree JSON_server; //nostro file system
        boost::property_tree::ptree JSON_client; //filesystem del client


};



#endif //PDSPROJECT_TCP_CONNECTION_H
