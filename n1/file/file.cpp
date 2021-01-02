
#include "file.hpp"
    
file::file () {
    init ();
}

file::file (const char *file_s) {
    init ();
    this->load (file_s);
}

file::file (uint8_t *_b, uint32_t b_n) {
    init ();
    this->_b = _b;
    this->b_n = b_n;
    this->free_ = false;
}

file::~file () {
    if (this->mapped_)
        this->unmap ();

    if (this->free_ && this->_b)
        free (this->_b);
}

void file::init () {
    this->file_s[0] = '\0';
    this->_b = NULL;
    this->b_n = 0;
    
    this->free_ = true;

    this->mapped_ = false;
    this->readonly_ = false;
    this->shared_ = true;
}

bool file::map (const char *file_s, uint32_t b_n) {
    int f_in;
    uint32_t f_len;

    f_in = open (file_s, O_RDWR | O_CREAT);
    if (f_in < 0) {
        dlog::error ("file::map open", file_s);
        return false;
    }

    f_len = lseek (f_in, 0, SEEK_END);
    lseek (f_in, 0, SEEK_SET);

    if (! b_n) {
        if (f_len == 0) {
            close (f_in);
            dlog::error ("file::map: file is zero length", file_s);
            return false;
        }
        this->b_n = f_len;
    }
    else if (b_n > f_len) {
        // TODO: resize f_in
        this->b_n = f_len;
    }
    else {
        this->b_n = b_n;
    }

    this->_b = (uint8_t *) mmap (NULL, this->b_n,
        this->readonly_ ? PROT_READ : PROT_READ | PROT_WRITE,
        this->shared_ ? MAP_SHARED : MAP_PRIVATE,
        f_in, 0);

    if (this->_b == MAP_FAILED) {
        dlog::error ("file::map: mmap", file_s);
        return false;
    }

    close (f_in);
    strcpy (this->file_s, file_s);
    this->mapped_ = true;
    return true;
}

bool file::unmap () {
    if (! this->mapped_)
        return false;

    if (munmap ((void *)this->_b, this->b_n)) {
        dlog::error ("file::unmap: munmap", this->b_n);
        return false;
    }
    
    this->mapped_ = false;
    this->_b = NULL;
    this->b_n = 0;
    return true;
}

bool file::load (const char *file_s) {
    FILE *f_in;

    dlog::debug ("file::load", file_s);

    f_in = fopen (file_s, "rb");
    if (! f_in) {
        dlog::error ("file::load: fopen rb", file_s);
        return false;
    }

    fseek (f_in, 0, SEEK_END);
    this->b_n = ftell (f_in);
    fseek (f_in, 0, SEEK_SET);

    this->_b = (uint8_t *) malloc (this->b_n);
    if (! this->_b) {
        dlog::error ("file::load: malloc", this->b_n);
        return false;
    }

    if (fread (this->_b, 1, this->b_n, f_in) < this->b_n) {
        dlog::error ("file::load: fread", this->b_n);
        return false;
    }

    fclose (f_in);
    strcpy (this->file_s, file_s);
    return true;
}

bool file::save () {
    FILE *f_out;

    if (! this->file_s[0]) {
        dlog::error ("file::save: empty filename");
        return false;
    }

    f_out = fopen (this->file_s, "wb");
    if (! f_out) {
        dlog::error ("file::save: fopen wb", this->file_s);
        return false;
    }

    if (fwrite (this->_b, 1, this->b_n, f_out) < this->b_n) {
        dlog::error ("file::save: fwrite", b_n);
        return false;
    }

    fclose (f_out);
    return true;
}

bool file::save_as (const char *file_s) {
    strcpy (this->file_s, file_s);
    return this->save ();
}

bool file::_export (const char *file_s) {
    FILE *f_out;

    f_out = fopen (file_s, "wb");
    if (! f_out) {
        dlog::error ("file::_export: fopen wb", file_s);
        return false;
    }

    if (fwrite (this->_b, 1, this->b_n, f_out) < this->b_n) {
        dlog::error ("file::_export: fwrite", b_n);
        return false;
    }

    fclose (f_out);
    return true;
}

