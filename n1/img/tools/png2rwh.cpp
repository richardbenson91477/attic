
#include "../img.hpp"

int main (int argc, char **argv) {
    uint8_t *p;
    FILE *f_out;
    img img1;

    if (argc != 3) {
        fprintf (stderr, "%s", "usage: png2rwh input output\n");
        return 1;
    }

    if (! img1.load_png (argv[1]))
        return 2;

    f_out = fopen (argv[2], "wb");
    if (! f_out) {
        dlog::error ("write", argv[2]);
        return 3;
    }

    fwrite (&img1.w, 1, 4, f_out);
    fwrite (&img1.h, 1, 4, f_out);
    fwrite (img1._b, 1, img1.w * img1.h * 4, f_out);

    fclose (f_out);
    exit (0);
}

