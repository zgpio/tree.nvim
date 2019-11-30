#include "nvim.hpp"
#include "util.h"

std::string type_name(const nvim::Object &obj)
{
    std::string s;
    if (obj.is_vector()) s = "vector";
    else if (obj.is_ext()) s = "ext";
    else if (obj.is_map()) s = "map";
    else if (obj.is_nil()) s = "nil";
    else if (obj.is_bool()) s = "bool";
    else if (obj.is_double()) s = "double";
    else if (obj.is_string()) s = "string";
    else if (obj.is_ext_ref()) s = "ext_ref";
    else if (obj.is_int64_t()) s = "int64_t";
    else if (obj.is_raw_ref()) s = "raw_ref";
    else if (obj.is_multimap()) s = "multimap";
    else if (obj.is_vector_char()) s = "vector_char";
    else if (obj.is_boost_string_ref()) s = "boost_string_ref";
    else if (obj.is_uint64_t()) s = "uint64_t";
    return s;
}
