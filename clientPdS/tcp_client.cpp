//
// Created by simo on 15/10/20.
//

#include "tcp_client.h"


tcp_client::tcp_client(boost::asio::io_context& io_context, std::string username, std::string password, std::string directory, std::string command)
    : stopped_(false),
    socket_(io_context),
    username_(username),
    password_(password),
    path_(boost::filesystem::path(directory)),
    command_(command)
{
    attempt_ = 0;
    if(command == "default"){
        fw = new FileWatcher(boost::filesystem::path(directory).string(), std::chrono::milliseconds(5000));
    }
}

void tcp_client::start(tcp::resolver::results_type endpoints){
    // Start the connect actor.
    endpoints_ = endpoints;
    start_connect(endpoints_.begin());
}

void tcp_client::stop(){
    std::cout <<"\nChiusura socket"<< std::endl;
    stopped_ = true;
    boost::system::error_code ignored_ec;
    socket_.close(ignored_ec);
}

void tcp_client::start_connect(tcp::resolver::results_type::iterator endpoint_iter){
    if (endpoint_iter != endpoints_.end()){
        std::cout << "Trying " << endpoint_iter->endpoint() << "\n";

        // Start the asynchronous connect operation.
        socket_.async_connect(endpoint_iter->endpoint(),
                              boost::bind(&tcp_client::handle_connect,
                                          this, _1, endpoint_iter));

    } else{
        // There are no more endpoints to try. Shut down the user.
        stop();
    }
}

void tcp_client::handle_connect(const boost::system::error_code& ec,tcp::resolver::results_type::iterator endpoint_iter){
    if (stopped_)
        return;

    /*if (!socket_.is_open()){
        std::cout << "Connect timed out\n";

        // Retry the endpoint.
        start_connect(endpoint_iter);
    }*/

    if (ec){
        std::cout << "Connection error"<< std::endl;

        std::cout<<"Server offline, tento una nuova connessione . . . "<<std::endl;
        std::cout<<"Connessione " << attempt_ << "/" << MAX_ATTEMPT <<"\n"<<std::endl;

        if(attempt_ < MAX_ATTEMPT) {
            attempt_++;
            sleep(5);
            start_connect(endpoint_iter);
        }
        else{
            std::cout<<"Numero massimo di tentativi raggiunto..."<<std::endl;
            stop();
        }
    }
    // Otherwise we have successfully established a connection.
    else{
        attempt_=0;
        std::cout << "Connected to " << endpoint_iter->endpoint() << "\n\n";
        JSON_client = JSON_utility::create_json(username_, password_, command_, path_);
        write_data(JSON_client);
    }
}

void tcp_client::read_data(){

    // Start an asynchronous operation to read a newline-delimited message.
    boost::asio::async_read_until(socket_,
                                  boost::asio::dynamic_buffer(input_buffer_), '/',
                                  boost::bind(&tcp_client::handle_read_data, this, _1, _2));


}

void tcp_client::handle_read_data(const boost::system::error_code& ec, std::size_t n){
    if (stopped_)
        return;

    if (!ec){
        // Extract the newline-delimited message from the buffer.
        std::string line(input_buffer_.substr(0, n - 1));
        input_buffer_.erase(0, n);

        if (!line.empty()){
            std::istringstream ss(line);
            boost::property_tree::ptree JSON;
            read_json(ss, JSON);

            if(JSON.get("connection", "connection_error") == "login_error"){
                std::cout << "Autenticazione Fallita" << std::endl;
                stop();
            } else if(command_ == "check_validity"){
                check_validity_fun(JSON);
            } else if(command_ == "restore"){
                restore_fun(JSON);
            } else if(command_ == "default"){
                default_fun(JSON);
            }
        }
    }
    else{

        if(n==0){
            //Retry connection
            std::cout << "Server down, reconnection" << "\n";
            start(endpoints_);
        }
        else{
            std::perror("Error: ");
            std::cout << "Error on receive: " << ec.message() << "\n";
            stop();

        }

    }
}


