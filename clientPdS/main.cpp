#include <iostream>
#include "tcp_client.h"

using boost::asio::ip::tcp;


int main(int argc, char* argv[])
{
    try
    {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp_client c(io_context);

        c.start(resolver.resolve("127.0.0.1", "8001"));
        std::cout<<"test1"<<std::endl;
        io_context.run();
        std::cout<<"test"<<std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
/*

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
                resolver.resolve("127.0.0.1", "8001");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        for (;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);
            std::cout<<"len: "<<len<<std::endl;


            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            std::cout.write(buf.data(), len);
            std::cout<<"buf: "<<buf.data()<<std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
 */