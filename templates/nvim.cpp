// Auto generated
#include "nvim_rpc.hpp"
#include "nvim.hpp"

namespace nvim {

void Nvim::connect_tcp(const std::string &host,
        const std::string &service, double timeout_sec) {

    client_.connect_tcp(host, service, timeout_sec);
}


{% for func in functions %}
{{func.return}} Nvim::{{func.short_name}}({% for arg in func.args %}{{arg.type}} {{arg.name}}{% if not loop.last %}, {% endif %}{% endfor %}) {
    {%- if func.return != "void" %}
    {{func.return}} res;
    client_.call("{{func.name}}", res{% for arg in func.args %}, {{arg.name}}{% endfor %});
    return res;
    {%- else %}
    client_.call("{{func.name}}", nullptr{% for arg in func.args %}, {{arg.name}}{% endfor %});
    {%- endif %}
}
{% endfor %}

{% for func in functions %}
void Nvim::async_{{func.short_name}}({% for arg in func.args %}{{arg.type}} {{arg.name}}{% if not loop.last %}, {% endif %}{% endfor %}) {
    client_.async_call("{{func.name}}"{% for arg in func.args %}, {{arg.name}}{% endfor %});
}
{% endfor %}
} //namespace nvim
