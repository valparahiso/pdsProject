#include <iostream>
#include "user.h"
#include "parameters_exception.h"

int main(int argc, char* argv[])
{
    try
    {
        if(argc < 2 || argc > 3) { // user.exe "nome directory" "comando"

            throw parameters_exception();
        }
        user user1 = (argc == 2) ? user(std::string(argv[1]), "default") : user(std::string(argv[1]), std::string(argv[2]));

        user1.authentication();

        if(argc == 2){ // specificata solo la directory ---> COMANDO DI DEFAULT
            std::cout<<"DEFAULT "<<std::endl;


        } else {
            std::string comando = argv[2];
            if(comando.compare("check_validity")==0){ // check_validity
                std::cout<<"CHECK VALIDITY "<<std::endl;

            } else if(comando.compare("restore")==0){ // restore
                std::cout<<"RESTORE "<<std::endl;
            }
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
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
            std::cerr << "Usage: user <host>" << std::endl;
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
} */
