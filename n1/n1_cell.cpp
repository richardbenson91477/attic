
#include "n1.hpp"

n1_cell::n1_cell () {
    this->cells = NULL;
    this->cell_n = 0;
}

uint32_t n1_cell::classify (const float *v) {
    cell *_cell;
    uint32_t c;

    if (! this->cell_n)
        return 0;

    _cell = this->cells;
    for (c = 0; c < this->cell_n; c ++) {
        if ((v[0] >= _cell->min_x) && (v[0] <= _cell->max_x) &&
                (v[1] >= _cell->min_y) && (v[1] <= _cell->max_y) &&
                (v[2] >= _cell->min_z) && (v[2] <= _cell->max_z)) {
            return c + 1;
        }
        _cell ++;
    }
    return 0;
}

// checks cell_c first
uint32_t n1_cell::classify2 (const float *v, uint32_t cell_c) {
    cell *_cell;
    uint32_t c;

    if (! this->cell_n)
        return 0;
    if (! cell_c)
        return 0;

    _cell = this->cells + (cell_c - 1);
    if ((v[0] >= _cell->min_x) && (v[0] <= _cell->max_x) &&
        (v[1] >= _cell->min_y) && (v[1] <= _cell->max_y) &&
        (v[2] >= _cell->min_z) && (v[2] <= _cell->max_z))
        return cell_c;

    for (c = 0; c < this->cell_n; c ++) {
        if (c + 1 == cell_c)
            continue;
        _cell = this->cells + c;

        if ((v[0] >= _cell->min_x) && (v[0] <= _cell->max_x) &&
            (v[1] >= _cell->min_y) && (v[1] <= _cell->max_y) &&
            (v[2] >= _cell->min_z) && (v[2] <= _cell->max_z))
            return c + 1;
    }
    return 0;
}

bool n1_cell::load (const char *file_s) {
    char b_in [PATH_MAX];
    FILE *f_in;
    cell *_cell;
    minpoly *_mpoly;
    n1_obj::otex *_otex;
    uint32_t tex_type, name_n, vertex_id;
    uint32_t c, c2;

    f_in = fopen (file_s, "rb");
    if (! f_in) {
        dlog::error ("cellmap not found", file_s);
        return false;
    }

    // header
    fread (b_in, 4, 1, f_in);
    if (strncmp (b_in, "MCM1", 4))
        return false;

    fread (&this->cell_n, 4, 1, f_in);
    this->cells = (cell *) malloc (sizeof(cell) * this->cell_n);
    for (c = 0; c < this->cell_n; c ++) {
        _cell = this->cells + c;

        //fread (&_cell->id, 4, 1, f_in);
        fseek (f_in, 4, SEEK_CUR);

        fread (&_cell->min_x, sizeof(float), 1, f_in);
        fread (&_cell->max_x, sizeof(float), 1, f_in);
        fread (&_cell->min_y, sizeof(float), 1, f_in);
        fread (&_cell->max_y, sizeof(float), 1, f_in);
        fread (&_cell->min_z, sizeof(float), 1, f_in);
        fread (&_cell->max_z, sizeof(float), 1, f_in);

        fread (&_cell->poly_n, 4, 1, f_in);

        _cell->_tnvs = (float *) malloc (24 * sizeof(float) * _cell->poly_n);
        fread (_cell->_tnvs, (24 * sizeof(float)) * _cell->poly_n, 1, f_in);

        _cell->_mpolys = (minpoly *) malloc (_cell->poly_n *
                sizeof(minpoly));
        for (c2 = 0; c2 < _cell->poly_n; c2 ++) {
            _mpoly = _cell->_mpolys + c2;

            fread (&vertex_id, 4, 1, f_in);
            _mpoly->_v1 = _cell->_tnvs + vertex_id;

            fread (&vertex_id, 4, 1, f_in);
            _mpoly->_v2 = _cell->_tnvs + vertex_id;

            fread (&vertex_id, 4, 1, f_in);
            _mpoly->_v3 = _cell->_tnvs + vertex_id;

            // needed because _tnvs contains vertex normals
            fread (_mpoly->n, sizeof(float) * 3, 1, f_in);
            // calc plane D
            _mpoly->n[3] = -math::v_dot (_mpoly->_v1, _mpoly->n);
        }

        fread (&_cell->otex_n, 4, 1, f_in);

        if (_cell->otex_n)
            _cell->_otexs = (n1_obj::otex *) malloc (_cell->otex_n *
                sizeof(n1_obj::otex));
        else
            _cell->_otexs = NULL;

        for (c2 = 0; c2 < _cell->otex_n; c2 ++) {
            _otex = _cell->_otexs + c2;

            _otex->i1 = 0;
            fread (&_otex->i1, 4, 1, f_in);
            _otex->i2 = 0;
            fread (&_otex->i2, 4, 1, f_in);

            tex_type = 0;
            tex_type = fgetc (f_in);
            name_n = 0;
            name_n = fgetc (f_in);

            fread (b_in, name_n, 1, f_in);
            b_in[name_n] = 0;
            if (! tex_type) {
                _otex->_tex = _app->disp.tex_load (b_in);
                _otex->psh = NULL;
            }
            else {
                _otex->psh = _app->texshader.load (b_in);
                _otex->_tex = NULL;
            }
        }

        fread (&_cell->block_n, 4, 1, f_in);
        _cell->bcenter = (float *) malloc (_cell->block_n * 3 * sizeof(float));
        fread (_cell->bcenter, sizeof(float), _cell->block_n * 3, f_in);

        _cell->bsphere = (float *) malloc (_cell->block_n * sizeof(float));
        fread (_cell->bsphere, sizeof(float), _cell->block_n, f_in);
    }

    fclose (f_in);

    dlog::debug ("cellmap load", file_s);
    return true;
}

