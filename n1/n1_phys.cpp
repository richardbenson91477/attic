#include "n1.hpp"

bool n1_phys::init (bool t_) {
    if (t_) {
        this->wall_eps = 0.1f;
    }
    else {
        this->ents.destroy (blaze::hook_free);
    }
    return true;
}

n1_phys::ent *n1_phys::ent_add () {
    float tmp [3];
    ent *_ent;

    _ent = (ent *) malloc (sizeof(ent));
    if (! _ent)
        return NULL;

    memset (_ent, 0, sizeof(ent));
    math::v_clear (tmp);
    math::m_from_r (_ent->rmat, tmp);

    this->ents.insert (_ent, 0);
    return _ent;
}

bool n1_phys::collide_all (const float *p, const float r, const float *ray,
        ecol *_col, ent *_ent_skip) {
    const uint32_t hit_max_n = 4;
    ecol cols [hit_max_n];
    ent *_ent;
    n1_obj::poly *_poly;
    n1_cell::cell *_cell;
    n1_cell::minpoly *_mpoly;
    float p_new [3], ray_len, ray_norm [3], v [3], v_rel [3];
    uint32_t hit_n, block_c, cell, mpoly_from_c, mpoly_to_c;
    float *block_bcenter, block_bsphere;
    float closest_len, len, ray_t;
    uint32_t closest_col, c, c2;
    uint32_t iter;

    memset (cols, 0, sizeof(cols));
    hit_n = 0;

    math::v_add2 (p_new, p, ray);
    ray_len = math::v_len (ray);
    math::v_norm2 (ray_norm, ray);

    cell = _app->cell.classify (p);
    if (! cell)
        return false;

    _cell = _app->cell.cells + (cell - 1);
    if (! _cell)
        return false;

    for (block_c = 0; block_c < _cell->block_n; block_c ++) {
            block_bcenter = _cell->bcenter + block_c * 3;
            block_bsphere = _cell->bsphere[block_c];
        if (r > 0.0f) {
            math::v_sub2 (v_rel, p_new, block_bcenter);
            // TODO: replace with sweeping sph_to_sph
            if (! math::sph_in_sph (v_rel, r, block_bsphere))
                continue;
        }
        else {
            math::v_sub2 (v_rel, block_bcenter, p);
            if (! math::pt_in_sph (v_rel, block_bsphere)) {
                if (! math::ray_to_sph (v_rel, ray_norm, block_bsphere,
                        &ray_t, NULL))
                    continue;

                if (ray_t > ray_len)
                    continue;
            }
        }

        mpoly_from_c = block_c * _app->cell.poly_block_n;
        mpoly_to_c = (block_c + 1) * _app->cell.poly_block_n;
        if (mpoly_to_c > _cell->poly_n)
            mpoly_to_c = _cell->poly_n;

        for (c2 = mpoly_from_c; c2 < mpoly_to_c; c2 ++) {
            _mpoly = _cell->_mpolys + c2;
            if (r > 0.0f) {
                if (! math::sph_to_poly (p, r, ray_norm, ray_len,
                        _mpoly->_v1, _mpoly->_v2, _mpoly->_v3,
                        _mpoly->n, NULL, v))
                    continue;
            }
            else {
                if (math::ray_to_poly (p, ray_norm, _mpoly->_v1,
                        _mpoly->_v2, _mpoly->_v3, _mpoly->n, &ray_t, v)){
                    if (ray_t > ray_len)
                        continue;
                }
                else
                    continue;
            }

            math::v_copy (cols[hit_n].isec, v);
            math::v_copy (cols[hit_n].pn_hit, _mpoly->n);
            cols[hit_n].hit_type = TYPE_CELL;
            cols[hit_n].hit_parent = _cell;
            cols[hit_n]._hit_ent = NULL;

            hit_n ++;
            if (hit_n == hit_max_n)
                break;
        }
        if (hit_n == hit_max_n)
            break;
    }

    // ents
    if (hit_n < hit_max_n) {
        for (_ent = (ent *) ents.iter_init (true, &iter); _ent;
                _ent = (ent *) ents.iterate ()) {
            if (_ent->cell != cell)
                continue;
            if (_ent == _ent_skip)
                continue;
            if (! _ent->type)
                continue;
   
            if (r > 0.0f) {
                math::v_sub2 (v_rel, p_new, _ent->pos);
                // TODO: replace with sweeping sph_to_sph
                if (! math::sph_in_sph (v_rel, r, _ent->_obj->brad))
                    continue;
            }
            else {
                math::v_sub2 (v_rel, _ent->pos, p);
                if (! math::pt_in_sph (v_rel, _ent->_obj->pre_sbrad)) {
                    if (! math::ray_to_sph (v_rel, ray_norm, 
                            _ent->_obj->pre_sbrad, &ray_t, NULL))
                        continue;
                    if (ray_t > ray_len)
                        continue;
                }
            }

            if (_ent->bound_only) {
                math::v_scalef2 (cols[hit_n].isec, ray_norm, ray_t);
                math::v_add (cols[hit_n].isec, p);
                math::v_norm2 (cols[hit_n].pn_hit, v_rel);
                cols[hit_n].hit_type = _ent->type;
                cols[hit_n].hit_parent = _ent->parent;
                cols[hit_n]._hit_ent = _ent;

                hit_n ++;
                if (hit_n == hit_max_n)
                    break;
                continue;
            }

            math::v_sub2 (v_rel, p, _ent->pos);
            for (c2 = 0; c2 < _ent->_obj->poly_n; c2 ++) {
                    _poly = _ent->_obj->_polys + c2;
                if (r > 0.0f) {
                    if (! math::sph_to_poly (p, r, ray_norm, ray_len,
                            _poly->_v1, _poly->_v2, _poly->_v2, _poly->n,
                            NULL, v))
                        continue;
                }
                else {
                    if (math::ray_to_poly (v_rel, ray_norm,
                            _poly->_v1, _poly->_v2, _poly->_v3, _poly->n,
                            &ray_t, v)) {
                        if (ray_t > ray_len)
                            continue;
                        }
                    else
                        continue;
                }

                math::v_add (v, _ent->pos);
                math::v_copy (cols[hit_n].isec, v);
                math::v_copy (cols[hit_n].pn_hit, _poly->n);
                cols[hit_n].hit_type = _ent->type;
                cols[hit_n].hit_parent = _ent->parent;
                cols[hit_n]._hit_ent = _ent;

                hit_n ++;
                if (hit_n == hit_max_n)
                    break;
            }
            if (hit_n == hit_max_n)
                break;
        }
        this->ents.iter_init (false, &iter);
    }

    if (hit_n) { 
        closest_col = 0;
        if (hit_n > 1) {
            math::v_sub2 (v_rel, p, cols[0].isec);
            closest_len = math::v_len (v_rel);

            for (c = 1; c < hit_n; c ++) {
                math::v_sub2 (v_rel, p, cols[c].isec);
                len = math::v_len (v_rel);
                if (len < closest_len) {
                    closest_len = len;
                    closest_col = c;
                }
            }
        }

        math::v_copy (_col->isec, cols[closest_col].isec);
        math::v_copy (_col->pn_hit, cols[closest_col].pn_hit);
        _col->hit_type = cols[closest_col].hit_type;
        _col->hit_parent = cols[closest_col].hit_parent;
        _col->_hit_ent = cols[closest_col]._hit_ent;
        return true;
    }

    return false;
}

