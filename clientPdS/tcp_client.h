#include <boost/asio.hpp>
#include "filesystem_utility.h"
#include "hash_utility.h"
#include "JSON_utility.h"
#include "file_watcher.h"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#define MAX_ATTEMPT 5

using boost::asio::steady_timer;
using boost::asio::ip::tcp;

class tcp_client
{
public:

    tcp_client(boost::asio::io_context& io_context, std::string username, std::string password, std::string directory, std::string command)
            : stopped_(false),
              socket_(io_context),
              deadline_(io_context),
              heartbeat_timer_(io_context),
              username_(username),
              password_(password),
              directory_(directory),
              path_(boost::filesystem::path(directory)),
              command_(command)
    {
        attempt_ = 0;
        if(command == "default"){
            fw = new FileWatcher(boost::filesystem::path(directory).string(), std::chrono::milliseconds(5000));
        }
    }

    // Called by the user of the user class to initiate the connection process.
    // The endpoints will have been obtained using a tcp::resolver.
    void start(tcp::resolver::results_type endpoints)
    {
        // Start the connect actor.
        endpoints_ = endpoints;
        start_connect(endpoints_.begin());

        // Start the deadline actor. You will note that we're not setting any
        // particular deadline here. Instead, the connect and input actors will
        // update the deadline prior to each asynchronous operation.
        deadline_.async_wait(boost::bind(&tcp_client::check_deadline, this));
    }

    // This function terminates all the actors to shut down the connection. It
    // may be called by the user of the user class, or by the class itself in
    // response to graceful termination or an unrecoverable error.
    void stop()
    {
        std::cout <<"CHIUSURA SOCKET... "<< std::endl;
        stopped_ = true;
        boost::system::error_code ignored_ec;
        socket_.close(ignored_ec);
        deadline_.cancel();
        heartbeat_timer_.cancel();
    }

private:
    void start_connect(tcp::resolver::results_type::iterator endpoint_iter)
    {
        if (endpoint_iter != endpoints_.end())
        {
            std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

            // Set a deadline for the connect operatiod\nn.
            deadline_.expires_after(boost::asio::chrono::seconds(60));

            // Start the asynchronous connect operation.
            socket_.async_connect(endpoint_iter->endpoint(),
                                  boost::bind(&tcp_client::handle_connect,
                                              this, _1, endpoint_iter));

        }
        else
        {
            // There are no more endpoints to try. Shut down the user.
            stop();
        }
    }

