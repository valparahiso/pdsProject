//
// Created by simo on 06/10/20.
//

#ifndef SERVERPDS_CON_HANDLER_H
#define SERVERPDS_CON_HANDLER_H
//importing libraries
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;


class con_handler : public boost::enable_shared_from_this<con_handler>
{
private:
    tcp::socket sock;
    std::string message="Hello From Server!";
    enum { max_length = 1024 };
    char data[max_length];

public:
    typedef boost::shared_ptr<con_handler> pointer;
    con_handler(boost::asio::io_service& io_service): sock(io_service){}

    // creating the pointer
    static  boost::shared_ptr<con_handler> create(boost::asio::io_service& io_service);

    //socket creation
    tcp::socket& socket();

    void start();

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred);

    void handle_write(const boost::system::error_code& err, size_t bytes_transferred);
};


#endif //SERVERPDS_CON_HANDLER_H
