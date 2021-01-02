
#ifndef RND
    #define RND ((double)rand() / ((double)RAND_MAX + 1.0) )
#endif

#ifndef PI2_78
    #define PI (3.14159265f)
    #define PI14 (0.78539816f) // 1/4 PI
    #define PI12 (1.57079633f) // 1/2 PI
    #define PI34 (2.3561945f) // 3/4 PI
    #define PI2 (6.28318531f) // 2*PI
    #define PI2_34 (4.71238898f) // 3/4 2*PI
    #define PI2_78 (5.49778714f) // 7/8 2*PI
#endif

#ifndef RADIANS
    #define RADIANS(x) ((x) * 0.017453292519943295f)
    #define DEGREES(x) ((x) * 57.29577951308232f)
#endif

#ifndef MIN
    #define MIN(x,y) ((x) < (y) ? (x) : (y))
    #define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ifndef PATH_MAX
    #define PATH_MAX _POSIX_PATH_MAX
#endif

