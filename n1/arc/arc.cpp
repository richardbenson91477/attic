
#include "arc.hpp"

arc::arc () {
    this->l.auto_compact_ = true;
    this->l.block_len = 20;
}

arc::~arc () {
    this->l.destroy (ive_destroy);
}

arc::ive *arc::add (const char *name_s) {
    arc::ive *_arc;
    file *_file;

    _file = new file (name_s);
    if (! _file->_b)
        return NULL;

    _arc = (arc::ive *) malloc (sizeof(arc::ive));
    if (! _arc) {
        dlog::error ("arc::add: malloc");
        delete _file;
        return NULL;
    }
    
    _arc->b_n = _file->b_n;
    _file->free_ = false;
    _arc->_b = _file->_b;

    _arc->name_s = (char *) strdup (name_s);
    _arc->name_n = (uint32_t) strlen (name_s);

    delete _file;
    return _arc;
}

bool arc::import_dir (const char *dir_s) {
    char name_s [PATH_MAX];
    struct stat s;
    arc::ive *_arc;
    dirent *_dirent;
    DIR *_dir;
    
    dlog::debug ("arc::import_dir", dir_s);

    if (dir_s) 
        sprintf (name_s, "%s/", dir_s);
    else 
        sprintf (name_s, "./");

    _dir = opendir (name_s);
    if (! _dir) {
        dlog::error ("arc::import_dir: opendir", name_s);
        return false;
    }

    _dirent = readdir (_dir);
    if (! _dirent) {
        dlog::error ("arc::import_dir: readdir", name_s);
        closedir (_dir);
        return false;
    }

    while (_dirent) {
        if (dir_s) 
            sprintf (name_s, "%s/%s", dir_s, _dirent->d_name);
        else 
            sprintf (name_s, "%s", _dirent->d_name);
        
        if (stat (name_s, &s) == -1) {
            dlog::error ("arc::import_dir: stat", name_s);
            return false;
        }

        if (! S_ISDIR (s.st_mode)) {
            _arc = this->add (name_s);
            if (! _arc) 
                return false;

            this->l.add (_arc);
        }
        else if (strcmp (_dirent->d_name, ".") &&
                strcmp (_dirent->d_name, ".."))
            if (! this->import_dir (name_s)) 
                return false;

        _dirent = readdir (_dir); 
    }

    closedir (_dir);
    return true;
}

bool arc::export_dir (const char *dir_s) {
    char file_s [PATH_MAX];
    char subdir_s [PATH_MAX];
    arc::ive *_arc;
    char *m;
    bool res_;
    file *_file;

    dlog::debug ("arc::export_dir", dir_s);

    for (_arc = (arc::ive *) this->l.iter_init (); _arc;
            _arc = (arc::ive *) this->l.iterate ()) {
        m = strchr (_arc->name_s, '/');
        while (m) {
            bcopy (_arc->name_s, subdir_s, m - _arc->name_s);
            subdir_s[m - _arc->name_s] = 0;
            sprintf (file_s, "%s/%s", dir_s, subdir_s);
            mkdir (file_s, 0755);

            m = strchr (m + 1, '/');
        }

        sprintf (file_s, "%s/%s", dir_s, _arc->name_s);
    
        _file = new file (_arc->_b, _arc->b_n);
        _file->free_ = false;
        res_ = _file->_export (file_s);
        if (! res_) {
            delete _file;
            return false;
        }
        delete _file;
    }

    return true;
}

void arc::report () {
    arc::ive *_arc;
    for (_arc = (arc::ive *) this->l.iter_init (); _arc;
            _arc = (arc::ive *) this->l.iterate ())
        printf ("name_s: %s, b_n: %u\n", _arc->name_s, _arc->b_n);
}

bool arc::get (const char *name_s, uint8_t **__b,  uint32_t *_b_n) {
    arc::ive *_arc;
    uint32_t iter;

    for (_arc = (arc::ive *) this->l.iter_init (true, &iter); _arc;
            _arc = (arc::ive *) this->l.iterate ())
        if (! strcmp (name_s, _arc->name_s)) {
            *__b = _arc->_b;
            *_b_n = _arc->b_n;
            this->l.iter_init (false, &iter);
            return true;
        }
    this->l.iter_init (false, &iter);
    return false;
}

bool arc::load_file (const char *name_s) {
    uint32_t b_n;
    uint8_t *b;
    bool res_;
    file *_file;

    _file = new file (name_s);
    if (! _file->_b)
        return false;

    res_ = this->load_ser (_file->_b, _file->b_n);
    if (! res_)
        return false;
   
    delete _file;
    return true;
}

bool arc::load_ser (const uint8_t *_b, uint32_t b_n) {
    arc::ive *_arc;
    uint32_t item_n, c;
    uint8_t *m;

    dlog::debug ("arc::load_ser");

    m = (uint8_t *) _b;

    if (memcmp (m, "N2ARCHIV", 8)) 
        return false;

    m += 8;

    item_n = *(uint32_t *)m;
    m += 4;

    for (c = 0; c < item_n; c ++) {
        _arc = (arc::ive *) malloc (sizeof(arc::ive));
        if (! _arc) {
            dlog::error ("malloc");
            return false;
        }

        _arc->name_n = *(uint32_t *)m;
        m += 4;

        _arc->name_s = (char *) malloc (_arc->name_n + 1);
        if (! _arc->name_s) {
            dlog::error ("malloc");
            return false;
        }

        bcopy (m, _arc->name_s, _arc->name_n);
        _arc->name_s[_arc->name_n] = 0;
        m += _arc->name_n;

        _arc->b_n = *(uint32_t *)m;
        m += 4;

        _arc->_b = (uint8_t *) malloc (_arc->b_n);
        if (! _arc->_b) {
            dlog::error ("malloc");
            return false;
        }

        bcopy (m, _arc->_b, _arc->b_n);
        m += _arc->b_n;

        this->l.add (_arc);
    }

    return true;
}

bool arc::save_file (const char *name_s) {
    uint8_t *_b;
    uint32_t b_n;
    uint32_t res_;
    file *_file;

    res_ = this->save_ser (&_b, &b_n);
    if (! res_) 
        return false;

    _file = new file (_b, b_n);
    if (! _file->_export (name_s)) {
        delete _file;
        return false;
    }

    delete _file;
    return true;
}

bool arc::save_ser (uint8_t **__b, uint32_t *_b_n) {
    arc::ive *_arc;
    uint8_t *m;
    uint32_t len;
    
    dlog::debug ("arc::save_ser");

    // calculate total archive data size
    len = 12;
    for (_arc = (arc::ive *) this->l.iter_init (); _arc; 
            _arc = (arc::ive *) this->l.iterate ())
        len += 8 + _arc->name_n + _arc->b_n;

    *__b = (uint8_t *) malloc (len);
    if (! *__b) {
        dlog::error ("malloc");
        return false;
    }
    *_b_n = len;

    m = *__b;

    bcopy ("N2ARCHIV", m, 8);
    m += 8;

    *(uint32_t *)m = this->l.item_n;
    m += 4;
    
    for (_arc = (arc::ive *) this->l.iter_init (); _arc; 
            _arc = (arc::ive *) this->l.iterate ()) {
        *(uint32_t *)m = _arc->name_n;
        m += 4;
        
        bcopy (_arc->name_s, m, _arc->name_n);
        m += _arc->name_n;
        
        *(uint32_t *)m = _arc->b_n;
        m += 4;
        
        bcopy (_arc->_b, m, _arc->b_n);
        m += _arc->b_n;
    }

    return true;
}

