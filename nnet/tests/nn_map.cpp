#include "../nnet.hpp"

int main (int argc, char **argv) {
    double res;
    uint32_t c, c2;
    class nnet *_nnet;
    FILE *f_in, *f_out;

    if (argc < 5) {
        printf ("usage: %s net.mnn weight.mnw input output\n", argv[0]);
        return 1;
    }
 
    _nnet = new nnet (argv[1]);
    if (! _nnet->item_n) {
        printf ("failed to load network\n");
        return 1;
    }

    if (! _nnet->load_weight (argv[2])) {
        printf ("can't open weight file - did you train?\n");
        delete _nnet;
        return 1;
    }

    f_in = fopen (argv[3], "r");
    if (! f_in) {
        printf ("can't open input\n");
        delete _nnet;
        return 1;
    }

    f_out = fopen (argv[4], "w");
    if (! f_out) {
        printf ("can't open output\n");
        delete _nnet;
        return 1;
    }

    while (! feof (f_in)) {
        for (c = 1; c < _nnet->in_n; c ++)
            fscanf (f_in, "%lf", _nnet->items[0]._in + c);

        if (feof (f_in)) break;

        _nnet->net (0);
        
        for (c = 0; c < _nnet->out_n; c++) {
            if (! _nnet->value_n) {
                fprintf (f_out, "%lf ", _nnet->_out_sig[c]);
            }
            else {
                c2 = _nnet->val_nearest (_nnet->_out_sig[c]);
                if (c2 <= _nnet->value_n) {
                    fprintf (f_out, "%s (%lf) ", _nnet->values[c2].name_s, 
                        _nnet->_out_sig[c]);
                }
                else 
                    fprintf (f_out, "ERROR ");
            }
        }
        
        fprintf (f_out, "\n");
    }
    
    delete _nnet;
    return 0;
}

