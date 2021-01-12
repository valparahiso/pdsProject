

#ifndef CLIENTPDS_FILESYSTEM_UTILITY_H
#define CLIENTPDS_FILESYSTEM_UTILITY_H


#include <string>
#include <vector>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "JSON_utility.h"


namespace filesystem_utility {
    void write_file(const std::string &path, std::vector<unsigned char> buffer);


    std::vector<boost::property_tree::ptree>
    create_file_system(boost::property_tree::ptree JSON_client, std::string path_server,
                       std::string path_client, std::vector<boost::property_tree::ptree> files_to_ask);

    std::vector<boost::property_tree::ptree> add_to(const boost::property_tree::ptree &JSON_client, boost::property_tree::ptree &JSON_server,
                                                    const std::string &path_server,
                                                    const std::string &path_client, std::vector<boost::property_tree::ptree> files_to_ask);

    void delete_from(const boost::property_tree::ptree &JSON_client, boost::property_tree::ptree &JSON_server,
                     const std::string &path_server);
}


#endif //CLIENTPDS_FILESYSTEM_UTILITY_H
