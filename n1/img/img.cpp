#include "img.hpp"

img::img () {
    this->w = this->h = 0;
    this->_b = NULL;
    this->free_ = true;
}

img::~img () {
    if (this->free_ && this->_b)
        free (this->_b);
}

// RWH format: int32_t width, int32_t height, uint8_t *p (RGBA)
bool img::load_rwh (arc *_arc, const char *_s) {
    FILE *f;
    uint8_t *_b;
    uint32_t b_n;
    bool res_;
    file *_file = NULL;

    if (_arc) {
        if (! _arc->get (_s, &_b, &b_n))
            return false;
        this->free_ = false;
    }
    else {
        _file = new file (_s);
        if (! _file->_b)
            return false;
        _file->free_ = false;
        _b = _file->_b;
        b_n = _file->b_n;
    }

    this->w = *(uint32_t *)_b;
    _b += 4;
    this->h = *(uint32_t *)_b;
    _b += 4;
    this->_b = _b;

    if (_file)
        delete _file;
}

bool img::load_rwh (const char *file_s) {
    return load_rwh (NULL, file_s);
}

bool img::load_raw (arc *_arc, const char *_s, int32_t w, int32_t h) {
    uint8_t *_b;
    uint32_t b_n;
    bool res_;
    file *_file = NULL;

    if (_arc) {
        if (! _arc->get (_s, &_b, &b_n))
            return false;
        this->free_ = false;
    }
    else {
        _file = new file (_s);
        if (! _file->_b)
            return false;
        _file->free_ = false;
        _b = _file->_b;
        b_n = _file->b_n;
    }

    this->w = w;
    this->h = h;
    this->_b = _b;

    if (_file)
        delete _file;
}

bool img::load_raw (const char *file_s, int32_t w, int32_t h) {
    return load_raw (NULL, file_s, w, h);
}

bool img::load_png (const char *file_s) {
    png_structp png_ptr;
    png_infop info_ptr;
    uint8_t **row_pointers;
    FILE *f_in;
    int32_t row_s, c;

    f_in = fopen (file_s, "rb");
    if (! f_in) {
        dlog::error ("img", file_s);
        return false;
    }

    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (! png_ptr) {
        dlog::error ("img", "png_create_read_struct");
        return false;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (! info_ptr) {
        dlog::error ("img", "png_create_info_struct");
        png_destroy_read_struct (&png_ptr, (png_infopp) NULL,
            (png_infopp) NULL);
        return false;
    }

    png_init_io (png_ptr, f_in);
    png_read_info (png_ptr, info_ptr);
    png_set_filler (png_ptr, 255, PNG_FILLER_AFTER);
    png_read_update_info (png_ptr, info_ptr);

    // Get dimensions and row size
    row_s = png_get_rowbytes (png_ptr, info_ptr);
    this->w = png_get_image_width (png_ptr, info_ptr);
    this->h = png_get_image_height (png_ptr, info_ptr);

    this->_b = (uint8_t *) malloc (this->w * this->h * 4);
    if (! this->_b) {
        dlog::error ("img", "allocate png data failed");
        return false;
    }

    row_pointers = (uint8_t **) malloc (this->h * sizeof(uint8_t *));
    for (c = 0; c < this->h; c ++)
        row_pointers[c] = this->_b + (c * row_s);

    png_read_image (png_ptr, row_pointers);

    free (row_pointers);
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    fclose (f_in);
    return true;
}

void img::swap_r_b () {
    uint8_t *_b;
    int32_t c;

    if (! this->_b)
        return;

    _b = this->_b;
    for (c = 0; c < (this->w * this->h); c ++) {
        // nifty swap trick
        _b[0] += _b[2];
        _b[2] = _b[0] - _b[2];
        _b[0] -= _b[2];
        _b += 4;
    }
}

