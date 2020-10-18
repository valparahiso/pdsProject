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

//public boost::enable_shared
//std::std::enable_shared_from_this<tcp_connection> (senza public)

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

            message_ = make_daytime_string();
            std::cout<<"MESSAGGIO: "<<message_<<std::endl;
           boost::asio::async_write(socket_, boost::asio::buffer(message_),
                                 boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
           // boost::asio::async_write(socket_, boost::asio::buffer(message_,256), &tcp_connection::handle_write);
        }



    private:
        tcp_connection(boost::asio::io_context& io_context): socket_(io_context){}
        void handle_write(const boost::system::error_code& /*error*/,
                      size_t /*bytes_transferred*/){

        }

    void handle_read(const boost::system::error_code& ec, std::size_t n)
    {
        std::cout<<"In handle read - server"<<std::endl;

        std::cout<<"EC: "<<ec<<std::endl;

        std::cout<<"Ricevuto : "<<input_buffer_<<std::endl;
        if (!ec)
        {

            // Extract the newline-delimited message from the buffer.
            std::string line(input_buffer_.substr(0, n - 1));
            input_buffer_.erase(0, n);

            // Empty messages are heartbeats and so ignored.
            if (!line.empty())
            {
                std::cout << "Received: " << line << "\n";
            }

        }
        else
        {
            std::cout << "Error on receive: " << ec.message() << "\n";
        }
    }


    tcp::socket socket_;
        std::string message_;
        std::string input_buffer_;
};



#endif //PDSPROJECT_TCP_CONNECTION_H
