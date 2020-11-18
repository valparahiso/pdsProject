//
// Created by simo on 10/11/20.
//

#ifndef CLIENTPDS_JSON_UTILITY_H
#define CLIENTPDS_JSON_UTILITY_H

#include "hash_utility.h"
#include "filesystem_utility.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace JSON_utility {
    void print_JSON(const boost::property_tree::ptree & JSON);

    boost::property_tree::ptree create_json(const std::string& username_, std::string password_, const std::string& command_, const boost::filesystem::path& path_);

    boost::property_tree::ptree create_data_json(const boost::filesystem::path& path);

    std::vector<boost::property_tree::ptree> JSON_differences(const boost::property_tree::ptree& JSON_destination, boost::property_tree::ptree JSON_source,
                                                              const std::string& username_, const boost::filesystem::path& path_, std::vector<boost::property_tree::ptree> files_to_ask);
}


#endif //CLIENTPDS_JSON_UTILITY_H
