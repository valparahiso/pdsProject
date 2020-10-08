#include <iostream>
#include "Client.h"
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

int main(int argc, char** argv) {

    if(argc == 1){
        std::cout<<"Errore, nessun argomento"<<std::endl;
        return 1;
    }

    Client cl(argv[1]);

    if(argc > 2){ // ci sono opzioni
        std::string comando(argv[2]);
        if(comando.compare("check_validity")==0){
            //check validity
        } else if((comando.compare("restore"))==0){
            //restore
        } else {
            std::cout<<argv[2]<<" non e' un comando valido."<<std::endl<<"I comandi accettati sono:"
                     <<std::endl<<"1)'check_validity': controlla la validità della directory."
                     <<std::endl<<"2)'restore': scarica l'ultima versione della directory dal server se presente."
                     <<std::endl<<"3) nessun argomento: background check delle modifiche sulla directory specificata e backup automatico sul server"<<std::endl;
        }

    } else {
        //default
    }



    boost::asio::io_service io_service;
//socket creation
    tcp::socket socket(io_service);
//connection
    socket.connect( tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 8001 ));
// request/message from client
    const string msg = "Hello from Client!\n";
    boost::system::error_code error;
    boost::asio::write( socket, boost::asio::buffer(msg), error );
    if( !error ) {
        cout << "Client sent hello message!" << endl;
    }
    else {
        cout << "send failed: " << error.message() << endl;
    }
    // getting response from server
    boost::asio::streambuf receive_buffer;
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
    if( error && error != boost::asio::error::eof ) {
        cout << "receive failed: " << error.message() << endl;
    }
    else {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
        cout << data << endl;
    }
    return 0;
}
