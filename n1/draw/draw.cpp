#include "draw.hpp"

uint32_t *draw_p;
int32_t draw_w, draw_h, draw_ps;
int32_t draw_hw, draw_hh;
uint8_t alpha_tbl [256][256];

void draw_init (uint32_t *p, int32_t w, int32_t h) {
    uint32_t a, b;

    draw_p = p;
    draw_w = w;
    draw_h = h;

    draw_hw = draw_w / 2;
    draw_hh = draw_h / 2;
    draw_ps = draw_w * draw_h;

    for (a = 0; a < 256; a ++)
        for (b = 0; b < 256; b ++)
            alpha_tbl[a][b] = ((uint8_t)((a * b) / 256));
            
    srand (time (NULL));
}

void draw_clear (uint32_t c) {
    uint32_t *p = draw_p;
    for (uint32_t pc = 0; pc < draw_ps; pc ++)
        *p++ = c;
}

void draw_point (int32_t x, int32_t y, uint32_t c) {
    if ((x < 0) || (y < 0) || (x >= draw_w) || (y >= draw_h))
        return;

    draw_p[y * draw_w + x] = c;        
}

void draw_effect (uint32_t w, uint32_t d1, uint32_t d2) {
    uint32_t *p, pixel_c, line_c;
    uint32_t tmp_u32, tmp2_u32;
    uint8_t *p_b;

    p_b = (uint8_t *)draw_p;

    // static: d1 = chance, d2 = brigtness range
    if (w == 0) {
        for (pixel_c = 0; pixel_c < draw_ps; pixel_c ++) {
            tmp2_u32 = rand ();
            if (! (tmp2_u32 % d1)) {
                tmp2_u32 = rand ();
                tmp_u32 = p_b[0] + ((tmp2_u32 & 255) % d2);
                if (tmp_u32 < 256)
                    p_b[0] = tmp_u32;
                else
                    p_b[0] = 255;

                tmp_u32 = p_b[1] + (((tmp2_u32 >> 8) & 255) % d2);
                if (tmp_u32 < 256)
                    p_b[1] = tmp_u32;
                else
                    p_b[1] = 255;

                tmp_u32 = p_b[2] + (((tmp2_u32 >> 16) & 255) % d2);
                if (tmp_u32 < 256)
                    p_b[2] = tmp_u32;
                else
                    p_b[2] = 255;
            }
            p_b += 4;
        }
    }
    // ghosting: d1 = offset, d2 = depth
    else if (w == 1) {
        for (line_c = 0; line_c < draw_h; line_c ++) {
            for (pixel_c = 0; pixel_c < draw_w - d1; pixel_c ++) {
                tmp_u32 = p_b[0] + p_b[d1 * 4] / d2;
                if (tmp_u32 < 256)
                    p_b[0] = tmp_u32;
                else
                    p_b[0] = 255;

                tmp_u32 = p_b[1] + p_b[d1 * 4 + 1] / d2;
                if (tmp_u32 < 256)
                    p_b[1] = tmp_u32;
                else
                    p_b[1] = 255;

                tmp_u32 = p_b[2] + p_b[d1 * 4 + 2] / d2;
                if (tmp_u32 < 256)
                    p_b[2] = tmp_u32;
                else
                    p_b[2] = 255;

                p_b += 4;
            }
            p_b += d1 * 4;
        }
    }
    // scanlines: d1 = freq, d2 = pix depth
    else if (w == 2) {
        for (line_c = 0; line_c < draw_h; line_c ++) {
            if (! (line_c % d1)) {
                for (pixel_c = 0; pixel_c < draw_w; pixel_c ++) {
                    if (p_b[0] >= d2)
                        p_b[0] -= d2;
                    else
                        p_b[0] = 0;

                    if (p_b[1] >= d2)
                        p_b[1] -= d2;
                    else
                        p_b[1] = 0;

                    if (p_b[2] >= d2)
                        p_b[2] -= d2;
                    else
                        p_b[2] = 0;
                        
                    p_b += 4;
                }
            }
            else {
                p_b += draw_w * 4;
            }
        }
    }
}

void draw_line (int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t c) {
    int32_t dx, dy, x_c, y_c;
    int32_t last_y, new_y, abs_dy, dst_line_d;
    uint32_t *dst_p;
    float slope;

    if ((x1 < 0) || (y1 < 0) || (x2 < 0) || (y2 < 0) || 
            (x1 >= draw_w) || (y1 >= draw_h) || 
            (x2 >= draw_w) || (y2 >= draw_h))
        return;

    dx = x2 - x1;
    if (dx < 0) {
        x_c = x2; x2 = x1; x1 = x_c;
        y_c = y2; y2 = y1; y1 = y_c;
        dx = -dx;
    }
    dy = y2 - y1;

    dst_p = draw_p + (y1 * draw_w) + x1;

    if (! dy) {
        for (x_c = 0; x_c <= dx; x_c ++)
            *dst_p++ = c;
        return;
    } 
    else if (dy < 0)
        dst_line_d = -draw_w;
    else 
        dst_line_d = draw_w;

    if (! dx) {
        if (dy < 0)
            abs_dy = -dy;
        else
            abs_dy = dy;
        for (y_c = 0; y_c <= abs_dy; y_c ++) {
            *dst_p = c;
            dst_p += dst_line_d;
        }
        return;
    }

    slope = (float)dy / (float)dx;

    new_y = 0;
    for (x_c = 0; x_c <= dx; x_c ++) {
        last_y = new_y;
        new_y = (int32_t)(slope * (float)x_c);
        abs_dy = new_y - last_y;
        if (abs_dy < 0)
            abs_dy = -abs_dy;
        for (y_c = 0; y_c < abs_dy; y_c ++) {
            *dst_p = c;
            dst_p += dst_line_d;
        }
        *dst_p++ = c;
    }
}

