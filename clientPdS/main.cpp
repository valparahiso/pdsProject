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
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
