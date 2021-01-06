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

    tcp_client(boost::asio::io_context& io_context, std::string username, std::string password, std::string directory, std::string command);

    // Called by the user of the user class to initiate the connection process.
    // The endpoints will have been obtained using a tcp::resolver.
    void start(tcp::resolver::results_type endpoints);

    // This function terminates all the actors to shut down the connection. It
    // may be called by the user of the user class, or by the class itself in
    // response to graceful termination or an unrecoverable error.
    void stop();

private:
    void start_connect(tcp::resolver::results_type::iterator endpoint_iter);

    void handle_connect(const boost::system::error_code& ec,tcp::resolver::results_type::iterator endpoint_iter);

    void read_data();

    void handle_read_data(const boost::system::error_code& ec, std::size_t n);

    void write_data(boost::property_tree::ptree JSON);

    void handle_write_data(const boost::system::error_code& ec);

    void check_deadline();

    std::vector<boost::property_tree::ptree> to_file_from_bytes(std::string path_file);

    std::vector<boost::property_tree::ptree> send_file(std::string path_file, int bytes_to_transfer, std::vector<boost::property_tree::ptree> file_blocks, int offset);

    void check_validity_fun(boost::property_tree::ptree& JSON);

    void restore_fun(boost::property_tree::ptree& JSON);

    void empty_data_fun(boost::property_tree::ptree& JSON);

    boost::property_tree::ptree create_request(boost::property_tree::ptree& JSON, std::string path);

    void sending_file_fun(boost::property_tree::ptree& JSON);

    void differences_fun(boost::property_tree::ptree& JSON);

    void default_fun(boost::property_tree::ptree& JSON);

    void start_watcher();

    void ask_file_fun(boost::property_tree::ptree& JSON,int start);

    void file_received_fun(boost::property_tree::ptree& JSON);

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
