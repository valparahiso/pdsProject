//
// Created by simo on 10/11/20.
//

#ifndef CLIENTPDS_FILESYSTEM_UTILITY_H
#define CLIENTPDS_FILESYSTEM_UTILITY_H


#include <string>
#include <vector>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


namespace filesystem_utility {
    void write_file(const std::string &path, std::vector<unsigned char> buffer);


    std::vector<boost::property_tree::ptree>
    create_file_system(boost::property_tree::ptree JSON_destination, std::string path_source,
                       std::string path_destination, std::vector<boost::property_tree::ptree> files_to_ask);

    std::vector<boost::property_tree::ptree> add_to(const boost::property_tree::ptree &JSON_destination, boost::property_tree::ptree &JSON_source,
                const std::string &path_source,
                const std::string &path_destination, std::vector<boost::property_tree::ptree> files_to_ask);

    void delete_from(const boost::property_tree::ptree &JSON_destination, boost::property_tree::ptree &JSON_source,
                     const std::string &path_source);
}


#endif //CLIENTPDS_FILESYSTEM_UTILITY_H
