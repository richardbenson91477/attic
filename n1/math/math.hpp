#ifndef MATH_INCLUDED

#include <cstdlib>
#include <cstdint>
#include <cfloat>
#include <cmath>

#include "../ndef.hpp"

// NOTE: not a class
namespace math {

    void m_ident (float (*mo)[4]);
    void m_copy (float (*mo)[4], const float (*m)[4]);
    void m_concat (float (*mo)[4], const float (*m1)[4], const float (*m2)[4]);
    void m_invert (float (*mo)[4], const float (*m)[4]);
    void m_from_r (float (*mo)[4], const float *r);
    void m_from_p (float (*mo)[4], const float *p);
    void m_from_r_p (float (*mo)[4], const float *r, const float *p);
    void r_from_m (float *vo, const float (*m)[4]);
    void v_rotate_m (float *v, const float (*m)[4]);
    void v_rotate_m2 (float *vo, const float *v, const float (*m)[4]);
    void v_list_rotate_m2 (float *vo, const float *v, uint32_t nv, 
        const float (*m)[4]);
    void v_apply_m (float *v, const float (*m)[4]);
    void v_apply_m2 (float *vo, const float *v, const float (*m)[4]);
    void v_inv_rotate_m (float *v, const float (*m)[4]);
    void v_inv_rotate_m2 (float *vo, const float *v, const float (*m)[4]);
    void v_inv_apply_m (float *v, const float (*m)[4]);
    void v_inv_apply_m2 (float *vo, const float *v, const float (*m)[4]);
 
    void m_from_q (float (*mat)[4], const float *quat);
    void q_from_r (float *quat, const float *rot);
    void q_slerp (float *res, const float *p, float *q, float t);

    float randf (float min, float max);
    float a_shift (float begin, float end, float rate);

    bool pt_in_box (const float *p, const float *hb2);
    bool pt_in_sph (const float *p, float sr2);
    bool ray_to_sph (const float *p, const float *ray, float sr2,
        float *t1, float *t2);
    bool ray_to_poly (const float *p, const float *ray,
        const float *_v1, const float *_v2, const float *_v3, const float *pn, 
        float *t_out, float *isec);
    float pt_on_line_closest_to_pt (const float *p,
        const float *v1, const float *v2, float *p_out);
    float pt_on_line_closest_to_pt (const float *p,
        const float *v1, const float *v2);
    bool sph_in_sph (const float *p, float r1, float r2);
    bool sph_in_plane (const float *p, float r, const float *pl);
    bool sph_to_poly (const float *p, float r, const float *ray, float move_t,
        const float *v1, const float *v2, const float *v3, const float *pl,
        float *t_out, float *isec_out);
    bool sph_to_poly (const float *p, float r, const float *ray, float move_t,
        const float *v1, const float *v2, const float *v3, const float *pl);

