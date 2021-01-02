#include "../nnet.hpp"

#define NTRIES (20)
#define NFREEBIES (20)
#define TFREE (0.001)

class nnet *_nnet = NULL;

void sig (int c) {
    printf ("caught signal: bailing out\n");
    if (_nnet)
        _nnet->save_weight ((char *)"save.mnw");
    exit (-1);
}

int main (int argc, char **argv) {
    int c, freebies;
    double res, res_prev;
    double test_res, test_res_prev;
    bool done;

    signal (SIGINT, sig);

    srand ((uint32_t)time (NULL));

    if (argc < 2) {
        printf ("usage: %s input.mnn [weight.mnw]\n", argv[0]);
        return 1;
    }
    
    _nnet = new nnet (argv[1]);
    if (! _nnet->item_n) {
        printf ("failed to load network\n");
        return 1;
    }

    if (argc >= 2) {
        done = false;
        while (! done) {
            test_res = 100.0;
            res_prev = 2.0;
            _nnet->rand_weight ();
            c = freebies = 0;
            while (c < NTRIES) {
                res_prev = res;
                res = _nnet->scan ();

                test_res_prev = test_res;
                test_res = _nnet->test ();

                if ((_nnet->error_sum <= _nnet->error_thresh) && 
                        (test_res >= _nnet->test_thresh)) {
                    done = true;
                    break;
                }
 
                if ((freebies < NFREEBIES) && ((res < (res_prev - TFREE)) 
                        || (test_res > test_res_prev))) {
                    if (c > 1)
                        c -= 1;
                    freebies ++;
                    printf ("1up #%d\n", freebies);
                }
                else 
                    c += 1;
            }
        }
    }

    if (argc < 3) 
        _nnet->save_weight ((char *)"save.mnw");
    else 
        _nnet->save_weight ((char *)argv[2]);
    
    printf ("successful train\n");
    
    delete _nnet;
    return 0;
}

