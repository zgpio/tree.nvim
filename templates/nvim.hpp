// Auto generated

#ifndef NEOVIM_CPP__NVIM_HPP_
#define NEOVIM_CPP__NVIm_HPP_

#include "nvim_rpc.hpp"

namespace nvim {

class Nvim {
public:
{% for func in functions%}
    {{func.return}} {{func.name}} ({% for arg in func.args %}{{arg.type}} {{arg.name}}{% if not loop.last %}, {% endif %}{% endfor %}) { 
        {% if func.return != "void" %}
        {{func.return}} res;
        client.send("{{func.name}}", res{% for arg in func.args %}, {{arg.name}}{% endfor %}); 
        return res;
        {% else %}
        client.send("{{func.name}}", nullptr{% for arg in func.args %}, {{arg.name}}{% endfor %}); 
        {% endif %}
    }
{% endfor %}

private:
    NvimRPC client;

};

} //namespace nvim

#endif //NEOVIM_CPP__NVIM_HPP_
