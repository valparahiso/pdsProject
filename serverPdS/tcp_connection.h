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
using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}


class tcp_connection: std::enable_shared_from_this<tcp_connection> {
    public:
        typedef boost::shared_ptr<tcp_connection> pointer;
        static pointer create(boost::asio::io_context& io_context){
            return pointer(new tcp_connection(io_context));
        }

        tcp::socket& socket(){
            return socket_;
        }

        void start(){
            message_ = make_daytime_string();
            std::cout<<"MESSAGGIO: "<<message_<<std::endl;
            boost::asio::async_write(socket_, boost::asio::buffer(message_),
                                 boost::bind(&tcp_connection::handle_write, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred)); //ghghghghgh
           // boost::asio::async_write(socket_, boost::asio::buffer(message_,256), &tcp_connection::handle_write);




        }

    private:
        tcp_connection(boost::asio::io_context& io_context): socket_(io_context){}
        void handle_write(const boost::system::error_code& /*error*/,
                      size_t /*bytes_transferred*/){}

        tcp::socket socket_;
        std::string message_;
};



#endif //PDSPROJECT_TCP_CONNECTION_H
