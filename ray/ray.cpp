#include "ray.hpp"

ray::ray () {
    this->pre_init ();
    _ = true;
};

ray::~ray () {
    free (this->_rays);

    for (auto _light : this->lights)
        delete _light;

    for (auto _shape : this->shapes)
        delete _shape;
}

void ray::pre_init () {
    this->trace_rec_max = 3;
    this->_rays = nullptr;
}

void ray::render (uint32_t *draw_p) {
    const uint32_t x_lace_n = 3, y_lace_n = 3, lace_n = x_lace_n * y_lace_n;
    static uint32_t x_lace_c = 0, y_lace_c = 0, lace_c = 0;
    float lsh [3];

    lace_c = (lace_c + 1) % lace_n;
    x_lace_c = lace_c / x_lace_n;
    y_lace_c = lace_c % y_lace_n;

    auto _p = draw_p + (y_lace_c * this->w) + x_lace_c;
    auto _ray = _rays + (y_lace_c * this->w * 3) + (x_lace_c * 3);

    for (auto y = 0; y < (this->h / y_lace_n); y ++) {
        for (auto x = 0; x < (this->w / x_lace_n); x ++) {
            this->trace_rec_n = 0;
            if (! trace (nullptr, this->eye_v, _ray, lsh))
                *_p = 0;        
            else
                *_p = 0xff000000 +
                    (FTOB(lsh[2]) << 16) + (FTOB(lsh[1]) << 8) + FTOB(lsh[0]);

            _p += x_lace_n;
            _ray += x_lace_n * 3;
        }
        _p += (y_lace_n - 1) * this->w;
        _ray += (y_lace_n - 1) * this->w * 3;
    }
}

bool ray::calc_rays () {
    const float res_hfx = ((float) this->w) / 2.0;
    const float res_hfy = ((float) this->h) / 2.0;

    this->eye_v [0] = 0.0;
    this->eye_v [1] = 0.0;
    this->eye_v [2] = (float) this->h;

    if (_rays)
        free (_rays);

    _rays = (float *) malloc (sizeof (float) * 3 * this->w * this->h);
    if (! _rays)
        return false;
    
    auto _ray = _rays;
    auto co_y = -res_hfy;
    for (auto y = 0; y < this->h; y ++) {
        auto co_x = -res_hfx;
        for (auto x = 0; x < this->w; x ++) {
            _ray[0] = co_x;
            _ray[1] = co_y;
            _ray[2] = -this->eye_v [2];
            math::v_norm (_ray);
            _ray += 3;
            co_x += 1.0;
        }
        co_y += 1.0;
    }

    return true;
}

bool ray::ray_collide (const float *pos, const float *ray,
        const struct shape *_shape, float *d) {
    float pos_rel [3];

    switch (_shape->type) {
        case 0:
            math::v_sub2 (pos_rel, _shape->v, pos);
            return math::ray_to_sph (pos_rel, ray, _shape->rad_sq, d, nullptr);
            break;

        case 1:
            math::v_sub2 (pos_rel, pos, _shape->v);
            return math::ray_to_poly (pos_rel, ray, _shape->v1, _shape->v2,
                _shape->v3, _shape->n, d, nullptr);
            return false;
            break;
    }

    return false;
}

bool ray::trace (const struct shape *_shape_src, const float *pos,
        const float *ray, float *lsh) {
    shape *_shape_hit;
    float lsh2 [3];

    // find closest shape if any within ray
    _shape_hit = nullptr;
    float d, closest_d = FLT_MAX;
    for (auto _shape : this->shapes) {
        if (_shape == _shape_src)
            continue;

        if (! ray_collide (pos, ray, _shape, &d))
            continue;

        if (d < closest_d) {
            closest_d = d;
            _shape_hit = _shape;
        }
    }

    if (! _shape_hit)
        return false;

    // find intersection point
    float pos_isec [3];
    math::v_scalef2 (pos_isec, ray, closest_d);
    math::v_add (pos_isec, pos);

    // find normal at intersection
    float normal [3];
    if (_shape_hit->type == 0) {
        math::v_sub2 (normal, pos_isec, _shape_hit->v);
        math::v_divf (normal, _shape_hit->rad);
    }
    else {
        math::v_copy (normal, _shape_hit->n);
    }

    // ambient light
    math::v_copy (lsh, this->ambient);

    // texture
    if (_shape_hit->tex) {
        auto u = (atan2f (normal[0], normal[1])) / PI2;
        auto v = (atan2f (
            sqrtf (normal[2] * normal[2] + normal[0] * normal[0]),
                normal[1])) / PI2;
        auto tsh = 1.0 - v;
        math::v_scalef (lsh, 1.0 - tsh);
    }

    float ray_light [3];
    for (auto _light : this->lights) {
        // ray from light to just behind object
        math::v_sub2 (ray_light, pos_isec, _light->v);
        auto ray_light_len = math::v_len (ray_light);
        if (ray_light_len == 0.0)
            math::v_clear (ray_light);
        else
            math::v_divf (ray_light, ray_light_len);
        ray_light_len *= 0.9999;

        // check whether light is occluded by shape
        auto hit_ = false;
        for (auto _shape : this->shapes) {
            if (! ray_collide (_light->v, ray_light, _shape, &d))
                continue;

            // ignore if beyond current shape
            if (d > ray_light_len)
                continue;

            hit_ = true;
            break;
        }

        if (hit_)
            continue;

        // phong: Ka + Kd * (N dot L) + Ks * (N dot (L + V / 2))^n

        // diffuse
        auto dot = -math::v_dot (ray_light, normal);
        math::v_scalef2 (lsh2, _light->c, dot);
        math::v_add (lsh, lsh2);

        // specular
        if (_shape_hit->specular_exp) {
            math::v_add2 (lsh2, ray_light, ray);
            math::v_scalef (lsh2, 0.5);
            dot = math::v_dot (lsh2, normal);
            auto specular_pow = powf (dot, _shape_hit->specular_exp);
            math::v_scalef2 (lsh2, _shape_hit->specular, dot * specular_pow);
            math::v_add (lsh, lsh2);
        }
    }

    math::v_scale (lsh, _shape_hit->diffuse);
    math::v_scalef (lsh, 1.0 - _shape_hit->reflectivity);

    // reflections
    if ((_shape_hit->reflectivity > 0.0) &&
            (this->trace_rec_n < this->trace_rec_max)) {
        float ray_reflect [3];
        math::v_reflect (ray_reflect, ray, normal);

        this->trace_rec_n ++;
        if (trace (_shape_hit, pos_isec, ray_reflect, lsh2)) {
            math::v_scalef (lsh2, _shape_hit->reflectivity);
            math::v_add (lsh, lsh2);
        }
    }

    if (lsh[0] > 1.0)
        lsh[0] = 1.0;

    if (lsh[1] > 1.0)
        lsh[1] = 1.0;

    if (lsh[2] > 1.0)
        lsh[2] = 1.0;
    
    return true;
}

