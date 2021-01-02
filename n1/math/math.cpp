
#include "math.hpp"

namespace math {

void m_ident (float (*mo)[4]) {
    mo[0][0] = mo[1][1] = mo[2][2] = 1.0f;
    mo[0][1] = mo[0][2] = mo[0][3] = 0.0f;
    mo[1][0] = mo[1][2] = mo[1][3] = 0.0f;
    mo[2][0] = mo[2][1] = mo[2][3] = 0.0f;
}

void m_copy (float (*mo)[4], const float (*m)[4]) {
    mo[0][0] = m[0][0]; mo[0][1] = m[0][1]; mo[0][2] = m[0][2]; 
    mo[0][3] = m[0][3];
    mo[1][0] = m[1][0]; mo[1][1] = m[1][1]; mo[1][2] = m[1][2]; 
    mo[1][3] = m[1][3];
    mo[2][0] = m[2][0]; mo[2][1] = m[2][1]; mo[2][2] = m[2][2]; 
    mo[2][3] = m[2][3];
}

void m_concat (float (*mo)[4], const float (*m1)[4],
        const float (*m2)[4]) {
    mo[0][0] = m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0] + m1[0][2] * m2[2][0];
    mo[0][1] = m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1] + m1[0][2] * m2[2][1];
    mo[0][2] = m1[0][0] * m2[0][2] + m1[0][1] * m2[1][2] + m1[0][2] * m2[2][2];
    mo[0][3] = m1[0][0] * m2[0][3] + m1[0][1] * m2[1][3] + m1[0][2] * m2[2][3]
        + m1[0][3];
    mo[1][0] = m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0] + m1[1][2] * m2[2][0];
    mo[1][1] = m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1] + m1[1][2] * m2[2][1];
    mo[1][2] = m1[1][0] * m2[0][2] + m1[1][1] * m2[1][2] + m1[1][2] * m2[2][2];
    mo[1][3] = m1[1][0] * m2[0][3] + m1[1][1] * m2[1][3] + m1[1][2] * m2[2][3] 
        + m1[1][3];
    mo[2][0] = m1[2][0] * m2[0][0] + m1[2][1] * m2[1][0] + m1[2][2] * m2[2][0];
    mo[2][1] = m1[2][0] * m2[0][1] + m1[2][1] * m2[1][1] + m1[2][2] * m2[2][1];
    mo[2][2] = m1[2][0] * m2[0][2] + m1[2][1] * m2[1][2] + m1[2][2] * m2[2][2];
    mo[2][3] = m1[2][0] * m2[0][3] + m1[2][1] * m2[1][3] + m1[2][2] * m2[2][3] 
        + m1[2][3];
}

void m_invert (float (*mo)[4], const float (*m)[4]) {
    mo[0][0] = m[0][0]; mo[0][1] = m[1][0]; mo[0][2] = m[2][0];
    mo[1][0] = m[0][1]; mo[1][1] = m[1][1]; mo[1][2] = m[2][1];
    mo[2][0] = m[0][2]; mo[2][1] = m[1][2]; mo[2][2] = m[2][2];
    mo[0][3] = m[0][3]; mo[1][3] = m[1][3]; mo[2][3] = m[2][3];
}

void m_from_r (float (*mo)[4], const float *r) {
    float s0, s1, s2, c0, c1, c2;

    s0 = sinf (r[0]); c0 = cosf (r[0]);
    s1 = sinf (r[1]); c1 = cosf (r[1]);
    s2 = sinf (r[2]); c2 = cosf (r[2]);

    mo[0][0] = c1 * c2;
    mo[1][0] = c1 * s2;
    mo[2][0] = -s1;
    mo[0][1] = s0 * s1 * c2 + c0 *-s2;
    mo[1][1] = s0 * s1 * s2 + c0 * c2;
    mo[2][1] = s0 * c1;
    mo[0][2] = c0 * s1 * c2 + s0 * s2;
    mo[1][2] = c0 * s1 * s2 + -s0 * c2;
    mo[2][2] = c0 * c1;
    
    mo[0][3] = 0.0f; mo[1][3] = 0.0f; mo[2][3] = 0.0f;
}

