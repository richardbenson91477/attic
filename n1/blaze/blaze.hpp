#ifndef BLAZE_INCLUDED

#include <cstdlib>
#include <cstdint>
#include <cstring>

class blaze {
public:
    uint32_t item_n;
    uint32_t block_n;
    uint32_t block_len;
    void **__p;
    bool auto_compact_;
    uint32_t iter_c;
    void **__iter;

    blaze ();
    ~blaze ();

    bool add (void *_v);
    bool insert (void *_v, uint32_t i);
    void *get (uint32_t i);
    bool del (void *_v, uint32_t i, void (* hook_fn)(void *));
    bool del (uint32_t i) {
        return del (NULL, i, NULL);
    }
    bool del (void *_v) {
        return del (_v, 0, NULL);
    }
    void destroy (void (* hook_fn)(void *));
    bool compact ();
    void *iter_init (bool save_, uint32_t *iter_c_last);
    void *iter_init ();
    void *iterate ();
    void *iter_get ();
    static void hook_free (void *_v);
};

#define BLAZE_INCLUDED
#endif

