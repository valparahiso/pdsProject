

#include "filesystem_utility.h"

void filesystem_utility::write_file(const std::string &path, std::vector<unsigned char> buffer) {

    std::basic_string<uint8_t> bytes;
    std::string hex = std::string(buffer.begin(), buffer.end());

    for (size_t i = 0; i < hex.length(); i += 2) {
        uint16_t byte;
        std::string next_byte = hex.substr(i, 2);
        std::istringstream(next_byte) >> std::hex >> byte;
        bytes.push_back(static_cast<uint8_t>(byte));
    }

    std::string result(begin(bytes), end(bytes));

    std::vector<unsigned char> my_buff = std::vector<unsigned char>(result.begin(), result.end());
    std::ofstream output_file(path, std::ios::binary | std::ios::out | std::fstream::app); //binario, scrittura, append

    output_file.write((char *) &my_buff[0], my_buff.size());
    output_file.close();

}

std::vector<boost::property_tree::ptree>
filesystem_utility::create_file_system(boost::property_tree::ptree JSON_server, std::string path_client,
                   std::string path_server, std::vector<boost::property_tree::ptree> files_to_ask) {
    boost::filesystem::create_directory(path_client);

    for (auto tree : JSON_server) {
        if (tree.second.data() != "") {
            //FILE
            boost::property_tree::ptree ask_file;
            ask_file.put("connection", "ask_file");
            std::vector<std::string> string_split;
            boost::algorithm::split(string_split, path_server, boost::is_any_of("/"));
            ask_file.put("size", std::to_string(string_split.size()));
            for (int i = 0; i < string_split.size(); i++) {
                ask_file.put(std::to_string(i), string_split[i]);
            }
            ask_file.put("file_name", tree.first);

            files_to_ask.push_back(ask_file);
        } else {
            //CARTELLA
            files_to_ask = create_file_system(
                    JSON_server.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                    path_client + "/" + tree.first, path_server + "/" + tree.first, files_to_ask);
        }
    }
    return files_to_ask;

}

std::vector<boost::property_tree::ptree> filesystem_utility::add_to(const boost::property_tree::ptree &JSON_server,
                                                                    boost::property_tree::ptree &JSON_client, const std::string &path_client,
                                                                    const std::string &path_server, std::vector<boost::property_tree::ptree> files_to_ask) {
    for (auto tree : JSON_server) {
        if (tree.second.data() == "") {   //DIRECTORY

            if (JSON_client.count(tree.first) == 0) {
                files_to_ask = create_file_system(
                        JSON_server.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                        path_client + "/" + tree.first,
                        path_server + "/" + tree.first, files_to_ask);
                JSON_client.add_child(tree.first, JSON_server.get_child(
                        boost::property_tree::ptree::path_type(tree.first, '/')));
            } else {
                files_to_ask = add_to(JSON_server.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                                      JSON_client.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                                      path_client + "/" + tree.first, path_server + "/" + tree.first, files_to_ask);
            }
        } else {
            if (JSON_client.count(tree.first) == 0) {
                boost::property_tree::ptree file_JSON;
                file_JSON.put(boost::property_tree::ptree::path_type(tree.first, '/'), "X");
                files_to_ask = create_file_system(file_JSON, path_client, path_server, files_to_ask);
                JSON_client.put(boost::property_tree::ptree::path_type(tree.first, '/'), tree.second.data());
            } else if (JSON_client.get<std::string>(boost::property_tree::ptree::path_type(tree.first, '/')) !=
                       tree.second.data()) {
                //DELETE DEL FILE

                boost::filesystem::remove(boost::filesystem::path(path_client + "/" + tree.first));
                boost::property_tree::ptree file_JSON;
                file_JSON.put(boost::property_tree::ptree::path_type(tree.first, '/'), "X");
                files_to_ask = create_file_system(file_JSON, path_client, path_server, files_to_ask);
                JSON_client.add(tree.first, tree.second.data());
            }
        }
    }
    return files_to_ask;
}


void filesystem_utility::delete_from(const boost::property_tree::ptree &JSON_server, boost::property_tree::ptree &JSON_client,
                 const std::string &path_client) {

    for (auto tree : JSON_client) {

        if (tree.second.data() == "") {   //DIRECTORY
            if (JSON_server.count(tree.first) == 0) {
                boost::filesystem::remove_all(boost::filesystem::path(path_client + "/" + tree.first));

                JSON_client.erase(tree.first);

            } else {
                //CARTELLA ESISTE SUL CLIENT
                delete_from(JSON_server.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                            JSON_client.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                            path_client + "/" + tree.first);
            }
        } else {
            if (JSON_server.count(tree.first) == 0) {
                boost::filesystem::remove(boost::filesystem::path(path_client + "/" + tree.first));
            }
        }
    }
}
