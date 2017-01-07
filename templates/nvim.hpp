// Auto generated

#ifndef NEOVIM_CPP__NVIM_HPP_
#define NEOVIM_CPP__NVIm_HPP_

#include "nvim_rpc.hpp"

namespace nvim {

class Nvim {
public:
    void connect_tcp(const std::string &host, 
            const std::string &service, double timeout_sec = 1.0) {
        
        client_.connect_tcp(host, service, timeout_sec);
    }

{% for func in functions%}
    {{func.return}} {{func.name}} ({% for arg in func.args %}{{arg.type}} {{arg.name}}{% if not loop.last %}, {% endif %}{% endfor %}) { 
        {% if func.return != "void" %}
        {{func.return}} res;
        client_.send("{{func.name}}", res{% for arg in func.args %}, {{arg.name}}{% endfor %}); 
        return res;
        {% else %}
        client_.send("{{func.name}}", nullptr{% for arg in func.args %}, {{arg.name}}{% endfor %}); 
        {% endif %}
    }
{% endfor %}

private:
    NvimRPC client_;

};

} //namespace nvim

#endif //NEOVIM_CPP__NVIM_HPP_
