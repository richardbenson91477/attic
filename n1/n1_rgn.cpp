
#include "n1.hpp"

void n1_rgn::init (n1_rgn::rgn *_rgn, uint32_t n) {
    rgn *_rgn2;
    int32_t x, y;
    uint32_t c;

    if (! _rgn)
        return;
    if (! n)
        return;

    _rgn2 = _rgn;
    for (c = 0; c < n; c++) {
        x = _rgn2->right - _rgn2->left;
        if (x < 0)
            _rgn2->w = x - 1;
        else
            _rgn2->w = x + 1;

        y = _rgn2->bottom - _rgn2->top;
        if (y < 0)
            _rgn2->h = y - 1;
        else
            _rgn2->h = y + 1;

        _rgn2->col_n = _rgn2->w / _rgn2->col_s;
        _rgn2->row_n = _rgn2->h / _rgn2->row_s;

        _rgn2 ++;
    }
}

uint32_t n1_rgn::classify (int32_t x, int32_t y, int32_t *o,
        n1_rgn::rgn *_rgn, uint32_t n) {
    rgn *_rgn2;
    int32_t ox, oy;
    uint32_t c;

    if (! n)
        return 0;
    
    _rgn2 = _rgn;
    for (c = 0; c < n; c++) {
        if ((x >= _rgn2->left) && (x <= _rgn2->right) &&
                (y >= _rgn2->top) && (y <= _rgn2->bottom)) {
            if (o) {
                ox = (x - _rgn2->left) / _rgn2->col_s;
                oy = (y - _rgn2->top) / _rgn2->row_s;
                *o = (_rgn2->col_n * oy) + ox;
            }
            return c + 1;
        }
        _rgn2 ++;
    }

    return 0;
}

bool n1_rgn::getcell (n1_rgn::rgn *_rgn, int32_t o, int32_t *top,
        int32_t *left, int32_t *bottom, int32_t *right) {
    int orr;

    if (! _rgn)
        return false;

    orr = o / _rgn->col_n;
    
    *left = ((o - (orr * _rgn->col_n)) * _rgn->col_s) + _rgn->left;
    *right = *left + (_rgn->col_s - 1);
    *top = (orr * _rgn->row_s) + _rgn->top;
    *bottom = *top + (_rgn->row_s - 1);

    return true;
}

void n1_rgn::render (uint32_t texid, n1_rgn::rgn *_texrg, int texo, 
        n1_rgn::rgn *_destrg, int desto) {
    int tt, tl, tb, tr;
    int dt, dl, db, dr;
    float ftt, ftl, ftb, ftr;
    float fdt, fdl, fdb, fdr;

    if (_texrg) {
        getcell (_texrg, texo, &tt, &tl, &tb, &tr);
        ftl = (float) tl / (float) (_texrg->w - 1);
        ftr = (float) tr / (float) (_texrg->w - 1);
        ftb = 1.0f - (float) tb / (float) (_texrg->h - 1);
        ftt = 1.0f - (float) tt / (float) (_texrg->h - 1);
    }
    else {
        ftl = 0.0f;
        ftr = 1.0f;
        ftb = 1.0f;
        ftt = 0.0f;
    }

    if (desto >= 0) {
        getcell (_destrg, desto, &dt, &dl, &db, &dr);
        fdl = (float) dl;
        fdr = (float) dr + 1.0f;
        fdb = (float) db;
        fdt = (float) dt + 1.0f;
    }
    else {
        fdl = (float) _destrg->left;
        fdr = (float) _destrg->right + 1.0f;
        fdb = (float) _destrg->bottom; 
        fdt = (float) _destrg->top + 1.0f;
    }

    glBindTexture (GL_TEXTURE_2D, texid);
    glBegin (GL_QUADS);

    glTexCoord2f (ftl, ftb);
    glVertex2f (fdl, fdb);
    glTexCoord2f (ftr, ftb);
    glVertex2f (fdr, fdb);
    glTexCoord2f (ftr, ftt);
    glVertex2f (fdr, fdt);
    glTexCoord2f (ftl, ftt);
    glVertex2f (fdl, fdt);

    glEnd ();
}

