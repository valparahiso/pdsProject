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
            std::cout<<"In start -server"<<std::endl;

            boost::asio::async_read_until(socket_,
                                          boost::asio::dynamic_buffer(input_buffer_), '\n',
                                          boost::bind(&tcp_connection::authentication, shared_from_this(), boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred));
        }



    private:
        tcp_connection(boost::asio::io_context& io_context): socket_(io_context){}

        void authentication(const boost::system::error_code& ec, std::size_t n)
        {
            std::cout<<"STARTING AUTHENTICATION:"<<std::endl;

            std::cout<<"EC: "<<ec<<std::endl;

            if (!ec)
            {

                // Extract the newline-delimited message from the buffer.
                std::string line(input_buffer_.substr(0, n - 1));
                input_buffer_.erase(0, n);

                // Empty messages are heartbeats and so ignored.
                if (!line.empty())
                {
                    std::stringstream ss(line);

                    std::string username;
                    std::getline(ss, username, '-');
                    std::cout << "Username: " << username << "\n";

                    std::string password;
                    std::getline(ss, password, '-');
                    std::cout << "Password: " << password << "\n";

                    std::string message = this->login_db(username, password) ? "user_accepted\n" : "user_refused\n";

                    boost::asio::async_write(socket_, boost::asio::buffer(message),
                                             boost::bind(&tcp_connection::get_directory_and_command, shared_from_this(),
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));

                }

            }
            else
            {
                std::cout << "Error on receive: " << ec.message() << "\n";
            }
        }

        bool login_db(std::string username, std::string password){
            sqlite3 *db;
            int rc;
            char *zErrMsg = 0;

            rc = sqlite3_open("db/server.db", &db);

            if( rc )
            {
                std::cout<<"Can't open database: "<<sqlite3_errmsg(db)<<"\n";
            }
            else
            {
                std::cout<<"Open database successfully\n\n";
            }

            std::string query_string = "select count(*) from user where username = '" + username + "' and password = '" + password + "'";
            const char *query = query_string.c_str();
            std::cout<<query<<std::endl;

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
            std::cout<<"NUmero di righe : "<<argc<<"\n"<<azColName[0]<<"       "<<argv[0]<<std::endl;
            if(std::string(argv[0]) == "1"){
                std::cout<<"Utente presente nel DB!"<<std::endl;
                return 0;
            } else {
                std::cout<<"Utente NON presente nel DB!"<<std::endl;
                return 1;
            }
        }

    void get_directory_and_command(const boost::system::error_code& /*error*/,
                                   size_t /*bytes_transferred*/){
        std::cout<<"READING DIRECTORY AND COMMAND FROM THE CLIENT"<<std::endl;
        boost::asio::async_read_until(socket_,
                                      boost::asio::dynamic_buffer(input_buffer_), '\n',
                                      boost::bind(&tcp_connection::menu, shared_from_this(), boost::asio::placeholders::error,
                                                  boost::asio::placeholders::bytes_transferred));

    }

    void menu(const boost::system::error_code& ec, std::size_t n)
    {
        /*std::cout<<"SELECTING COMMAND:"<<std::endl;

        std::cout<<"EC: "<<ec<<std::endl;

        if (!ec)
        {

            // Extract the newline-delimited message from the buffer.
            std::string line(input_buffer_.substr(0, n - 1));
            input_buffer_.erase(0, n);

            // Empty messages are heartbeats and so ignored.
            if (!line.empty())
            {
                std::cout << "Error on receive: " << ec.message() << "\n";
            }
        }
        else
        {
            std::cout << "Error on receive: " << ec.message() << "\n";
        }*/
    }



        tcp::socket socket_;
        std::string input_buffer_;
};



#endif //PDSPROJECT_TCP_CONNECTION_H