void tcp_client::write_data(boost::property_tree::ptree JSON){

    std::ostringstream JSON_string;
    write_json(JSON_string, JSON);
    std::string data = JSON_string.str() + "/";
    if (stopped_)
        return;

    // Start an asynchronous operation to send a heartbeat message.
    boost::asio::async_write(socket_, boost::asio::buffer(data),
                             boost::bind(&tcp_client::handle_write_data, this, _1));
}

void tcp_client::handle_write_data(const boost::system::error_code& ec){

    if (stopped_)
        return;

    if (!ec){
        read_data();
    }
    else{
        std::cout << "Error: " << ec.message() << "\n";
        stop();
    }
}

std::vector<boost::property_tree::ptree> tcp_client::to_file_from_bytes(std::string path_file){

    std::vector<boost::property_tree::ptree> file_blocks;
    std::ifstream size(path_file, std::ios::ate | std::ios::binary);
    int file_size = size.tellg();
    size.close();
    if(file_size == -1){
        return file_blocks;
    }

    file_blocks = send_file(path_file, file_size, file_blocks, 0);
    return file_blocks;
}

std::vector<boost::property_tree::ptree> tcp_client::send_file(std::string path_file, int bytes_to_transfer, std::vector<boost::property_tree::ptree> file_blocks, int offset){

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

void tcp_client::check_validity_fun(boost::property_tree::ptree& JSON){
    if(JSON.get("connection", "connection_error") == "directory_error"){
        std::cout << "Directory invalida\nDirectory non presente sul server" << std::endl;
        stop();
    } else if(JSON.get("connection", "connection_error") == "directory_valid"){
        std::cout << "Directory valida\nClient e server aggiornati" << std::endl;
        stop();
    } else if(JSON.get("connection", "connection_error") == "directory_invalid"){
        std::cout << "Directory invalida\nClient e server non aggiornati" << std::endl;
        stop();
    }
}

void tcp_client::restore_fun(boost::property_tree::ptree& JSON){
    if(JSON.get("connection", "connection_error") == "directory_error"){
        std::cout << "Impossibile esguire il restore della directory. Directory non presente sul server" << std::endl;
        stop();
    }
    else if(JSON.get("connection", "connection_error") == "directory_valid"){
        std::cout << "Directory di client e server sono già aggiornate" << std::endl;
        stop();
    } else if(JSON.get("connection", "connection_error") == "empty_data"){
        empty_data_fun(JSON);
    } else if(JSON.get("connection", "connection_error") == "sending_file"){
        sending_file_fun(JSON);
    } else if(JSON.get("connection", "connection_error") == "differences"){
        differences_fun(JSON);
    }
}


void tcp_client::empty_data_fun(boost::property_tree::ptree& JSON){
    files = filesystem_utility::create_file_system(JSON.get_child("data"), path_.string(), username_ + "/" + path_.filename().string(), files);
    if(!files.empty()){
        files[0].put("num_files", std::to_string(files.size()));
        files[0].put("index_file", "0");
        write_data(files[0]);
    }
}

void tcp_client::sending_file_fun(boost::property_tree::ptree& JSON){
    std::string path_file = path_.string() + "/";
    for(int i=2; i<JSON.get<int>("size"); i++){
        path_file += JSON.get<std::string>(std::to_string(i)) + "/";
    }
    std::string data = JSON.get<std::string>("block_info.data");
    filesystem_utility::write_file(path_file + JSON.get<std::string>("file_name"), std::vector<unsigned char>(data.begin(), data.end()));
    std::cout<<"\nTrasferimento file in corso...\n"<<std::endl;

    if(JSON.get("block_info.status", "status_error") == "last"){
        //ultimo blocco di un file
        int index_file = JSON.get<int>("index_file") +1;
        int num_files = JSON.get<int>("num_files");
        if(index_file == num_files){
            //ultimo file da ricevere
            std::cout<<"Cartella aggiornata con successo"<<std::endl;
            stop();
        } else{
            files[index_file].put("index_file", std::to_string(index_file));
            files[index_file].put("num_files", std::to_string(num_files));
            write_data(files[index_file]);
        }

    } else {
        //Scriviamo che abbiamo ricevuto un blocco del file (non l'ultimo)
        JSON.put("connection", "file_received");
        write_data(JSON);
    }
}

void tcp_client::differences_fun(boost::property_tree::ptree& JSON){
    files = JSON_utility::JSON_differences(JSON.get_child("data"), JSON_client.get_child("data"), username_, path_, files);
    if(!files.empty()){
        files[0].put("num_files", std::to_string(files.size()));
        files[0].put("index_file", "0");
        write_data(files[0]);
    } else {
        std::cout<<"Cartella aggiornata con successo"<<std::endl;
        stop();
    }
}

void tcp_client::default_fun(boost::property_tree::ptree& JSON){
    if(JSON.get("connection", "connection_error") == "ask_file"){
        ask_file_fun(JSON,0);
    }
    else if(JSON.get("connection", "connection_error") == "file_received") {
        file_received_fun(JSON);
    }
    else if(JSON.get("connection", "connection_error") == "default_directory_valid") {
        std::cout<<"Cartelle aggiornate, starting file watcher"<<std::endl;
        start_watcher();
    }
}

void tcp_client::start_watcher(){
    JSON_client_old = JSON_client;
    std::vector<std::string> result;
    std::string correct_path = "";
    std::string dir = path_.filename().string();
    files.clear();
    file_blocks.clear();
    // Start monitoring a folder for changes and (in case of changes)
    // run a user provided lambda function
    int status = fw->start([dir, this] (std::string path_to_watch, FileStatus status) -> int {
        // Process only regular files, all other file types are ignored
        if(!std::experimental::filesystem::is_regular_file(std::experimental::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
            std::cout<<"Creazione\n"<<std::endl;
            return 1;
        }
        int statusVal = -1;

        switch(status) {
            case FileStatus::created: {
                std::cout<<"Creazione\n"<<std::endl;
                statusVal = 1;

                break;
            }
            case FileStatus::modified: {
                std::cout<<"Modifica\n"<<std::endl;
                statusVal = 1;
                break;
            }
            case FileStatus::erased: {
                std::cout<<"Eliminazione\n"<<std::endl;
                statusVal = 1;
                break;
            }
            default: {
                statusVal = 4;
            }
        }

        return statusVal;
    });


    if(status == 1){
        //C'è stata una modifica/creazione/eliminazione, uso JSON default
        JSON_client = JSON_utility::create_json(username_, password_, "default", path_);
        write_data(JSON_client);
    }
    else{
        start_watcher();
    }
}

void tcp_client::ask_file_fun(boost::property_tree::ptree& JSON,int start){

    std::string path = path_.string() + "/";
    for(int i=1; i<JSON.get<int>("size"); i++){
        path += JSON.get<std::string>(std::to_string(i)) + "/";
    }
    path += JSON.get<std::string>("file_name");

    file_blocks = to_file_from_bytes(path);

    if(file_blocks.size() > 0){

        if(file_blocks.size() == 1) file_blocks[0].put("status", "last");
        JSON.put("connection","sending_file");
        file_blocks[0].put("num_blocks", std::to_string(file_blocks.size()));
        file_blocks[0].put("index_block", "0");
        JSON.add_child("block_info", file_blocks[0]);
        write_data(JSON);
    }
}


void tcp_client::file_received_fun(boost::property_tree::ptree& JSON){

    int index_block = JSON.get<int>("block_info.index_block") + 1;
    int num_blocks = JSON.get<int>("block_info.num_blocks");

    if (index_block + 1 == num_blocks) {
        //E' l'ultimo
        file_blocks[index_block].put("status", "last");
    }
    file_blocks[index_block].put("index_block", std::to_string(index_block));
    file_blocks[index_block].put("num_blocks", std::to_string(num_blocks));
    JSON.erase("block_info");
    JSON.add_child("block_info", file_blocks[index_block]);
    JSON.put("connection", "sending_file");

    write_data(JSON);
}
