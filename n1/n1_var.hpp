#ifndef N1_MISC_INCLUDED

// note: not a class
namespace n1_var {

    extern char *_var;

    void var_start (char *m);
    bool var_next (char *_var_out);

} // namespace n1_var

#define N1_MISC_INCLUDED
#endif