bool n1_phys::collide_ray_all (const float *p, const float *ray,
        ecol *_col, ent *_ent_skip) {
    return this->collide_all (p, 0.0f, ray, _col, _ent_skip);
}

bool n1_phys::tick (float t) {
    ent *_ent;
    float ray [3];
    uint32_t iter;

    for (_ent = (ent *) ents.iter_init (true, &iter); _ent;
            _ent = (ent *) ents.iterate ()) {
        if (! _ent->type)
            continue;

        if (_ent->mov_rho) {
            math::v_scalef2 (ray, _ent->mov_v, _ent->mov_rho * t);
            move (_ent, ray);
        }
        if (_ent->weight)
            fall (_ent, t);
        if (_ent->jumping)
            jump (_ent, t);
    }
    ents.iter_init (false, &iter);
    return true;
}

bool n1_phys::move (ent *_ent, float *ray) {
    ecol col;
    bool res_;

    res_ = collide_all (_ent->pos, _ent->_obj->brad, ray, &col, _ent);
    if (res_) {
        col.col_type = COL_MOVE;
        col._ent = _ent;
        callback (_ent->type, &col);
        return res_;
    }

    math::v_add (_ent->pos, ray);
    _ent->cell = _app->cell.classify2 (_ent->pos, _ent->cell);
    return false;
}

void n1_phys::fall (ent *_ent, float t) {
    static float ray [3] = { 0.0f, 0.0f, 0.0f };
    ecol col;
    static const bool gravity_ = true;
    bool res_;

    if (! gravity_) 
        return;

    if (! _ent->falling)
        _ent->fall_t = 0.0f;
        
    _ent->fall_t += _ent->weight * t * t;

    ray[1] = -_ent->fall_t;
    res_ = collide_all (_ent->pos, _ent->_obj->brad, ray, &col, _ent);
    if (res_) {
        if (_ent->falling) {
            _ent->falling = false;
            col.col_type = COL_GROUND;
            col._ent = _ent;
            callback (_ent->type, &col);
        }
    }
    else {
        math::v_add (_ent->pos, ray);
        _ent->cell = _app->cell.classify2 (_ent->pos, _ent->cell);
        if (! _ent->falling) {
            _ent->falling = true;
            col.col_type = COL_FALL;
            col._ent = _ent;
            callback (_ent->type, &col);
        }
    }
}

void n1_phys::jump (ent *_ent, float t) {
    ecol col;
    float ray [3] = { 0.0f, 0.0f, 0.0f };
    bool res_;

    if (! _ent->jumping) {
        _ent->jumping = true;
        _ent->jump_t = 0.0f;
    }
        
    ray[1] = t * _ent->jump_r;
 
    _ent->jump_t += t;
    if (_ent->jump_t > 0.5f) {
        _ent->jumping = false;
        return;
    }

    res_ = collide_all (_ent->pos, _ent->_obj->brad, ray, &col, _ent);
    if (res_) {
        _ent->jumping = false;
        col.col_type = COL_HEAD;
        col._ent = _ent;
        callback (_ent->type, &col);
    }
    else {
        _ent->pos[1] += ray[1];
        _ent->cell = _app->cell.classify2 (_ent->pos, _ent->cell);
    }
}

void n1_phys::callback (uint32_t type, ecol *_col) {
    if (type == TYPE_IGNORE)
        return;
    if (this->cb_table[type - 1].callback)
        this->cb_table[type - 1].callback (_col);
}

void n1_phys::set_callback (uint32_t type,
        void (*callback) (const ecol *_col)) {
    if (type == TYPE_IGNORE)
        return;
    this->cb_table[type - 1].callback = callback;
}

