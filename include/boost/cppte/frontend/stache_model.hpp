#pragma once

#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <string>
#include <map>
#include <vector>
#include <functional>

namespace boost
{
	namespace cppte
	{
		namespace front_end
		{
			typedef std::function<std::string()> stache_string_function;
			typedef std::function<bool()> stache_bool_function;

			typedef boost::make_recursive_variant<
				std::string,
				stache_string_function,
				stache_bool_function,
				std::vector<boost::recursive_variant_>,
				std::map<std::string, boost::recursive_variant_>
				>::type stache_variant;

			typedef std::vector<stache_variant> stache_model_vector;
			typedef std::map<std::string, stache_variant> stache_model;
		}
	}
}
