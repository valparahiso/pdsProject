#include <boost/asio/io_context.hpp>
#include "tcp_server.h"
#include <iostream>
#include <string>
#include <sqlite3.h>

using boost::asio::ip::tcp;



int main(){
    try{


        sqlite3 *db;

        int rc;

        rc = sqlite3_open("db/server.db", &db);

        if( rc )
        {
            std::cout<<"Can't open database: "<<sqlite3_errmsg(db)<<"\n";
        }
        else
        {
            std::cout<<"Open database successfully\n\n";
        }

        boost::asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    return 0;
}


