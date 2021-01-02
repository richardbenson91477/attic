#include "nnet.hpp"

nnet::nnet (char *name_s) {
    char item_s [PATH_MAX];
    char val_s [PATH_MAX];
    char val_val_s [80];
    FILE *f_in;
    uint32_t in_c, hid_c, out_c, val_c, item_c;
 
    item_n = 0;
    
    f_in = fopen (name_s, "r");
    if (! f_in)
        return;

    fscanf (f_in, "%d %d %d\n", &in_n, &hid_n, &out_n);
    fscanf (f_in, "%lf %lf %lf %lf %d %d\n", 
        &error_mu, &error_thresh, &pos_thresh, &test_thresh, 
        &scans_per_set, &scans_per_item);

    fscanf (f_in, "%s\n", item_s);

    val_s[0] = 0;
    fscanf (f_in, "%s\n", val_s);

    fclose (f_in);

    value_n = 0;
    values = NULL;
    if (val_s[0]) {
        f_in = fopen (val_s, "r");
        if (! f_in)
            return;

        for (int32_t ch = fgetc (f_in); ch >= 0; ch = fgetc (f_in))
            if (ch == '\n')
                value_n ++;

        fseek (f_in, 0, SEEK_SET);
    
        values = (struct value *) malloc (value_n * sizeof (struct value));
        for (val_c = 0; val_c < value_n; val_c ++) {
            fscanf (f_in, "%s ", values [val_c].name_s);
            fscanf (f_in, "%lf\n", &(values [val_c].val));
        }
        fclose (f_in);
    }
    
    f_in = fopen (item_s, "r");
    if (! f_in)
        return;

    for (int32_t ch = fgetc (f_in); ch >= 0; ch = fgetc (f_in))
        if (ch == '\n')
            item_n ++;

    fseek (f_in, 0, SEEK_SET);

    // bias input
    in_n ++;
    
    items = (struct item *) malloc (item_n * sizeof (struct item));
    for (item_c = 0; item_c < item_n; item_c ++) {
        items [item_c]._in = (double *) malloc (in_n * sizeof (double));
        items [item_c]._out = (double *) malloc (out_n * sizeof (double));
        
        // bias input
        items [item_c]._in[0] = 1.0;
        for (in_c = 1; in_c < in_n; in_c ++)
            fscanf (f_in, "%lf ", items [item_c]._in +in_c);

        for (out_c = 0; out_c < out_n; out_c ++) {
            if (! value_n)
                fscanf (f_in, "%lf ", items [item_c]._out +out_c);
            else {
                fscanf (f_in, "%s ", val_val_s);
                items [item_c]._out[out_c] = values [val_get (val_val_s)].val;
            }
        }
    }
    fclose (f_in);

    cout << "nnet: loaded" << name_s << endl;

    _hid_sig = (double *) malloc (hid_n * sizeof (double));
    _out_sig = (double *) malloc (out_n * sizeof (double));

    __out_weight = (double **) malloc (hid_n * sizeof (double *));
    for (hid_c = 0; hid_c < hid_n; hid_c ++)
        __out_weight [hid_c] = (double *) malloc (out_n * sizeof (double));

    __hid_weight = (double **) malloc (in_n * sizeof (double *));
    for (in_c = 0; in_c < in_n; in_c ++)
        __hid_weight [in_c] = (double *) malloc (hid_n * sizeof (double));

    _err = (double *) malloc (out_n * sizeof (double));
}

nnet::~nnet () {
    uint32_t in_c, hid_c, item_c;

    free (_hid_sig);
    free (_out_sig);

    for (hid_c = 0; hid_c < hid_n; hid_c ++)
        free (__out_weight[hid_c]);
    free (__out_weight);

    for (in_c = 0; in_c < in_n; in_c ++)
        free (__hid_weight[in_c]);
    free (__hid_weight);

    free (_err);
 
    for (item_c = 0; item_c < item_n; item_c ++) {
        free (items[item_c]._in);
        free (items[item_c]._out);
    }
    free (items);

    if (values) free (values);
}

double nnet::scan (void) {
    uint32_t per_item_c, per_set_c;
    uint32_t out_c, item_c;

    cout << "nnet: performing scans" << endl;

    for (per_set_c = 0; per_set_c < scans_per_set; per_set_c ++) {
        error_sum = 0.0;
        for (item_c = 0; item_c < item_n; item_c ++) {
            for (per_item_c = 0; per_item_c < scans_per_item; per_item_c++) {
                net (item_c);

                for (out_c = 0; out_c < out_n; out_c ++)
                    _err[out_c] = items[item_c]._out[out_c] - _out_sig[out_c];

                update_weight (item_c);
            }
 
            for (out_c = 0; out_c < out_n; out_c ++)
                error_sum += (_err[out_c] * _err[out_c]);
        }

        printf ("   %lf\r", error_sum);

        if ((error_sum <= error_thresh) && (test() >= test_thresh)) {
            cout << "nnet: error_sum: " << error_sum << endl;
            return error_sum;
        }
    }

    cout << "nnet: error_sum: " << error_sum << endl;
    return error_sum;
}

