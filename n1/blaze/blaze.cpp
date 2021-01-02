#include "blaze.hpp"

// implements a dynamic array of void pointers

blaze::blaze () {
    this->item_n = 0;
    this->block_len = 8192;
    this->block_n = 0;
    this->__p = NULL;
    this->auto_compact_ = false;
}

blaze::~blaze () {
    destroy (NULL);
}

bool blaze::add (void *_v) {
    void **__v;

    if (this->item_n == (this->block_len * this->block_n)) {
        this->block_n ++;

        __v = (void **) realloc (this->__p, 
            (this->block_len * this->block_n) * sizeof(void **));

        if (! __v)
            return false;

        this->__p = __v;
    }

    *(this->__p + this->item_n) = _v;

    this->item_n ++;
    return true;
}

bool blaze::insert (void *_v, uint32_t i) {
    void **__v;
    uint32_t c;

    if (i > this->item_n) 
        return false;

    if (this->item_n == this->block_len * this->block_n) {
        this->block_n ++;

        __v = (void **) realloc (this->__p, 
            (this->block_len * this->block_n) * sizeof(void **));

        if (! __v)
            return false;

        this->__p = __v;
    }

    if (this->item_n) {
        __v = this->__p + this->item_n;
        for (c = this->item_n; c > i; c--) {
            *__v = *(__v - 1);
            __v -= 1; 
        }
    }

    *(this->__p + i) = _v;

    this->item_n ++;
    return true;
}

void *blaze::get (uint32_t i) {
    if (! this->item_n) 
        return NULL;
    else if (i >= this->item_n) 
        return NULL;

    return *(this->__p + i);
}

bool blaze::del (void *_v, uint32_t i, void (* hook_fn)(void *)) {
    void **__v;
    bool found;
    uint32_t c, found_id;

    if ((! this->item_n) || (i >= this->item_n))
        return false;

    if (_v) {
        found = false;
        __v = this->__p;
        for (c = 0; c < this->item_n; c ++) {
            if (*__v == _v) {
                found = true;
                break;
            }
            __v ++;
        }
        if (! found) 
            return false;

        found_id = c;
        if (hook_fn) 
            hook_fn (_v);
    }
    else {
        found_id = i;
        if (hook_fn) 
            hook_fn (*(this->__p + i));
    }

    __v = this->__p + found_id;
    for (c = found_id; c < (this->item_n -1); c ++) {
        *__v = *(__v + 1);
        __v ++;
    }

    this->item_n --;

    if (this->auto_compact_) 
        compact ();

    return true;
}

void blaze::destroy (void (* hook_fn)(void *)) {
    void **__v;
    uint32_t c;

    if (! this->__p) 
        return;

    if (hook_fn && (this->item_n > 0)) {
        __v = this->__p;

        for (c = 0; c < this->item_n; c ++) {
            hook_fn (*__v);
            __v ++;
        }
    }

    free (this->__p);
    this->__p = NULL;

    this->block_n = 0;
    this->item_n = 0;
}

bool blaze::compact () {
    void **__v;

    if (! this->block_n) 
        return false;

    if (this->item_n) {
        if (this->block_len * (this->block_n -1) >= this->item_n) {
            this->block_n --;

            __v = (void **) realloc (this->__p, 
                (this->block_len * this->block_n) * sizeof(void **));

            if (__v)
                this->__p = __v;
            else 
                return false;
        }
    }
    else {
        this->block_n = 0;

        free (this->__p);
        this->__p = NULL;
    }

    return true;
}

void *blaze::iter_init (bool save_, uint32_t *iter_c_last) {
    if (save_) {
        *iter_c_last = this->iter_c;
        return this->iter_init ();
    }
    else {
        this->iter_c = *iter_c_last;
        this->__iter = this->__p + this->iter_c;
        return this->iter_get ();
    }
}

void *blaze::iter_init () {
    if (! this->item_n) 
        return NULL;

    this->iter_c = 0;
    this->__iter = this->__p;
    return *(this->__p);
}

void *blaze::iterate () {
    this->iter_c ++;
    this->__iter ++;
    return this->iter_get ();
}

void *blaze::iter_get () {
    if (this->iter_c >= this->item_n)
        return NULL;

    return *(this->__iter);
}

void blaze::hook_free (void *_v) {
    free (_v);
}

