//
// Created by simo on 09/11/20.
//

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
    std::ofstream output_file(path, std::ios::binary | std::ios::out | std::fstream::app);

    output_file.write((char *) &my_buff[0], my_buff.size());
    output_file.close();

}

std::vector<boost::property_tree::ptree>
filesystem_utility::create_file_system(boost::property_tree::ptree JSON_destination, std::string path_source,
                   std::string path_destination, std::vector<boost::property_tree::ptree> files_to_ask) {
    boost::filesystem::create_directory(path_source);

    for (auto tree : JSON_destination) {
        if (tree.second.data() != "") {
            //FILE
            boost::property_tree::ptree ask_file;
            ask_file.put("connection", "ask_file");
            std::vector<std::string> string_split;
            boost::algorithm::split(string_split, path_destination, boost::is_any_of("/"));
            ask_file.put("size", std::to_string(string_split.size()));
            for (int i = 0; i < string_split.size(); i++) {
                ask_file.put(std::to_string(i), string_split[i]);
            }
            ask_file.put("file_name", tree.first);

            files_to_ask.push_back(ask_file);
        } else {
            //CARTELLA
            files_to_ask = create_file_system(
                    JSON_destination.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                    path_source + "/" + tree.first, path_destination + "/" + tree.first, files_to_ask);
        }
    }
    return files_to_ask;

}

std::vector<boost::property_tree::ptree> filesystem_utility::add_to(const boost::property_tree::ptree &JSON_destination,
                                                                    boost::property_tree::ptree &JSON_source, const std::string &path_source,
                                                                    const std::string &path_destination, std::vector<boost::property_tree::ptree> files_to_ask) {
    for (auto tree : JSON_destination) {
        if (tree.second.data() == "") {   //DIRECTORY

            if (JSON_source.count(tree.first) == 0) {
                files_to_ask = create_file_system(
                        JSON_destination.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                        path_source + "/" + tree.first,
                        path_destination + "/" + tree.first, files_to_ask);
                JSON_source.add_child(tree.first, JSON_destination.get_child(
                        boost::property_tree::ptree::path_type(tree.first, '/')));
            } else {
                files_to_ask = add_to(JSON_destination.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                       JSON_source.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                       path_source + "/" + tree.first, path_destination + "/" + tree.first, files_to_ask);
            }
        } else {
            if (JSON_source.count(tree.first) == 0) {
                boost::property_tree::ptree file_JSON;
                file_JSON.put(boost::property_tree::ptree::path_type(tree.first, '/'), "X");
                files_to_ask = create_file_system(file_JSON, path_source, path_destination, files_to_ask);
                JSON_source.put(boost::property_tree::ptree::path_type(tree.first, '/'), tree.second.data());
            } else if (JSON_source.get<std::string>(boost::property_tree::ptree::path_type(tree.first, '/')) !=
                       tree.second.data()) {
                //DELETE DEL FILE

                boost::filesystem::remove(boost::filesystem::path(path_source + "/" + tree.first));
                boost::property_tree::ptree file_JSON;
                file_JSON.put(boost::property_tree::ptree::path_type(tree.first, '/'), "X");
                files_to_ask = create_file_system(file_JSON, path_source, path_destination, files_to_ask);
                JSON_source.add(tree.first, tree.second.data());
            }
        }
    }
    return files_to_ask;
}


void filesystem_utility::delete_from(const boost::property_tree::ptree &JSON_destination, boost::property_tree::ptree &JSON_source,
                 const std::string &path_source) {

    for (auto tree : JSON_source) {

        if (tree.second.data() == "") {   //DIRECTORY
            if (JSON_destination.count(tree.first) == 0) {
                boost::filesystem::remove_all(boost::filesystem::path(path_source + "/" + tree.first));

                JSON_source.erase(tree.first);

            } else {
                //CARTELLA ESISTE SUL CLIENT
                delete_from(JSON_destination.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                            JSON_source.get_child(boost::property_tree::ptree::path_type(tree.first, '/')),
                            path_source + "/" + tree.first);
            }
        } else {
            if (JSON_destination.count(tree.first) == 0) {
                boost::filesystem::remove(boost::filesystem::path(path_source + "/" + tree.first));
            }
        }
    }
}
