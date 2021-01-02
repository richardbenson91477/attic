#ifndef NNET_INCLUDED

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <csignal>
#include <cmath>
#include <ctime>

using namespace std;

class nnet {
public:
    struct item {
        double *_in;
        double *_out;
    } *items;

    struct value {
        char name_s[80];
        double val;
    } *values;

    uint32_t in_n, hid_n, out_n;
    uint32_t item_n, value_n;
    uint32_t scans_per_item, scans_per_set;

    double *_hid_sig, **__hid_weight, *_out_sig, **__out_weight;
    double *_err, error_mu, error_sum, error_thresh;
    double pos_thresh, test_thresh;

    nnet (char *name_s);
    ~nnet ();

    double scan (void);
    void net (uint32_t i);
    void update_weight (uint32_t i);
    double test ();
    void rand_weight (void);
    bool load_weight (char *name_s);
    bool save_weight (char *name_s);
    uint32_t val_get (const char *name_s);
    uint32_t val_get (double val);
    uint32_t val_nearest (double val);
};

#define NNET_INCLUDED
#endif

