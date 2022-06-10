#include "nvim.hpp"
#include "util.h"
#include <codecvt>

bool IS_GBK;

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

// https://stackoverflow.com/questions/41744559/is-this-a-bug-of-gcc
// https://en.cppreference.com/w/cpp/locale/wstring_convert/~wstring_convert
// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};
typedef deletable_facet<std::codecvt_byname<wchar_t, char, mbstate_t>> facet_t;

// https://blog.csdn.net/qq_31175231/article/details/83865059
std::string gbk_to_utf8(const std::string &str)
{
    // GBK locale name in windows
    const char *GBK_LOCALE_NAME = ".936";
    std::wstring_convert<facet_t> convert(new facet_t(GBK_LOCALE_NAME));
    std::wstring tmp_wstr = convert.from_bytes(str);

    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv2;
    return cv2.to_bytes(tmp_wstr);
}