    void handle_connect(const boost::system::error_code& ec,
                        tcp::resolver::results_type::iterator endpoint_iter)
    {
        if (stopped_)
            return;

        // The async_connect() function automatically opens the socket at the start
        // of the asynchronous operation. If the socket is closed at this time then
        // the timeout handler must have run first.
        if (!socket_.is_open())
        {
            std::cout << "Connect timed out\n";

            // Try the next available endpoint.
            start_connect(++endpoint_iter);
        }

            // Check if the connect operation failed before the deadline expired.
        else if (ec)
        {
            std::cout << "Connect error: " << ec.message() << "\n";

            // We need to close the socket used in the previous connection attempt
            // before starting a new one.


            socket_.close();

            // Try the next available endpoint.
            std::cout<<"Server offline, tento una nuova connessione . . . "<<std::endl;
            std::cout<<"Connessione " << attempt_ << "/" << MAX_ATTEMPT <<std::endl;

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
        else
        {
            std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";
            JSON_client = JSON_utility::create_json(username_, password_, command_, path_);
            write_data(JSON_client);
        }
    }

    void read_data()
    {
        std::cout<<"READING DATA"<<std::endl;

        // Set a deadline for the read operation.
        deadline_.expires_after(boost::asio::chrono::seconds(30));

        std::cout<<"******************** In read DATA"<<std::endl;

        // Start an asynchronous operation to read a newline-delimited message.
        boost::asio::async_read_until(socket_,
                                      boost::asio::dynamic_buffer(input_buffer_), '/',
                                      boost::bind(&tcp_client::handle_read_data, this, _1, _2));


    }

    void handle_read_data(const boost::system::error_code& ec, std::size_t n)
    {
        if (stopped_)
            return;

        std::cout<<"EC: "<<ec<<std::endl;


        if (!ec)
        {
            std::cout<<"******************** In handle read DATA"<<std::endl;

            // Extract the newline-delimited message from the buffer.
            std::string line(input_buffer_.substr(0, n - 1));
            input_buffer_.erase(0, n);

            // Empty messages are heartbeats and so ignored.
            if (!line.empty())
            {
                std::istringstream ss(line);
                boost::property_tree::ptree JSON;
                read_json(ss, JSON);

                std::cout << "Received: " << line << "\n";
                if(JSON.get("connection", "connection_error") == "login_error"){
                    std::cout << "Autenticazione Fallita. Chiusura socket . . .  " << std::endl;
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
        else
        {
            std::perror("Error: ");
            std::cout << "Error on receive: " << ec.message() << "\n";

            stop();
        }
    }

    void write_data(boost::property_tree::ptree JSON)
    {
        std::cout<<"******************** In WRITE DATA"<<std::endl;

        std::ostringstream JSON_string;
        write_json(JSON_string, JSON);
        std::string data = JSON_string.str() + "/";
        std::cout<<"SENDING DATA TO SERVER "<<std::endl;
        if (stopped_)
            return;

        std::cout<<"MESSAGGIO: "<<data<<std::endl;
        // Start an asynchronous operation to send a heartbeat message.
        boost::asio::async_write(socket_, boost::asio::buffer(data),
                                 boost::bind(&tcp_client::handle_write_data, this, _1));
    }

    void handle_write_data(const boost::system::error_code& ec)
    {
        std::cout<<"******************** In handle WRITE DATA"<<std::endl;

        if (stopped_)
            return;

        if (!ec)
        {
            // Wait 10 seconds before sending the next heartbeat.
            //heartbeat_timer_.expires_after(boost::asio::chrono::seconds(1));

            heartbeat_timer_.async_wait(boost::bind(&tcp_client::read_data, this));

        }
        else
        {
            std::cout << "Error on heartbeat: " << ec.message() << "\n";

            stop();
        }
    }

    void check_deadline()
    {
        if (stopped_)
            return;

        // Check whether the deadline has passed. We compare the deadline against
        // the current time since a new asynchronous operation may have moved the
        // deadline before this actor had a chance to run.
        if (deadline_.expiry() <= steady_timer::clock_type::now())
        {
            // The deadline has passed. The socket is closed so that any outstanding
            // asynchronous operations are cancelled.
            std::cout << "deadline_ " << "\n";
            //socket_.close();

            // There is no longer an active deadline. The expiry is set to the
            // maximum time point so that the actor takes no action until a new
            // deadline is set.
            deadline_.expires_at(steady_timer::time_point::max());
        }

        // Put the actor back to sleep.
        deadline_.async_wait(boost::bind(&tcp_client::check_deadline, this));
    }

    std::vector<boost::property_tree::ptree> to_file_from_bytes(std::string path_file){

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

    std::vector<boost::property_tree::ptree> send_file(std::string path_file, int bytes_to_transfer, std::vector<boost::property_tree::ptree> file_blocks, int offset){

        std::cout<<"LEGGO DAL FILE PARTENDO DALL'INDICE "<<0<<" MANCANO DA LEGGERE " << bytes_to_transfer << " bytes" <<std::endl;
        std::ifstream input(path_file, std::ios::in | std::ios::binary);
        input.seekg (offset);


        boost::property_tree::ptree file_asked;
        int size = 1024;
        if(bytes_to_transfer >= 1024){
            bytes_to_transfer -= 1024;
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
        //std::cout<<"LEGGO DAL FILE PARTENDO DALL'INDICE "<<index<<" UNA SIZE DI " << buffer.size() << " bytes" <<std::endl;
        input.read((char*)&buffer[0], buffer.size());

        std::stringstream buffer_hex;
        file_asked.put("status", "");
        for(int i=0; i<buffer.size(); i++){
            buffer_hex << std::hex << std::setw(2) << std::setfill('0') << +buffer[i];
        }

        file_asked.put("data", std::string(buffer_hex.str()));

        file_blocks.push_back(file_asked);
        file_blocks = send_file(path_file, bytes_to_transfer, file_blocks, offset + 1024);
        return file_blocks;
    }

    void check_validity_fun(boost::property_tree::ptree& JSON){
        if(JSON.get("connection", "connection_error") == "directory_error"){
            std::cout << "Directory invalida. Directory NON presente sul server. Chiusura socket . . .  " << std::endl;
            stop();
        } else if(JSON.get("connection", "connection_error") == "directory_valid"){
            std::cout << "Directory VALIDA. Client e server aggiornati. . . . Chiusura socket . . .  " << std::endl;
            stop();
        } else if(JSON.get("connection", "connection_error") == "directory_invalid"){
            std::cout << "Directory INVALIDA. Client e server NON aggiornati. . . . Chiusura socket . . .  " << std::endl;
            stop();
        }
    }

    void restore_fun(boost::property_tree::ptree& JSON){
        if(JSON.get("connection", "connection_error") == "directory_error"){
            std::cout << "Impossibile esguire il restore della directory. Directory NON presente sul server. Chiusura socket . . .  " << std::endl;
            stop();
        }
        else if(JSON.get("connection", "connection_error") == "directory_valid"){
            std::cout << "Directory di client e server sono già aggiornate . . . . Chiusura socket . . .  " << std::endl;
            stop();
        } else if(JSON.get("connection", "connection_error") == "empty_data"){
            empty_data_fun(JSON);
        } else if(JSON.get("connection", "connection_error") == "sending_file"){
            sending_file_fun(JSON);
        } else if(JSON.get("connection", "connection_error") == "differences"){
            differences_fun(JSON);
        }
    }

    void empty_data_fun(boost::property_tree::ptree& JSON){
        files = filesystem_utility::create_file_system(JSON.get_child("data"), path_.string(), username_ + "/" + path_.filename().string(), files);
        if(!files.empty()){
            files[0].put("num_files", std::to_string(files.size()));
            files[0].put("index_file", "0");
            write_data(files[0]);
        }
    }

    boost::property_tree::ptree create_request(boost::property_tree::ptree& JSON, std::string path){
        files = filesystem_utility::create_file_system(JSON.get_child("data"), path, username_ + "/" + path, files);
        if(!files.empty()){
            files[0].put("num_files", std::to_string(files.size()));
            files[0].put("index_file", "0");
        }
        return files[0];
    }

    void sending_file_fun(boost::property_tree::ptree& JSON){
        std::string path_file = path_.string() + "/";
        for(int i=2; i<JSON.get<int>("size"); i++){
            path_file += JSON.get<std::string>(std::to_string(i)) + "/";
        }
        std::string data = JSON.get<std::string>("block_info.data");
        filesystem_utility::write_file(path_file + JSON.get<std::string>("file_name"), std::vector<unsigned char>(data.begin(), data.end()));
        if(JSON.get("block_info.status", "status_error") == "last"){
            int index_file = JSON.get<int>("index_file") +1;
            int num_files = JSON.get<int>("num_files");
            if(index_file == num_files){
                std::cout<<"CARTELLA AGGIORNATA CON SUCCESSO. . . . Chiusura socket. . . ."<<std::endl;
                stop();
            } else{
                files[index_file].put("index_file", std::to_string(index_file));
                files[index_file].put("num_files", std::to_string(num_files));
                write_data(files[index_file]);
            }

        } else {
            //Scriviamo che abbiamo ricevuto l'ultimo.
            JSON.put("connection", "file_received");
            write_data(JSON);
        }
    }

    void differences_fun(boost::property_tree::ptree& JSON){
        files = JSON_utility::JSON_differences(JSON.get_child("data"), JSON_client.get_child("data"), username_, path_, files);
        if(!files.empty()){
            files[0].put("num_files", std::to_string(files.size()));
            files[0].put("index_file", "0");
            write_data(files[0]);
        } else {
            std::cout<<"CARTELLA AGGIORNATA CON SUCCESSO. . . . Chiusura socket. . . ."<<std::endl;
            stop();
        }
    }

    void default_fun(boost::property_tree::ptree& JSON){
        if(JSON.get("connection", "connection_error") == "ask_file"){
            ask_file_fun(JSON,0);
        }
        else if(JSON.get("connection", "connection_error") == "file_received") {
            file_received_fun(JSON);
        }
        else if(JSON.get("connection", "connection_error") == "default_directory_valid") {
            std::cout<<"Allineati server e client!\nStarting watcher..."<<std::endl;
            start_watcher();
        }
    }

    void start_watcher(){
        JSON_client_old = JSON_client;
        std::vector<std::string> result;
        std::string correct_path = "";
        boost::split(result, directory_, boost::is_any_of("/"));
        std::string dir = result[result.size()-1];
        files.clear();
        file_blocks.clear();
        // Start monitoring a folder for changes and (in case of changes)
        // run a user provided lambda function
        int status = fw->start([dir, this] (std::string path_to_watch, FileStatus status) -> int {
            // Process only regular files, all other file types are ignored
            if(!std::experimental::filesystem::is_regular_file(std::experimental::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
                return 1;
            }
            int statusVal = -1;

            switch(status) {
                case FileStatus::created: {
                    statusVal = 1;

                    break;
                }
                case FileStatus::modified: {
                    std::cout << "File modified: " << path_to_watch << '\n';
                    statusVal = 1;
                    break;
                }
                case FileStatus::erased: {
                    std::cout << "File erased: " << path_to_watch << '\n';
                    statusVal = 1;
                    break;
                }
                default: {
                    std::cout << "Error! Unknown file status.\n";
                    statusVal = 4;
                }
            }

            return statusVal;
        });

        std::cout<<"Status value: " << status <<std::endl;

        if(status == 1){
            JSON_client = JSON_utility::create_json(username_, password_, "default", path_);
            write_data(JSON_client);
        }
        else{
            start_watcher();
        }
    }

    void ask_file_fun(boost::property_tree::ptree& JSON,int start){
        std::cout<<"SERVER CHIEDE UN FILE"<<std::endl;
        std::string path = path_.string() + "/";
        for(int i=1; i<JSON.get<int>("size"); i++){
            path += JSON.get<std::string>(std::to_string(i)) + "/";
        }
        path += JSON.get<std::string>("file_name");
        std::cout<<"PERCORSO FILE : " << path<<std::endl;
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

    void file_received_fun(boost::property_tree::ptree& JSON){
        std::cout << "SERVER CHIEDE UN ALTRO PEZZO DEL FILE" << std::endl;
        int index_block = JSON.get<int>("block_info.index_block") + 1;
        int num_blocks = JSON.get<int>("block_info.num_blocks");

        if (index_block + 1 == num_blocks) {
            //E' l'ultimo
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

private:
    bool stopped_;
    tcp::resolver::results_type endpoints_;
    tcp::socket socket_;
    std::string input_buffer_;
    steady_timer deadline_;
    steady_timer heartbeat_timer_;
    std::string username_;
    std::string password_;
    std::vector<boost::property_tree::ptree> files;
    boost::filesystem::path path_;
    std::string command_;
    std::string directory_;
    boost::property_tree::ptree JSON_client;
    boost::property_tree::ptree JSON_client_old;
    std::vector<boost::property_tree::ptree> file_blocks;
    FileWatcher* fw;
    int attempt_;

};
