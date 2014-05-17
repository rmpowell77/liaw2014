/**
 *  \file test_dynamic_model.cpp
 *
 *  Copyright 2014 Michael Caisse : ciere.com
 *  Copyright 2014 Kevin Harris
 *  Copyright 2014 Michal Bukovsky
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <map>
#include <unordered_map>

#include <boost/test/unit_test.hpp>
#include <boost/cppte/frontend/stache_model.hpp>
#include <boost/cppte/model/dynamic_model_printer.hpp>

#include "test_utils.hpp"

namespace
{

// just some user types
typedef std::map<std::string, std::string> map_of_strings;
typedef std::unordered_map<std::string, std::string> umap_of_strings;
struct user {
    std::string name;
    std::string location;
    std::vector<map_of_strings> favorites;
    std::vector<umap_of_strings> unloved;
};
typedef std::map<std::string, user> map_of_users;

} // namespace

namespace boost { namespace cppte { namespace model
{

template <>
std::string get_variable_value(const map_of_strings &model,
                               const std::string &key)
{
    auto ivalue = model.find(key);
    if (ivalue != model.end()) return ivalue->second;
    return "undefined:" + key;
}

template <>
std::string get_variable_value(const umap_of_strings &model,
                               const std::string &key)
{
    auto ivalue = model.find(key);
    if (ivalue != model.end()) return ivalue->second;
    return "undefined:" + key;
}

template <>
std::string get_variable_value(const user &model,
                               const std::string &key)
{
    if (key == "NAME") {
        return model.name;

    } else if (key == "LOCATION") {
        return model.location;
    }
    return "undefined:" + key;
}

template <>
void get_section_value(const user &model,
                       const std::string &key,
                       section_range_sink<user> &sink)
{
    if (key == "FAVORITES") {
        sink(model.favorites);

    } else if (key == "UNLOVED") {
        sink(model.unloved);
    }
}

template <>
void get_section_value(const map_of_users &model,
                       const std::string &key,
                       section_range_sink<map_of_users> &sink)
{
    if (key == "USER") {
        sink(model);
    }
}

}}}

BOOST_AUTO_TEST_CASE(test_simple_value)
{
    namespace bfe = boost::cppte::front_end;
    map_of_strings model;
    model["NAME"] = "Boosties";
    bfe::ast::stache_root ast = parse("Hello, {{NAME}}!");
    std::string result = print(ast, model);
    BOOST_CHECK_EQUAL("Hello,"
            " "
            "Boosties!", result);
}

BOOST_AUTO_TEST_CASE(test_section_printing)
{
    namespace bfe = boost::cppte::front_end;
    user bob;
    bob.name = "Bob";
    bob.location = "Earth";
    bob.favorites.push_back(map_of_strings());
    bob.favorites.back()["FOOD"] = "Pizza";
    bob.favorites.back()["MUSIC"] = "Classical";
    map_of_users model;
    model["USER"] = bob;
    bfe::ast::stache_root ast = parse(
            "{{#USER}}"
            "user.name={{NAME}}\n"
            "user.location={{LOCATION}}\n"
            "{{#FAVORITES}}"
            "user.favorite.food={{FOOD}}\n"
            "user.favorite.music={{MUSIC}}\n"
            "{{/FAVORITES}}"
            "{{/USER}}");

    // FIXME! Whitespace!
    // Should have a trailing newline on all of these lines.
    BOOST_CHECK_EQUAL(
            "user.name=Bob\n"
            "user.location=Earth" // FIXME: there should be \n !
            "user.favorite.food=Pizza\n"
            "user.favorite.music=Classical\n",
            print(ast, model));
}

