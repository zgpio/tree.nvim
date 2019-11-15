#include "nvim.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>
using std::cout;
using namespace boost::filesystem;
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

void display(nvim::Nvim &nvim, path &p)
{
    try {
        if (exists(p)) {
            if (is_regular_file(p))
                cout << p << " size is " << file_size(p) << '\n';

            else if (is_directory(p)) {
                cout << p << " is a directory containing:\n";

                std::vector<std::string> lines;
                for (directory_entry &x : directory_iterator(p)) {
                    // cout << "    " << x.path() << '\n';
                    lines.push_back(x.path().generic_string());
                }
                nvim.nvim_buf_set_lines(0, 0, -1, true, lines);
            }
            else
                cout << p
                     << " exists, but is not a regular file or directory\n";
        }
        else
            cout << p << " does not exist\n";
    }

    catch (const filesystem_error &ex) {
        cout << ex.what() << '\n';
    }
}
void check_type(const nvim::Object &obj)
{
    if (obj.is_vector())
        std::cout << "vector" << std::endl;
    else if (obj.is_ext())
        std::cout << "ext" << std::endl;
    else if (obj.is_map())
        std::cout << "map" << std::endl;
    else if (obj.is_nil())
        std::cout << "nil" << std::endl;
    else if (obj.is_bool())
        std::cout << "bool" << std::endl;
    else if (obj.is_double())
        std::cout << "double" << std::endl;
    else if (obj.is_string())
        std::cout << "string" << std::endl;
    else if (obj.is_ext_ref())
        std::cout << "ext_ref" << std::endl;
    else if (obj.is_int64_t())
        std::cout << "int64_t" << std::endl;
    else if (obj.is_raw_ref())
        std::cout << "raw_ref" << std::endl;
    else if (obj.is_multimap())
        std::cout << "multimap" << std::endl;
    else if (obj.is_vector_char())
        std::cout << "vector_char" << std::endl;
    else if (obj.is_boost_string_ref())
        std::cout << "boost_string_ref" << std::endl;
    else if (obj.is_uint64_t())
        std::cout << "uint64_t" << std::endl;
}

int main(int argc, char *argv[])
{
    nvim::Nvim nvim;
    nvim.connect_tcp("localhost", "6666");
    nvim::Object rv = nvim.nvim_eval("( 3 + 2 ) * 4");
    check_type(rv);
    uint64_t res = rv.as_uint64_t();
    std::cout << res << std::endl;
    std::cout << "get_current_line = " << nvim.nvim_get_current_line()
              << std::endl;
    // nvim.vim_set_current_line("testhogefuga");

    if (argc < 2) {
        cout << "Usage: tut3 path\n";
        return 1;
    }

    path p(argv[1]);
    display(nvim, p);

    nvim::Integer src =
        nvim.nvim_buf_add_highlight(0, 0, "Identifier", 1, 0, 4);

    for (int i = 2; i < 10000; i++) {
        nvim.nvim_buf_add_highlight(0, src, "String", i, 2, 8);
    }
    for (;;) {
    }

    return 0;
}
