#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <openssl/md5.h>
#include <sys/mman.h>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>


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
              path_(boost::filesystem::path(directory)),
              command_(command)
    {


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
            start_connect(++endpoint_iter);
        }

            // Otherwise we have successfully established a connection.
        else
        {
            std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";

            write_data(create_json());
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

                } else if(command_ == "restore"){
                    if(JSON.get("connection", "connection_error") == "directory_error"){
                        std::cout << "Impossibile esguire il restore della directory. Directory NON presente sul server. Chiusura socket . . .  " << std::endl;
                        stop();
                    }
                    else if(JSON.get("connection", "connection_error") == "directory_valid"){
                        std::cout << "Directory di client e server sono già aggiornate . . . . Chiusura socket . . .  " << std::endl;
                        stop();
                    } else if(JSON.get("connection", "connection_error") == "empty_data"){
                        std::cout<<"SONO DENTRO "<<std::endl;
                        files = create_file_system(JSON.get_child("data"), path_.string(), username_ + "/" + path_.filename().string(), files);
                        if(!files.empty()){
                            files[0].put("num_files", std::to_string(files.size()));
                            files[0].put("index_file", "0");
                            write_data(files[0]);
                        }
                    } else if(JSON.get("connection", "connection_error") == "sending_file"){
                        std::string path_file = path_.string() + "/";
                        for(int i=2; i<JSON.get<int>("size"); i++){
                            path_file += JSON.get<std::string>(std::to_string(i)) + "/";
                        }
                        std::string data = JSON.get<std::string>("block_info.data");
                        write_file(path_file + JSON.get<std::string>("file_name"), std::vector<unsigned char>(data.begin(), data.end()));
                        if(JSON.get("block_info.status", "status_error") == "last"){
                            std::cout<<"FILE RICEVUTO "<<std::endl;
                            int index_file = JSON.get<int>("index_file") +1;
                            int num_files = JSON.get<int>("num_files");
                            if(index_file == num_files){
                                //ABBIAMO INVIATO TUTTI I FILE.
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

                } else if(command_ == "default"){

                }
            }
        }
        else
        {
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
            std::cout<<"ARRIVO CORRETTAMENTE"<<std::endl;
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
            socket_.close();

            // There is no longer an active deadline. The expiry is set to the
            // maximum time point so that the actor takes no action until a new
            // deadline is set.
            deadline_.expires_at(steady_timer::time_point::max());
        }

        // Put the actor back to sleep.
        deadline_.async_wait(boost::bind(&tcp_client::check_deadline, this));
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

    boost::property_tree::ptree create_json(){

        boost::property_tree::ptree JSON;
        boost::property_tree::ptree login;
        boost::property_tree::ptree dir_and_command;
        JSON.put("connection", "login");

        login.put("username", username_);
        login.put("password", calculate_password_hash());
        std::cout<<"PASSWORD HASH :      "<<calculate_password_hash()<<std::endl;

        dir_and_command.put("directory", path_.filename().string());
        dir_and_command.put("command", command_);

        JSON.add_child("login", login);
        JSON.add_child("dir_and_command", dir_and_command);

        if(exists(path_) && is_directory(path_)){
            JSON.add_child("data", create_data_json(path_));
        } else JSON.put("data", "");

        boost::property_tree::write_json(std::cout, JSON);
        return JSON;

    }

    std::string calculate_password_hash(){
        std::ostringstream out;
        const char* password = password_.c_str();
        MD5_CTX md5;
        MD5_Init (&md5);
        MD5_Update (&md5, (const unsigned char *) password, password_.length());
        unsigned char buffer_md5[MD5_DIGEST_LENGTH];
        MD5_Final ( buffer_md5, &md5);
        for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
            out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +buffer_md5[i]; //The unary "+" performs an integral promotion to int.
            //https://stackoverflow.com/questions/42902594/stdhex-does-not-work-as-i-expect
        }
        return out.str();
    }


    boost::property_tree::ptree create_data_json(boost::filesystem::path path){

        boost::property_tree::ptree directory;
        for (auto& entry : boost::filesystem::directory_iterator(path)){
            if(is_regular_file(entry.path())){
                directory.put(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), calculate_hash(entry.path().string()));
            } else {
                directory.add_child(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), create_data_json(entry.path()));
            }
        }
        return directory;

    }

    // Get the size of the file by its file descriptor
    static unsigned long get_size_by_fd(int fd) {
        struct stat statbuf;
        if(fstat(fd, &statbuf) < 0) exit(-1); //uscita controllata TODO
        return statbuf.st_size;
    }

    static std::string calculate_hash(std::string file_path){
        unsigned char result[MD5_DIGEST_LENGTH];
        int file_descriptor;
        unsigned long file_size;
        char* file_buffer;
        std::ostringstream out;

        file_descriptor = open(file_path.c_str(), O_RDONLY);
        if(file_descriptor < 0) exit(-1); //uscita controllata TODO

        file_size = get_size_by_fd(file_descriptor);

        file_buffer = static_cast<char *>(mmap(0, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0));
        MD5((unsigned char*) file_buffer, file_size, result);
        munmap(file_buffer, file_size);
            for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
               out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +result[i]; //The unary "+" performs an integral promotion to int.
                //https://stackoverflow.com/questions/42902594/stdhex-does-not-work-as-i-expect
            }

       return out.str();

    }

    std::vector<boost::property_tree::ptree> create_file_system(boost::property_tree::ptree JSON, std::string path_client, std::string path_server, std::vector<boost::property_tree::ptree> files){
        boost::filesystem::create_directory(path_client);
        std::cout<<"PERCORSO CLIENT: "<<path_client<<std::endl;
        std::cout<<"PERCORSO SERVER: "<<path_server<<std::endl;
        for(auto tree : JSON){
            if(tree.second.data() == "X"){
                //FILE
                std::cout<<"SONO NEL FILE"<<std::endl;
                boost::property_tree::ptree ask_file;
                ask_file.put("connection", "ask_file");
                std::vector<std::string> string_split;
                boost::algorithm::split(string_split, path_server, boost::is_any_of("/"));
                ask_file.put("size", std::to_string(string_split.size()));
                for(int i=0; i<string_split.size(); i++){
                    ask_file.put(std::to_string(i), string_split[i]);
                }
                ask_file.put("file_name", tree.first);

                files.push_back(ask_file);
            } else {
                //CARTELLA
                files = create_file_system(JSON.get_child(boost::property_tree::ptree::path_type(tree.first, '/')), path_client + "/" + tree.first, path_server + "/" + tree.first, files);
            }
        }
        return files;

    }


    void write_file(std::string path, std::vector<unsigned char> buffer){


        std::basic_string<uint8_t> bytes;
        std::string hex = std::string(buffer.begin(), buffer.end());

        for (size_t i = 0; i < hex.length(); i += 2) {
            uint16_t byte;
            std::string nextbyte = hex.substr(i, 2);
            std::istringstream(nextbyte) >> std::hex >> byte;
            bytes.push_back(static_cast<uint8_t>(byte));
        }

        std::string result(begin(bytes), end(bytes));

        std::vector<unsigned char> my_buff = std::vector<unsigned char>( result.begin(), result.end());
        std::ofstream output_file(path, std::ios::binary | std::ios::out | std::fstream::app);

        output_file.write((char*) &my_buff[0], my_buff.size());
        output_file.close();

    }
};
