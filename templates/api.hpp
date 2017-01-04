// Auto generated

#ifndef NEOVIM_CPP__API_HPP_
#define NEOVIM_CPP__API_HPP_

#include "neovim.hpp"

class Nvim {
    using Window = int64_t;
    using Buffer = int64_t;
    using Tabpage = int64_t;
    using Integer = int64_t;
    using Object = boost::any;

public:
{% for func in functions%}
    {{func.return}} {{func.name}} ({% for arg in func.args %}{{arg.type}} {{arg.name}}{% if not loop.last %}, {% endif %}{% endfor %}) { 
        return client.send("{{func.name}}"{% for arg in func.args %}, {{arg.name}}{% endfor %}); 
    }
{% endfor %}

private:
    NeoVim client;

};

#endif //NEOVIM_CPP__API_HPP_
