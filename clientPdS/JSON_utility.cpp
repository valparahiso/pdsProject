
#include "JSON_utility.h"

void JSON_utility::print_JSON(const boost::property_tree::ptree& JSON){
    boost::property_tree::write_json(std::cout, JSON);
}

boost::property_tree::ptree JSON_utility::create_json(const std::string& username_, std::string password_, const std::string& command_, const boost::filesystem::path& path_){

    boost::property_tree::ptree JSON;
    boost::property_tree::ptree login;
    boost::property_tree::ptree dir_and_command;
    JSON.put("connection", "login");

    login.put("username", username_);
    login.put("password", hash_utility::calculate_password_hash(password_));

    dir_and_command.put("directory", path_.filename().string());
    dir_and_command.put("command", command_);

    JSON.add_child("login", login);
    JSON.add_child("dir_and_command", dir_and_command);

    if(exists(path_) && is_directory(path_)){
        JSON.add_child("data", JSON_utility::create_data_json(path_));
    } else JSON.put("data", "");

    return JSON;

}

boost::property_tree::ptree JSON_utility::create_data_json(const boost::filesystem::path& path){
    boost::property_tree::ptree directory;
    for (auto& entry : boost::filesystem::directory_iterator(path)){
        if(is_regular_file(entry.path())){
            directory.put(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), hash_utility::calculate_hash(entry.path().string()));
        } else {
            directory.add_child(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), JSON_utility::create_data_json(entry.path()));
        }
    }
    directory.sort();
    return directory;
}

std::vector<boost::property_tree::ptree> JSON_utility::JSON_differences(const boost::property_tree::ptree& JSON_server, boost::property_tree::ptree JSON_client,
                      const std::string& username_, const boost::filesystem::path& path_, std::vector<boost::property_tree::ptree> files_to_ask){
    filesystem_utility::delete_from(JSON_server, JSON_client, path_.string());
    return filesystem_utility::add_to(JSON_server, JSON_client, path_.string(), username_ + "/" + path_.filename().string(), files_to_ask);
}