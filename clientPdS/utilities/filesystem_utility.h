//
// Created by simo on 09/11/20.
//

#ifndef UTILITIES_FILESYSTEM_UTILITY_H
#define UTILITIES_FILESYSTEM_UTILITY_H

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
    void write_file(const std::string&, std::vector<unsigned char>);


    std::vector<boost::property_tree::ptree>
    create_file_system(boost::property_tree::ptree JSON_destination, std::string path_source,
                       std::string path_destination, std::vector<boost::property_tree::ptree> files_to_ask);

    void add_to(const boost::property_tree::ptree &JSON_destination, boost::property_tree::ptree &JSON_source,
                const std::string &path_source,
                const std::string &path_destination, std::vector<boost::property_tree::ptree> files_to_ask);

    void delete_from(const boost::property_tree::ptree &JSON_destination, boost::property_tree::ptree &JSON_source,
                     const std::string &path_source);
}

#endif //UTILITIES_FILESYSTEM_UTILITY_H