    inline void v_scale (float *vo, const float *s) {
        vo[0] *= s[0]; vo[1] *= s[1]; vo[2] *= s[2];
    }
    inline void v_scale2 (float *vo, const float *v, const float *s) {
        vo[0] = v[0] * s[0]; vo[1] = v[1] * s[1]; vo[2] = v[2] * s[2];
    }
    inline void v_scalef (float *vo, float s) {
        vo[0] *= s; vo[1] *= s; vo[2] *= s;
    }
    inline void v_scalef2 (float *vo, const float *v, float s) {
        vo[0] = v[0] * s; vo[1] = v[1] * s; vo[2] = v[2] * s;
    }
    inline void v_div (float *vo, const float *s) {
        vo[0] /= s[0]; vo[1] /= s[1]; vo[2] /= s[2];
    }
    inline void v_div2 (float *vo, const float *v, const float *s) {
        vo[0] = v[0] / s[0]; vo[1] = v[1] / s[1]; vo[2] = v[2] / s[2];
    }
    inline void v_divf (float *vo, float s) {
        vo[0] /= s; vo[1] /= s; vo[2] /= s;
    }
    inline void v_divf2 (float *vo, const float *v, float s) {
        vo[0] = v[0] / s; vo[1] = v[1] / s; vo[2] = v[2] / s;
    }
    inline void v_add (float *vo, const float *v) {
        vo[0] += v[0]; vo[1] += v[1]; vo[2] += v[2];
    }
    inline void v_add2 (float *vo, const float *v1, const float *v2) {
        vo[0] = v1[0] + v2[0]; vo[1] = v1[1] + v2[1]; vo[2] = v1[2] + v2[2];
    }
    inline void v_addf (float *vo, float f) {
        vo[0] += f; vo[1] += f; vo[2] += f;
    }
    inline void v_addf2 (float *vo, const float *v, float f) {
        vo[0] = v[0] + f; vo[1] = v[1] + f; vo[2] = v[2] + f;
    }
    inline void v_sub (float *vo, const float *v) {
        vo[0] -= v[0]; vo[1] -= v[1]; vo[2] -= v[2];
    }
    inline void v_sub2 (float *vo, const float *v1, const float *v2) {
        vo[0] = v1[0] - v2[0]; vo[1] = v1[1] - v2[1]; vo[2] = v1[2] - v2[2];
    }
    inline void v_subf (float *vo, float f) {
        vo[0] -= f; vo[1] -= f; vo[2] -= f;
    }
    inline void v_subf2 (float *vo, const float *v, float f) {
        vo[0] = v[0] - f; vo[1] = v[1] - f; vo[2] = v[2] - f;
    }
    inline void v_neg (float *vo) {
        vo[0] = -vo[0]; vo[1] = -vo[1]; vo[2] = -vo[2];
    }
    inline void v_neg2 (float *vo, const float *v) {
        vo[0] = -v[0]; vo[1] = -v[1]; vo[2] = -v[2];
    }
    inline void v_copy (float *vo, const float *v) {
        vo[0] = v[0]; vo[1] = v[1]; vo[2] = v[2];
    }
    inline bool v_is_clear (const float *v) {
        return ((v[0] == 0.0f) && (v[1] == 0.0f) && (v[2] == 0.0f)) ? 
            true : false;
    }
    inline void v_clear (float *vo) {
        vo[0] = vo[1] = vo[2] = 0.0f;
    }
    inline void v_setf (float *vo, float f) {
        vo[0] = vo[1] = vo[2] = f;
    }
    inline float v_len (const float *v) {
        return (sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
    }
    inline bool v_equal (const float *v1, const float *v2) {
        return ((v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2])) ?
            true : false;
    }
    inline bool v_similar (const float *v1, const float *v2, float e) {
        float d [3];
        v_sub2 (d, v1, v2);
        return ((d[0] < e) && (d[0] > -e) && \
                (d[1] < e) && (d[1] > -e) && \
                (d[2] < e) && (d[2] > -e)) ? 
                true : false;
    }
    inline void v_norm (float *vo) {
        float len = sqrt (vo[0] * vo[0] + vo[1] * vo[1] + vo[2] * vo[2]);
        if (len == 0.0f) {
            vo[0] = 0.0f; vo[1] = 0.0f; vo[2] = 0.0f;
        }
        else {
            vo[0] /= len; vo[1] /= len; vo[2] /= len;
        }
    }
    inline void v_norm2 (float *vo, const float *v) {
        float len = sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        if (len == 0.0f) {
            vo[0] = 0.0f; vo[1] = 0.0f; vo[2] = 0.0f;
        }
        else {
            vo[0] = v[0] / len; vo[1] = v[1] / len; vo[2] = v[2] / len;
        }
    }
    // returns the cosine of the angle between v1 and v2
    inline float v_dot (const float *v1, const float *v2) {
        return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
    }
    // returns a vector which is normal to the plane containing vectors v1, v2
    // NOTE: if sign ((b - a) cross (c - a)) == sign ((b - a) cross (d - a))
        // then d is on the same side of line (b - a) as c
    inline void v_cross (float *vo, const float *v1, const float *v2) {
        vo[0] = v1[1] * v2[2] - v1[2] * v2[1];
        vo[1] = v1[2] * v2[0] - v1[0] * v2[2];
        vo[2] = v1[0] * v2[1] - v1[1] * v2[0];
    }
    inline void v_reflect (float *vo, const float *v, const float *n) {
        float dot = -(v_dot (n, v)) * 2.0f;
        v_scalef2 (vo, n, dot);
        v_add (vo, v);
    }
    inline void poly_normal (float *pl, const float *v1, const float *v2, 
            const float *v3) {
        float a [3], b [3];
        v_sub2 (a, v1, v2);
        v_sub2 (b, v3, v2);
        v_cross (pl, a, b);
        v_norm (pl);
    }
    inline void poly_plane (float *pl, const float *v1, const float *v2, 
            const float *v3) {
        float a [3], b [3];
        v_sub2 (a, v1, v2);
        v_sub2 (b, v3, v2);
        v_cross (pl, a, b);
        v_norm (pl);
        pl[3] = -v_dot (v1, pl);
    }
    inline void v_from_spherical (float *vo, float r, float p, float t) {
        vo[0] = r * sinf (t) * sinf (p);
        vo[1] = r * cosf (p);
        vo[2] = r * cosf (t) * sinf (p);
    }
    inline void a_bound (float *a) {
        while (*a < 0.0f) *a += PI2;
        while (*a >= PI2) *a -= PI2;
    }
    inline void v_a_bound (float *vo) {
        a_bound (vo);
        a_bound (vo + 1);
        a_bound (vo + 2);
    }
    inline void spherical_from_v (const float *v, float *r, float *p, float *t){
        if (r)
            *r = v_len (v);
        if (t) {
            *t = atan2f (v[0], v[2]);
            a_bound (t);
        }
        if (p) {
            *p = atan2f (sqrtf (v[2] * v[2] + v[0] * v[0]), v[1]);
            a_bound (p);
        }
    }
    inline float plane_dist (const float *p, const float *pl) {
        return v_dot (p, pl) + pl[3];
    }
    inline float v_list_farthest (const float *v, uint32_t nv) {
        const float *pv;
        float l, max;
        uint32_t c;
        for (c = 0, pv = v, max = 0.0f; c < nv; c ++) {
            l = v_len (pv);
            if (l > max)
                max = l;
            pv += 3;
        }
        return max;
    }
    inline void v_slide (float *vo, const float *v, const float *n) {
        float dot = -(v_dot (n, v));
        v_scalef2 (vo, n, dot);
        v_add (vo, v);
    }
};

#define MATH_INCLUDED
#endif

