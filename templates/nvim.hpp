// Auto generated

#ifndef NEOVIM_CPP__NVIM_HPP
#define NEOVIM_CPP__NVIM_HPP

#include "nvim_rpc.hpp"

namespace nvim {

class Nvim {
public:
    void connect_tcp(const std::string &host,
            const std::string &service, double timeout_sec = 1.0);
    void connect_pipe(const std::string &name,
            double timeout_sec = 1.0);

{% for func in functions %}
    {{func.return}} {{func.short_name}}({% for arg in func.args %}{{arg.type}} {{arg.name}}{% if not loop.last %}, {% endif %}{% endfor %});
{%- endfor %}

{%- for func in functions %}
    void async_{{func.short_name}}({% for arg in func.args %}{{arg.type}} {{arg.name}}{% if not loop.last %}, {% endif %}{% endfor %});
{%- endfor %}

private:
    NvimRPC client_;

};

} //namespace nvim

#endif //NEOVIM_CPP__NVIM_HPP