void m_from_p (float (*mo)[4], const float *p) {
    mo[0][3] = p[0];
    mo[1][3] = p[1];
    mo[2][3] = p[2];
}

void m_from_r_p (float (*mo)[4], const float *r, const float *p) {
    m_from_r (mo, r);
    m_from_p (mo, p);
}

// NOTE: what you get out is generally not what you put in
void r_from_m (float *r, const float (*m)[4]) {
    float cy = sqrt (m[0][0] * m[0][0] + m[1][0] * m[1][0]);

    if (cy > 16 * FLT_EPSILON) {
        r[0] = atan2f (m[2][1], m[2][2]);
        r[1] = atan2f (-m[2][0], cy);
        r[2] = atan2f (m[1][0], m[0][0]);
    } 
    else {
        r[0] = atan2f (-m[1][2], m[1][1]);
        r[1] = atan2f (-m[2][0], cy);
        r[2] = 0.0f;
    }
}

void v_rotate_m (float *v, const float (*m)[4]) {
    float v2 [3];
    v_rotate_m2 (v2, v, m);
    v_copy (v, v2);
}

void v_rotate_m2 (float *vo, const float *v, const float (*m)[4]) {
    vo[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2];
    vo[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2];
    vo[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2];
}

void v_list_rotate_m2 (float *vo, const float *v, uint32_t nv, 
        const float (*m)[4]) {
    float *_vo;
    const float *_v;
    uint32_t c;

    for (c = 0, _vo = vo, _v = v; c < nv; c ++) {
        v_rotate_m2 (_vo, _v, m);
        _vo += 3;
        _v += 3;
    }
}

void v_apply_m (float *v, const float (*m)[4]) {
    float v2 [3];
    v_apply_m2 (v2, v, m);    
    v_copy (v, v2);
}

