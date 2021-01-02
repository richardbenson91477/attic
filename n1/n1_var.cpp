#include "n1.hpp"

namespace n1_var {
    char *_var;
}

void n1_var::var_start (char *m) {
    _var = m;
}
bool n1_var::var_next (char *_var_out) {
    char *m;

    if ((! _var_out) || (! _var) || (! _var[0]))
        return false;

    for (; _var[0] == ' '; _var ++);
    if (! _var[0])
        return false;

    m = strchr (_var, ' ');
    if (! m)
        m = _var + strlen (_var);

    strncpy (_var_out, _var, m - _var);
    _var_out[m - _var] = 0;

    _var = m;
    return true;
}

