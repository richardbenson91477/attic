#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

extern double map (double);

int main (int argc, char **argv) {
    double i, o, rate = 0.1, x1 = 0.0, xn = 0.0;
    int c, n;
    bool bias = 0, out = 0, in = 0, x1set = 0, xnset = 0;

    for (c = 1; c < argc; c ++) {
        if (! strcmp (argv[c], "bias")) {
            bias = 1;
        }
        if (! strcmp (argv[c], "out")) {
            out = 1;
        }
        else if (! strcmp (argv[c], "in")) {            
            in = 1;
        }
        else if (! strcmp (argv[c], "rate")) {
            rate = atof (argv[c +1]);
        }
        else if (! strcmp (argv[c], "x1")) {
            x1 = atof (argv[c +1]);
            x1set = 1;
        }
        else if (! strcmp (argv[c], "xn")) {
            xn = atof (argv[c +1]);
            xnset = 1;
        }
    }

    if (! x1set)
        x1 = -1.0;
    if (! xnset)
        xn = 1.0;

    for (i = x1; i <= xn; i += rate) {
        o = map (i);

        if (bias)
            printf ("1.0 ");
        if (out)
            printf ("%lf\n", o);
        else if (in)
            printf ("%lf\n", i);
        else
            printf ("%lf %lf\n", i, o);
    }

    return 0;
}
