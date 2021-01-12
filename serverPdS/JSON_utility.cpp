//
// Created by simo on 09/11/20.
//
#include "JSON_utility.h"

void JSON_utility::print_JSON(const boost::property_tree::ptree& JSON){
    boost::property_tree::ptree JSON2 = JSON;

    if(JSON.get("connection", "")=="sending_file" || JSON.get("connection", "")=="file_received" || JSON.get("connection", "")=="default_directory_valid"){
        JSON2.put("block_info.data", "Bytes del file in hex");
    }

    boost::property_tree::write_json(std::cout, JSON2);
    std::cout<<""<<std::endl;
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
        JSON.add_child("data", JSON_utility::create_data_json(path_, 0));
    } else JSON.put("data", "");


    return JSON;

}

//option != 0 -> NO HASH
/*
 * L'opzione option != 0 è stata introdotta per evitare che nel caso il client chieda una restore con la cartella vuota vengano calcolati inutilmente
 * gli hash di tutti i file del server che sicuramente mancano sul client.
 */
//path: percorso da cui partire per calcolare il JSON.
boost::property_tree::ptree JSON_utility::create_data_json(const boost::filesystem::path& path, int option){

        boost::property_tree::ptree directory;

        for (auto& entry : boost::filesystem::directory_iterator(path)){
            if(is_regular_file(entry.path())){
                if(option==0)
                    directory.put(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), hash_utility::calculate_hash(entry.path().string()));
                else
                    directory.put(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), "X");
            } else {
                directory.add_child(boost::property_tree::ptree::path_type(entry.path().filename().string(), '/'), create_data_json(entry.path(), option));
            }
        }
        directory.sort();
        return directory;
}

std::vector<boost::property_tree::ptree> JSON_utility::JSON_differences(const boost::property_tree::ptree& JSON_destination, boost::property_tree::ptree JSON_source,
                                                                        const std::string& username_, const boost::filesystem::path& path_, std::vector<boost::property_tree::ptree> files_to_ask){

    filesystem_utility::delete_from(JSON_destination, JSON_source, path_.string());

    return filesystem_utility::add_to(JSON_destination, JSON_source, path_.string(), path_.filename().string(), files_to_ask);
}