void draw_rect (int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c) {
    uint32_t *dst_p;
    int32_t x_c, y_c, dst_line_d;

    if ((x < 0) || (y < 0) || (x >= draw_w) || (y >= draw_h))
        return;

    if (w < 0) {
        x = x + w;
        w = -w;
    }
    if ((x + w) > draw_w)
        w = (x + w) - draw_w;

    if (h < 0) {
        y = y + h;
        h = -h;
    }
    if ((y + h) > draw_h)
        h = (y + h) - draw_h;

    dst_line_d = draw_w - w;
    dst_p = draw_p + (y * draw_w) + x;

    for (y_c = 0; y_c < h; y_c ++) {
        for (x_c = 0; x_c < w; x_c ++)
            *dst_p++ = c;
        dst_p += dst_line_d;
    }
}

void draw_get_rect (uint32_t *dst_p, int32_t x, int32_t y,
        int32_t w, int32_t h) {
    uint32_t *src_p;
    int32_t x_c, y_c, dst_line_d;

    dst_line_d = draw_w - w;
    src_p = draw_p + (y * draw_w) + x;

    for (y_c = 0; y_c < h; y_c ++) {
        for (x_c = 0; x_c < w; x_c ++) {
            *dst_p = *src_p;
            dst_p ++;
            src_p ++;
        }
        src_p += dst_line_d;
    }
}

void draw_put_rect (uint32_t *src_p, uint32_t do_alpha,
        int32_t x, int32_t y, int32_t w, int32_t h) {
    uint32_t *dst_p;
    int32_t x_c, y_c, dst_line_d;
    uint8_t *src_p_b, *dst_p_b, a;

    dst_line_d = draw_w - w;
    dst_p = draw_p + (y * draw_w) + x;

    if (! do_alpha) {
        for (y_c = 0; y_c < h; y_c ++) {
            for (x_c = 0; x_c < w; x_c ++) {
                *dst_p ++ = *src_p;
                src_p ++;
            }
            dst_p += dst_line_d;
        }
    }
    else {
        dst_p_b = (uint8_t *)dst_p;
        dst_line_d *= 4;
        src_p_b = (uint8_t *)src_p;
        for (y_c = 0; y_c < h; y_c ++) {
            for (x_c = 0; x_c < w; x_c ++) {
                a = src_p_b[3];

                *dst_p_b = alpha_tbl[255 - a][*dst_p_b] +
                    alpha_tbl[a][*src_p_b];
                src_p_b ++; dst_p_b ++;

                *dst_p_b = alpha_tbl[255 - a][*dst_p_b] +
                    alpha_tbl[a][*src_p_b];
                src_p_b ++; dst_p_b ++;

                *dst_p_b = alpha_tbl[255 - a][*dst_p_b] +
                    alpha_tbl[a][*src_p_b];
                src_p_b ++; dst_p_b ++;

                src_p_b ++; dst_p_b ++;
            }
            dst_p_b += dst_line_d;
        }
    }
}

void draw_put_src_rect (uint32_t *src_p, uint32_t do_alpha,
        int32_t x, int32_t y, int32_t w, int32_t h,
        int32_t src_x, int32_t src_y, int32_t src_w) {
    uint32_t *dst_p;
    int32_t x_c, y_c, dst_line_d, src_line_d;
    uint8_t *src_p_b, *dst_p_b, a;

    dst_line_d = draw_w - w;
    dst_p = draw_p + (y * draw_w) + x;
    
    src_p += src_y * src_w + src_x;
    src_line_d = src_w - w;

    if (! do_alpha) {
        for (y_c = 0; y_c < h; y_c ++) {
            for (x_c = 0; x_c < w; x_c ++) {
                *dst_p ++ = *src_p;
                src_p ++;
            }
            dst_p += dst_line_d;
            src_p += src_line_d;
        }
    }
    else {
        dst_p_b = (uint8_t *)dst_p;
        dst_line_d *= 4;
        src_p_b = (uint8_t *)src_p;
        src_line_d *= 4;
        for (y_c = 0; y_c < h; y_c ++) {
            for (x_c = 0; x_c < w; x_c ++) {
                a = src_p_b[3];

                *dst_p_b = alpha_tbl[255 - a][*dst_p_b] +
                    alpha_tbl[a][*src_p_b];
                src_p_b ++; dst_p_b ++;

                *dst_p_b = alpha_tbl[255 - a][*dst_p_b] +
                    alpha_tbl[a][*src_p_b];
                src_p_b ++; dst_p_b ++;

                *dst_p_b = alpha_tbl[255 - a][*dst_p_b] +
                    alpha_tbl[a][*src_p_b];
                src_p_b ++; dst_p_b ++;

                src_p_b ++; dst_p_b ++;
            }
            dst_p_b += dst_line_d;
            src_p_b += src_line_d;
        }
    }
}

