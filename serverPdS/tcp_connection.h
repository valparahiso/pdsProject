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
using boost::asio::ip::tcp;

class tcp_connection: public boost::enable_shared_from_this<tcp_connection> {
    public:
        typedef boost::shared_ptr<tcp_connection> pointer;
        static pointer create(boost::asio::io_context& io_context){
            return pointer(new tcp_connection(io_context));
        }

        tcp::socket& socket(){
            return socket_;
        }

        void start(){
            std::cout<<"- Starting server . . ."<<std::endl;
            operation_ = "login";
            read_data();
        }



    private:
        tcp_connection(boost::asio::io_context& io_context): socket_(io_context){}


        void write_data(std::string data)
        {
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
                                          boost::asio::dynamic_buffer(input_buffer_), '\n',
                                          boost::bind(&tcp_connection::handle_read_data, shared_from_this(), boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred));


        }

        void handle_write_data(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/){
            if(operation_ == "logged"){
                read_data();
            } else {
                socket_.release(); // Non so se è corretto dobbiamo trovare il modo di rilasciare le risorse del socket da parte del server SENZA CHIUDERLO!
            }
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
                    std::stringstream ss(line);

                    if(operation_=="login") {
                        std::cout<<"- Reading Username and Password . . . .     ";

                        std::getline(ss, username_, '-');
                        std::getline(ss, password_, '-');
                        std::cout << "USERNAME: "<<username_<<"     PASSWORD: " << password_ <<" . . . ."<<std::endl;

                        std::string message = this->login_db(username_, password_) ? "user_accepted\n" : "user_refused\n";

                        write_data(message);

                    } else if(operation_ == "logged"){

                        std::cout<<"- Reading Directory and Command . . . .     ";
                        std::getline(ss, directory_, '-');
                        std::getline(ss, command_, '-');
                        std::cout << "DIRECTORY: "<<directory_<<"     COMMAND: " <<command_ <<" . . . ."<<std::endl;

                        if(command_ == "check_validity"){


                        } else if(command_ == "default"){

                        } else if(command_ == "restore"){

                        } else {
                            //COMANDO ERRATO.
                        }

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
            operation_ = "logged";
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

        tcp::socket socket_;
        std::string input_buffer_;
        std::string operation_;
        std::string directory_;
        std::string command_;
        std::string username_;
        std::string password_;


};



#endif //PDSPROJECT_TCP_CONNECTION_H
