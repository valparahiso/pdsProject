//
// Created by mario on 29/12/20.
//
#include "tcp_connection.h"


boost::asio::ip::tcp::socket& tcp_connection::socket(){
    return socket_;
}

void tcp_connection::start(){
    std::cout<<"- Starting server . . ."<<std::endl;
    read_data();
}

void tcp_connection::write_data(boost::property_tree::ptree JSON)
{
    std::ostringstream JSON_string;
    write_json(JSON_string, JSON);
    std::string data = JSON_string.str() + "/";
    std::cout<<"SEND"<<std::endl;
    JSON_utility::print_JSON(JSON);
    // Start an asynchronous operation to send a heartbeat message.
    boost::asio::async_write(socket_, boost::asio::buffer(data),
                             boost::bind(&tcp_connection::handle_write_data,
                                         shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::read_data()
{
    // Start an asynchronous operation to read a newline-delimited message.
    std::cout<<"RECEIVE"<<std::endl;

    boost::asio::async_read_until(socket_,
                                  boost::asio::dynamic_buffer(input_buffer_), '/',
                                  boost::bind(&tcp_connection::handle_read_data, shared_from_this(), boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));


}

void tcp_connection::handle_write_data(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/){
    //pool->pop(index);
    read_data();

}

void tcp_connection::handle_read_data(const boost::system::error_code& ec, std::size_t n)
{
    if (!ec)
    {

        // Extract the newline-delimited message from the buffer.
        std::string line(input_buffer_.substr(0, n - 1));
        input_buffer_.erase(0, n);

        // Empty messages are heartbeats and so ignored.
        if (!line.empty())
        {
            std::istringstream ss(line);
            read_json(ss, JSON_client);

            JSON_utility::print_JSON(JSON_client);

            if(JSON_client.get("connection", "connection_error") == "login"){
                login_fun();
            } else if(JSON_client.get("connection", "connection_error") == "ask_file"){
                ask_file_fun();
            } else if(JSON_client.get("connection", "connection_error") == "file_received") {
                file_received_fun();
            } else if(JSON_client.get("connection", "connection_error") == "sending_file"){
                sending_file_fun();
            }

        }

    }
}

void tcp_connection::login_fun(){
    set_login();

    if(db_utility::login_db(username_, password_)){
        JSON_client.put("connection", "logged");
        command_ = JSON_client.get("dir_and_command.command", "NO_COMMAND");
        path_ = boost::filesystem::path(username_ + "/" + JSON_client.get("dir_and_command.directory", "NO_DIRECTORY"));

        if(command_ == "restore"){
            restore_fun();
        } else if(command_ == "check_validity"){
            check_validity_fun();
        } else if(command_ == "default"){
            default_fun();
        }
    } else {
        JSON_client.put("connection", "login_error");
        write_data(JSON_client);
        //RILASCIARE RISORSE SOCKET.
    }
}

bool tcp_connection::check_directory(boost::filesystem::path path){
    if(exists(path)){
        if(is_directory(path)){
            return true;
        }
    }
    return false;
}

bool tcp_connection::check_validity(boost::property_tree::ptree JSON_client){
    JSON_client.sort();
    JSON_server.add_child("data", JSON_utility::create_data_json(path_, 0));
    boost::property_tree::ptree JSON_server_sorted = JSON_server.get_child("data");
    JSON_server_sorted.sort();

    boost::property_tree::write_json(std::cout, JSON_server_sorted);
    boost::property_tree::write_json(std::cout, JSON_client);
    return (JSON_server_sorted == JSON_client);
}

std::vector<boost::property_tree::ptree> tcp_connection::to_file_from_bytes(std::string path_file){

    std::vector<boost::property_tree::ptree> file_blocks;
    std::ifstream size(path_file, std::ios::ate | std::ios::binary);
    int file_size = size.tellg();
    size.close();
    if(file_size == -1){
        std::cout<<"FILE INESITENTE "<<std::endl;
        return file_blocks;
    }

    file_blocks = send_file(path_file, file_size, file_blocks, 0);
    return file_blocks;
}

std::vector<boost::property_tree::ptree> tcp_connection::send_file(std::string path_file, int bytes_to_transfer, std::vector<boost::property_tree::ptree> file_blocks, int offset){

    std::ifstream input(path_file, std::ios::in | std::ios::binary);
    input.seekg (offset);


    boost::property_tree::ptree file_asked;
    int size = 30720;
    if(bytes_to_transfer >= 30720){
        bytes_to_transfer -= 30720;
    } else {
        if(bytes_to_transfer >= 0) {
            size = bytes_to_transfer;
            bytes_to_transfer = -1;
            file_asked.put("status", "last");
        } else {
            input.close();
            return file_blocks;
        }
    }


    std::vector<unsigned char> buffer(size);
    input.read((char*)&buffer[0], buffer.size());

    std::stringstream buffer_hex;
    file_asked.put("status", "");
    for(int i=0; i<buffer.size(); i++){
        buffer_hex << std::hex << std::setw(2) << std::setfill('0') << +buffer[i];
    }

    file_asked.put("data", std::string(buffer_hex.str()));

    file_blocks.push_back(file_asked);
    file_blocks = send_file(path_file, bytes_to_transfer, file_blocks, offset + 30720);
    return file_blocks;
}

void tcp_connection::create_file(std::string path, std::string name_file, std::vector<unsigned char> buffer){
    std::ofstream output(path + "/" + name_file, std::ios::binary | std::fstream::app);

    output.write((char*) &buffer[0], buffer.size());
    output.close();
}

void tcp_connection::set_login(){
    username_ = JSON_client.get("login.username", "NO_USERNAME");
    password_ = JSON_client.get("login.password", "NO_PASSWORD");


}

void tcp_connection::restore_fun(){
    if(check_directory(path_)){
        if(!JSON_client.get_child("data").empty() && check_validity(JSON_client.get_child("data"))){
            JSON_client.put("connection", "directory_valid");
            write_data(JSON_client);
        } else if(JSON_client.get_child("data").empty()){
            std::cout<<"CARTELLA NON PRESENTE SUL CLIENT"<<std::endl;
            JSON_server = JSON_client;
            JSON_server.put_child("data",JSON_utility::create_data_json(path_, 1));
            JSON_server.put("connection", "empty_data");
            write_data(JSON_server);
        } else {
            //DIFFERENZE
            std::cout<<"CARTELLA PRESENTE SUL CLIENT"<<std::endl;
            JSON_server.put("connection", "differences");
            boost::property_tree::write_json(std::cout, JSON_server);

            write_data(JSON_server);
        }
    } else {
        JSON_client.put("connection", "directory_error");
        write_data(JSON_client);
        //RILASCIARE RISORSE SOCKET
    }
}

void tcp_connection::check_validity_fun() {
    if (check_directory(path_)) {
        if (!JSON_client.get_child("data").empty() && check_validity(JSON_client.get_child("data"))) {
            JSON_client.put("connection", "directory_valid");
            write_data(JSON_client);
        } else {
            JSON_client.put("connection", "directory_invalid");
            write_data(JSON_client);
        }
    } else {
        JSON_client.put("connection", "directory_error");
        write_data(JSON_client);
        //RILASCIARE RISORSE SOCKET
    }
}

void tcp_connection::default_fun(){
    if(check_directory(path_)){
        //cartella esiste sul client
        JSON_server.erase("data");
        if(!JSON_client.get_child("data").empty() && check_validity(JSON_client.get_child("data"))){

            std::cout<<"Cartelle già allineate"<<std::endl;
            JSON_client.put("connection", "default_directory_valid");
            write_data(JSON_client);

        } else {
            std::cout<<"Cartelle esistente ma non allineate"<<std::endl;
            boost::property_tree::write_json(std::cout, JSON_client.get_child("data"));

            boost::property_tree::write_json(std::cout, JSON_server.get_child("data"));

            files.clear();
            files = JSON_utility::JSON_differences(JSON_client.get_child("data"), JSON_server.get_child("data"), username_, path_, files);
            if(!files.empty()){
                files[0].put("num_files", std::to_string(files.size()));
                files[0].put("index_file", "0");
                boost::property_tree::write_json(std::cout, files[0]);
                write_data(files[0]);
            } else {
                std::cout<<"Cartella aggiornata con successo"<<std::endl;
                JSON_client.put("connection", "default_directory_valid");
                write_data(JSON_client);
                //stop();
            }
        }
    } else {
        JSON_server = JSON_client;
        JSON_server.erase("data");
        JSON_server.put("data", "");
        std::cout<<"Cartelle non esistente"<<std::endl;
        boost::property_tree::write_json(std::cout, JSON_client);

        boost::property_tree::write_json(std::cout, JSON_server);


        boost::filesystem::create_directory(path_);

        files = JSON_utility::JSON_differences(JSON_client.get_child("data"), JSON_server.get_child("data"), username_, path_, files);


        if(!files.empty()){
            files[0].put("num_files", std::to_string(files.size()));
            files[0].put("index_file", "0");
            boost::property_tree::write_json(std::cout, files[0]);
            write_data(files[0]);
        } else {
            std::cout<<"Cartella aggiornata con successo"<<std::endl;
            JSON_client.put("connection", "default_directory_valid");
            write_data(JSON_client);
            //stop();
        }
    }

}

void tcp_connection::ask_file_fun(){
    std::string path = "";
    for(int i=0; i<JSON_client.get<int>("size"); i++){
        path += JSON_client.get<std::string>(std::to_string(i)) + "/";
    }
    path += JSON_client.get<std::string>("file_name");
    file_blocks = to_file_from_bytes(path);

    if(file_blocks.size() > 0){

        if(file_blocks.size() == 1) file_blocks[0].put("status", "last");
        JSON_client.put("connection","sending_file");
        file_blocks[0].put("num_blocks", std::to_string(file_blocks.size()));
        file_blocks[0].put("index_block", "0");
        JSON_client.add_child("block_info", file_blocks[0]);
        write_data(JSON_client);
    }
}

void tcp_connection::file_received_fun(){
    int index_block = JSON_client.get<int>("block_info.index_block") + 1;
    int num_blocks = JSON_client.get<int>("block_info.num_blocks");

    if (index_block + 1 == num_blocks) {
        //E' l'ultimo
        file_blocks[index_block].put("status", "last");
    }
    file_blocks[index_block].put("index_block", std::to_string(index_block));
    file_blocks[index_block].put("num_blocks", std::to_string(num_blocks));
    JSON_client.erase("block_info");
    JSON_client.add_child("block_info", file_blocks[index_block]);
    JSON_client.put("connection", "sending_file");

    write_data(JSON_client);
}

void tcp_connection::sending_file_fun(){
    std::string path_file = path_.string() + "/";


    for(int i=1; i<JSON_client.get<int>("size"); i++){
        std::string directoryToCheck = JSON_client.get<std::string>(std::to_string(i));


        path_file += JSON_client.get<std::string>(std::to_string(i)) + "/";

    }

    std::string data = JSON_client.get<std::string>("block_info.data");
    filesystem_utility::write_file(path_file + JSON_client.get<std::string>("file_name"), std::vector<unsigned char>(data.begin(), data.end()));
    if(JSON_client.get("block_info.status", "status_error") == "last"){
        int index_file = JSON_client.get<int>("index_file") +1;
        int num_files = JSON_client.get<int>("num_files");
        if(index_file == num_files){
            std::cout<<"CARTELLA AGGIORNATA CON SUCCESSO. . . . Chiusura socket. . . ."<<std::endl;
            JSON_client.put("connection", "default_directory_valid");
            write_data(JSON_client);

            //stop();
        } else{


            files[index_file].put("index_file", std::to_string(index_file));
            files[index_file].put("num_files", std::to_string(num_files));
            write_data(files[index_file]);
        }

    } else {
        //Scriviamo che abbiamo ricevuto l'ultimo.
        JSON_client.put("connection", "file_received");
        write_data(JSON_client);
    }
}