void n1_cell::destroy_all () {
    cell *_cell;
    n1_obj::otex *_otex;
    uint32_t c, c2;

    _cell = this->cells;
    for (c = 0; c < this->cell_n; c++) {
        if (_cell->_tnvs)
            free (_cell->_tnvs);
        if (_cell->_mpolys)
            free (_cell->_mpolys);
        if (_cell->_otexs) {
            _otex = _cell->_otexs;
            for (c2 = 0; c2 < _cell->otex_n; c2 ++) {
                if (_otex->_tex)
                    _app->disp.tex_del (_otex->_tex);
                else if (_otex->psh)
                    _app->texshader.del (_otex->psh);
                _otex ++;
            }

            free (_cell->_otexs);
        }
        if (_cell->bcenter)
            free (_cell->bcenter);
        if (_cell->bsphere)
            free (_cell->bsphere);
        _cell ++;
    }

    free (this->cells);
    this->cells = NULL;
    this->cell_n = 0;
}

void n1_cell::render () {
    n1_phys::ent *_ent;
    cell *_cell;
    n1_obj::otex *_otex;
    float pos_rel [3];
    uint32_t cell_c;
    uint32_t c2;
    uint32_t iter;

    cell_c = this->classify (_app->disp.eye_pos);
    if (! cell_c) {
        dlog::error ("n1_cell::render", "cell_c == 0");
        return;
    }

    _cell = this->cells + (cell_c - 1);

    // cellmap
    if (_cell) {
        glInterleavedArrays (GL_T2F_N3F_V3F, 0, _cell->_tnvs);

        for (c2 = 0; c2 < _cell->otex_n; c2 ++) {
            _otex = _cell->_otexs + c2;

            if (_otex->_tex)
                glBindTexture (GL_TEXTURE_2D, _otex->_tex->gl_texid);
            else if (_otex->psh)
                _app->texshader.apply (_otex->psh);

            glDrawArrays (GL_TRIANGLES, _otex->i1 * 3, _otex->i2 * 3);

            if (_otex->psh)
                _app->texshader.unapply (_otex->psh);
        }
    }

    // ents
    for (_ent = (n1_phys::ent *) _app->phys.ents.iter_init (true, &iter);
            _ent; _ent = (n1_phys::ent *) _app->phys.ents.iterate ()) {
        if (_ent->cell != cell_c)
            continue;
        if (_ent->dont_render)
            continue;
        if (! _ent->_obj)
            continue;

        if (_app->disp.clip (_ent->pos) < -_ent->_obj->brad)
            continue;
        
        glPushMatrix ();
        glTranslatef (_ent->pos[0], _ent->pos[1], _ent->pos[2]);

        math::v_sub2 (pos_rel, _app->disp.eye_pos, _ent->pos);
        if (_ent->rotate_)
            _ent->_obj->render (pos_rel, _ent->rmat);
        else
            _ent->_obj->render (pos_rel, NULL);

        glPopMatrix ();
    }
    _app->phys.ents.iter_init (false, &iter);
}