void v_apply_m2 (float *vo, const float *v, const float (*m)[4]) {
    vo[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + m[0][3];
    vo[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + m[1][3];
    vo[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + m[2][3];
}

void v_inv_rotate_m (float *v, const float (*m)[4]) {
    float v2 [3];
    v_inv_rotate_m2 (v2, v, m);
    v_copy (v, v2);
}

void v_inv_rotate_m2 (float *vo, const float *v, const float (*m)[4]) {
    vo[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0];
    vo[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1];
    vo[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2];
}

void v_inv_apply_m (float *v, const float (*m)[4]) {
    float v2 [3];
    v_inv_apply_m2 (v2, v, m);
    v_copy (v, v2);
}

void v_inv_apply_m2 (float *vo, const float *v, const float (*m)[4]) {
    float v2 [3];
    v2[0] = v[0] - m[0][3];
    v2[1] = v[1] - m[1][3];
    v2[2] = v[2] - m[2][3];
    vo[0] = v2[0] * m[0][0] + v2[1] * m[1][0] + v2[2] * m[2][0];
    vo[1] = v2[0] * m[0][1] + v2[1] * m[1][1] + v2[2] * m[2][1];
    vo[2] = v2[0] * m[0][2] + v2[1] * m[1][2] + v2[2] * m[2][2];
}

void m_from_q (float (*mat)[4], const float *quat) {
    float tq0, tq1, tq2, tq3;

    tq0 = 2.0f * quat[0];
    tq1 = 2.0f * quat[1];
    tq2 = 2.0f * quat[2];
    tq3 = 2.0f * quat[3];
    mat[0][0] = 1.0f - tq1 * quat[1] - tq2 * quat[2];
    mat[1][0] = tq0 * quat[1] + tq3 * quat[2];
    mat[2][0] = tq0 * quat[2] - tq3 * quat[1];
    mat[0][1] = tq0 * quat[1] - tq3 * quat[2];
    mat[1][1] = 1.0f - tq0 * quat[0] - tq2 * quat[2];
    mat[2][1] = tq1 * quat[2] + tq3 * quat[0];
    mat[0][2] = tq0 * quat[2] + tq3 * quat[1];
    mat[1][2] = tq1 * quat[2] - tq3 * quat[0];
    mat[2][2] = 1.0f - tq0 * quat[0] - tq1 * quat[1];
}

void q_from_r (float *quat, const float *rot) {
    float a;
    float s0, s1, s2, c0, c1, c2;
    float s1s2, c0c1, s0c1, s1c2;

    a = rot[0] * 0.5f;
    s0 = sinf (a);
    c0 = cosf (a);
    a = rot[1] * 0.5f;
    s1 = sinf (a);
    c1 = cosf (a);
    a = rot[2] * 0.5f;
    s2 = sinf (a);
    c2 = cosf (a);
    s1s2 = s1 * s2;
    c0c1 = c0 * c1;
    s0c1 = s0 * c1;
    s1c2 = s1 * c2;
    quat[0] = s0c1 * c2 - s1s2 * c0; // x
    quat[1] = s1c2 * c0 + s0c1 * s2; // y
    quat[2] = c0c1 * s2 - s1c2 * s0; // z
    quat[3] = c0c1 * c2 + s1s2 * s0; // w
}

void q_slerp (float *res, const float *p, float *q, float t) {
    float omega, cosom, sinom, sclp, sclq;
    float a = 0.0f, b = 0.0f;

    a += (p[0] - q[0]) * (p[0] - q[0]);
    b += (p[0] + q[0]) * (p[0] + q[0]);
    a += (p[1] - q[1]) * (p[1] - q[1]);
    b += (p[1] + q[1]) * (p[1] + q[1]);
    a += (p[2] - q[2]) * (p[2] - q[2]);
    b += (p[2] + q[2]) * (p[2] + q[2]);
    a += (p[3] - q[3]) * (p[3] - q[3]);
    b += (p[3] + q[3]) * (p[3] + q[3]);

    if (a > b) {
        q[0] = -q[0];
        q[1] = -q[1];
        q[2] = -q[2];
        q[3] = -q[3];
    }

    cosom = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

    if ((1.0f + cosom) > 0.0f) {
        if ((1.0f - cosom) > 0.0f) {
            omega = acosf (cosom);
            sinom = sinf (omega);
            sclp = sinf ((1.0f - t) * omega) / sinom;
            sclq = sinf (t * omega) / sinom;
        }
        else {
            sclp = 1.0f - t;
            sclq = t;
        }
        
        res[0] = sclp * p[0] + sclq * q[0];
        res[1] = sclp * p[1] + sclq * q[1];
        res[2] = sclp * p[2] + sclq * q[2];
        res[3] = sclp * p[3] + sclq * q[3];
    }
    else {
        res[0] = -p[1];
        res[1] = p[0];
        res[2] = -p[3];
        res[3] = p[2];
        sclp = sinf ((1.0f - t) * PI12);
        sclq = sinf (t * PI12);

        res[0] = sclp * p[0] + sclq * res[0];
        res[1] = sclp * p[1] + sclq * res[1];
        res[2] = sclp * p[2] + sclq * res[2];
    }
}

float randf (float min, float max) {
    return min + ((float) rand() / (float)RAND_MAX * (max - min));
}

float a_shift (float begin, float end, float rate) {
    //n1_misc::debug (":", begin);
    //n1_misc::debug (";", end);
    return end;
}

bool pt_in_box (const float *p, const float *hb2) {
    return ((p[0] <= hb2[0]) && (p[0] >= -hb2[0]) &&
            (p[1] <= hb2[1]) && (p[1] >= -hb2[1]) &&
            (p[2] <= hb2[2]) && (p[2] >= -hb2[2])) ? true : false;
}

bool pt_in_sph (const float *p, float sr2) {
    return (sr2 >= v_dot (p, p)) ? true : false;
}

// p is relative to sphere (sphere_pos - ray_origin)
// ray must be normalized
// isec_n = p + tn * ray
bool ray_to_sph (const float *p, const float *ray, float sr2,
        float *t1, float *t2) {
    float t_ca, t_hc, d_sqr;

    // algorithm care of cs.princeton.edu (img013.gif)
    // ray: isec = ray_origin + t(ray)
    // sphere: |isec - sphere_origin|^2 - sphere_rad^2 = 0
    // p is line from sphere_origin to ray_origin
    // t_ca is line from ray_origin to midway between intersection points
    // t_ca = p . ray
    // d is line from sphere_origin to t_ca
    // pythagorean wizardry
    // p . p = d^2 + t_ca^2
    // d^2 = p . p - t_ca^2
    // t_hc is the distance from t_ca to the intersection points
    // more pythagorean wizardry
    // t_hc = sqrt (sphere_rad^2 - d^2)
    // t = t_ca - t_hc and t_ca + t_hc

    t_ca = v_dot (p, ray);
    if (t_ca <= 0.0f)
        return false;

    d_sqr = v_dot (p, p) - (t_ca * t_ca);
    if (d_sqr > sr2)
        return false;

    if (t1 || t2) {
        t_hc = sqrtf (sr2 - d_sqr);
        if (t1)
            *t1 = t_ca - t_hc;
        if (t2)
            *t2 = t_ca + t_hc;
    }
    return true;
}

bool ray_to_poly (const float *p, const float *ray,
        const float *_v1, const float *_v2, const float *_v3, const float *pn, 
        float *t_out, float *isec_out) {
    float ev0 [3], ev1 [3], ev2 [3], isec [3];
    float dot00, dot01, dot02, dot11, dot12, u, v;
    float t, d;

    // find ray intersection point isec
    // algorithm mostly care of cs.princeton.edu (img017.gif)
    // ray: isec = p + t(ray)
    // plane: isec . pn + pn[3] = 0
    // substituting for isec
    // (p + t(ray)) . pn + pn[3] = 0
    // (p . pn) + (t(ray) . pn) = -pn[3]
    // t(ray) . pn = -(p . pn + pn[3])
    // t = -(p . pn + pn[3]) / (ray . pn)
    // t = plane_dit (p, pn) / -(ray . pn)
    d = v_dot (ray, pn);
    if (d > 0.0f)
        return false;

    t = plane_dist (p, pn) / -d;

    if (t <= 0.0f)
        return false;

    v_scalef2 (isec, ray, t);
    v_add (isec, p);

    // algorithm care of http://www.blackpawn.com/texts/pointinpoly/
    // test whether isec is within polygon (using barycentric coordinates)
    // P   = A   + u (C - A) + p (B - A)
    // P - A     = u (C - A) + p (B - A)
    // ev2       = u (ev0) + p (ev1)
    // ev2 . ev0 = (u (ev0) + p (ev1)) . ev0
    // ev2 . ev0 = u (ev0 . ev0) + p (ev1 . ev0)
    // ev2 . ev1 = (u (ev0) + p (ev1)) . ev1
    // ev2 . ev1 = u (ev0 . ev1) + p (ev1 . ev1)
    // solve for u and v
    // u = ((ev1 . ev1)(ev2 . ev0) - (ev1 . ev0)(ev2 . ev1)) /
    //    ((ev0 . ev0)(ev1 . ev1) - (ev0 . ev1)(ev1 . ev0))
    // v = ((ev0 . ev0)(ev2 . ev1) - (ev0 . ev1)(ev2 . ev0)) /
    //    ((ev0 . ev0)(ev1 . ev1) - (ev0 . ev1)(ev1 . ev0))
    v_sub2 (ev0, _v3, _v1);
    v_sub2 (ev1, _v2, _v1);
    v_sub2 (ev2, isec, _v1);
    dot00 = v_dot (ev0, ev0);
    dot01 = v_dot (ev0, ev1);
    dot02 = v_dot (ev0, ev2);
    dot11 = v_dot (ev1, ev1);
    dot12 = v_dot (ev1, ev2);
    d = dot00 * dot11 - dot01 * dot01;
    if (d == 0.0f)
        return true;
    u = (dot11 * dot02 - dot01 * dot12) / d;
    v = (dot00 * dot12 - dot01 * dot02) / d;
    if ((u < 0.0f) || (v < 0.0f) || (u + v > 1.0f))
        return false;

    if (t_out)
        *t_out = t;
    if (isec_out)
        v_copy (isec_out, isec);
    return true;
}

bool sph_in_sph (const float *p, float r1, float r2) {
    float t = r1 + r2;
    return ((t * t) >= v_dot (p, p)) ? true : false;
}

bool sph_in_plane (const float *p, float r, const float *pl) {
    float t = plane_dist (p, pl);
    return ((t < -1.0f) || (t > r)) ? false : true;
}

float pt_on_line_closest_to_pt (const float *p,
        const float *v1, const float *v2, float *p_out) {
    float v [3], ray [3];
    float d, t;

    // algorithm care of DigiBen@GameTutorials.com

    v_sub2 (ray, v2, v1);
    d = v_len (ray);
    v_divf (ray, d);

    v_sub2 (v, p, v1);
    t = v_dot (v, ray);
    if (t <= 0.0f)
        v_copy (v, v1);
    else if (t >= d)
        v_copy (v, v2);
    else {
        v_scalef2 (v, ray, t);
        v_add (v, v1);
    }

    if (p_out)
        v_copy (p_out, v);
    
    v_sub (v, p);
    return v_len (v);
}

float pt_on_line_closest_to_pt (const float *p,
        const float *v1, const float *v2) {
    return pt_on_line_closest_to_pt (p, v1, v2, NULL);
}

bool sph_to_poly (const float *p, float r,
        const float *ray, float move_t,
        const float *v1, const float *v2, const float *v3, const float *pl,
        float *t_out, float *isec_out) {
    float ev0 [3], ev1 [3], ev2 [3];
    float dot00, dot01, dot02, dot11, dot12, u, v;
    float isec [3], d, t;

    // algorithm somewhat care of cs.princeton.edu (img017.gif)

    d = v_dot (ray, pl);
    if (d >= 0.0f)
        return false;

    t = plane_dist (p, pl) / -d;
    if (t < -r)
        return false;
    
    if (t_out)
        *t_out = t;

    if (((t + r * 2.0f) < move_t) || ((t - r) > move_t))
        return false;
    
    v_scalef2 (isec, ray, t);
    v_add (isec, p);
    if (isec_out)
        v_copy (isec_out, isec);

    // algorithm care of http://www.blackpawn.com/texts/pointinpoly/
    v_sub2 (ev0, v3, v1);
    v_sub2 (ev1, v2, v1);
    v_sub2 (ev2, isec, v1);
    dot00 = v_dot (ev0, ev0);
    dot01 = v_dot (ev0, ev1);
    dot02 = v_dot (ev0, ev2);
    dot11 = v_dot (ev1, ev1);
    dot12 = v_dot (ev1, ev2);
    d = dot00 * dot11 - dot01 * dot01;
    if (d == 0.0f)
        return true;
    u = (dot11 * dot02 - dot01 * dot12) / d;
    v = (dot00 * dot12 - dot01 * dot02) / d;
    if ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f))
        return true;

    else if (pt_on_line_closest_to_pt (isec, v1, v2) < r)
        return true;
    else if (pt_on_line_closest_to_pt (isec, v2, v3) < r)
        return true;
    else if (pt_on_line_closest_to_pt (isec, v3, v1) < r)
        return true;
    
    return false;
}

bool sph_to_poly (const float *p, float r,
        const float *ray, float move_t,
        const float *v1, const float *v2, const float *v3, const float *pl) {
    return sph_to_poly (p, r, ray, move_t, v1, v2, v3, pl, NULL, NULL);
}

} // namespace math