void nnet::net (uint32_t whichi) {
    uint32_t in_c, hid_c, out_c;
    double sum;

    for (hid_c = 0; hid_c < hid_n; hid_c ++) {
        sum = 0.0;
        for (in_c = 0; in_c < in_n; in_c ++)
            sum += items[whichi]._in[in_c] * __hid_weight[in_c][hid_c];

        _hid_sig[hid_c] = tanh (sum);
    }

    for (out_c = 0; out_c < out_n; out_c ++) {
        sum = 0.0;
        for (hid_c = 0; hid_c < hid_n; hid_c ++)
            sum += _hid_sig[hid_c] * __out_weight[hid_c][out_c];
 
        _out_sig[out_c] = tanh (sum);
    }
}

void nnet::update_weight (uint32_t whichi) {
    double out_slope, hid_slope;
    uint32_t in_c, hid_c, out_c;

    for (out_c = 0; out_c < out_n; out_c ++) {
        out_slope = 1.0 - tanh (_out_sig[out_c] * _out_sig[out_c]);
        
        for (hid_c = 0; hid_c < hid_n; hid_c++) {
            hid_slope = 1.0 - tanh (_hid_sig[hid_c] * _hid_sig[hid_c]);
            
            for (in_c = 0; in_c < in_n; in_c++) {
                __hid_weight [in_c][hid_c] = __hid_weight [in_c][hid_c] + 
                    (items [whichi]._in[in_c] * hid_slope) *
                    (__out_weight [hid_c][out_c] * out_slope) *
                    (_err [out_c] * error_mu);
            }
        
            __out_weight [hid_c][out_c] = __out_weight [hid_c][out_c] + 
                (_hid_sig[hid_c] * out_slope) *
                (_err[out_c] * error_mu);
        }
    }
}

double nnet::test () {
    double score;
    uint32_t item_c, out_c, pos_n;

    pos_n = 0;
    for (item_c = 0; item_c < item_n; item_c ++) {
        net (item_c);
        for (out_c = 0; out_c < out_n; out_c ++) {
            _err [out_c] = items [item_c]._out[out_c] - _out_sig[out_c];
            if (fabs (_err[out_c]) < pos_thresh)
                pos_n ++;
        }
    }
 
    score = ((double)pos_n / ((double)out_n * (double)item_n)) * 100.0;
    return score;
}

void nnet::rand_weight (void) {
    uint32_t in_c, hid_c, out_c;

    cout << "nnet: setting random weights" << endl;

    for (hid_c = 0; hid_c < hid_n; hid_c++)
        for (in_c = 0; in_c < in_n; in_c++)
            __hid_weight [in_c][hid_c] =
                ((double)rand() / (double)(RAND_MAX /2)) - 1.0;

    for (out_c = 0; out_c < out_n ; out_c++)
        for (hid_c = 0; hid_c < hid_n; hid_c++)
            __out_weight [hid_c][out_c] =
                ((double)rand() / (double)(RAND_MAX /2)) - 1.0;
}

bool nnet::load_weight (char *name_s) {
    FILE *f_in;
    uint32_t in_c, hid_c;

    cout << "nnet: loading weights" << name_s << endl;

    f_in = fopen (name_s, "rb");
    if (! f_in)
        return false;

    for (in_c = 0; in_c < in_n; in_c++)
        fread (__hid_weight[in_c], sizeof (double), hid_n, f_in);

    for (hid_c = 0; hid_c < hid_n; hid_c++)
        fread (__out_weight[hid_c], sizeof (double), out_n, f_in);

    fclose (f_in);
    return true;
}

bool nnet::save_weight (char *name_s) {
    FILE *fout;
    uint32_t in_c, hid_c;

    cout << "saving weights" << name_s << endl;

    fout = fopen (name_s, "w");
    if (! fout)
        return false;

    for (in_c = 0; in_c < in_n; in_c++)
        fwrite (__hid_weight[in_c], sizeof (double), hid_n, fout);

    for (hid_c = 0; hid_c < hid_n; hid_c++)
        fwrite (__out_weight[hid_c], sizeof (double), out_n, fout);

    return true;
}

uint32_t nnet::val_get (const char *n) {
    uint32_t val_c;

    if ((! value_n) || (! values))
        return (uint32_t)-1;
    
    for (val_c = 0; val_c < value_n; val_c ++)
        if (! strcmp (values[val_c].name_s, n))
            return val_c;
    
    return (uint32_t)-1;
}

uint32_t nnet::val_get (double v) {
    uint32_t val_c;

    if ((! value_n) || (! values))
        return (uint32_t)-1;

    for (val_c = 0; val_c < value_n; val_c ++)
        if (values[val_c].val == v)
            return val_c;

    return (uint32_t)-1;
}

uint32_t nnet::val_nearest (double v) {
    double near_v, val_d;
    uint32_t val_c, near_c = (uint32_t)-1;

    if ((! value_n) || (! values))
        return (uint32_t)-1;

    near_v = 2.0;
    for (val_c = 0; val_c < value_n; val_c ++) {
        val_d = fabs (values[val_c].val - v);
        if (val_d < near_v) {
            near_v = val_d;
            near_c = val_c;
        }
    }

    return near_c;
}

