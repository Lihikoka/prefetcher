#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include <xmmintrin.h>

#define TEST_W 4096
#define TEST_H 4096

/* provide the implementations of naive_transpose,
 * sse_transpose, sse_prefetch_transpose
 */

#include "impl.c"

/* transpose function pointers */
void (*transpose[3]) (int *src, int *dst, int w, int h) = {
    &naive_transpose,
    &sse_transpose,
    &sse_prefetch_transpose
};
enum {NAIVE, SSE, SSE_PREFETCH, ALL};

static long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

int main()
{
    /* verify the result of 4x4 matrix */
    if (VER == ALL) {
        switch(VER) {
        case NAIVE:
        case ALL:
            printf("Using naive:\n\n");
            break;
        case SSE:
            printf("Using sse:\n\n");
            break;
        case SSE_PREFETCH:
            printf("Using sse_prefetch:\n\n");
            break;
        default:
            fprintf(stderr, "No specified version\n");
            break;
        }
        int testin[16] = { 0, 1,  2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15
                         };
        int testout[16];
        int expected[16] = { 0, 4,  8, 12, 1, 5,  9, 13,
                             2, 6, 10, 14, 3, 7, 11, 15
                           };

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++)
                printf(" %2d", testin[y * 4 + x]);
            printf("\n");
        }
        printf("\n");
        transpose[NAIVE](testin, testout, 4, 4);
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++)
                printf(" %2d", testout[y * 4 + x]);
            printf("\n");
        }
        assert(0 == memcmp(testout, expected, 16 * sizeof(int)) &&
               "Verification fails");
    }

    {
        struct timespec start, end;
        int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out0 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out1 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out2 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);

        srand(time(NULL));
        for (int y = 0; y < TEST_H; y++)
            for (int x = 0; x < TEST_W; x++)
                *(src + y * TEST_W + x) = rand();
        if (VER == SSE_PREFETCH || VER == ALL) {
            clock_gettime(CLOCK_REALTIME, &start);
            transpose[SSE_PREFETCH](src, out0, TEST_W, TEST_H);
            clock_gettime(CLOCK_REALTIME, &end);
            printf("sse prefetch: \t %ld us\n", diff_in_us(start, end));
        }

        if (VER == SSE || VER == ALL) {
            clock_gettime(CLOCK_REALTIME, &start);
            transpose[SSE](src, out1, TEST_W, TEST_H);
            clock_gettime(CLOCK_REALTIME, &end);
            printf("sse: \t\t %ld us\n", diff_in_us(start, end));
        }

        if (VER == NAIVE || VER == ALL) {
            clock_gettime(CLOCK_REALTIME, &start);
            transpose[NAIVE](src, out2, TEST_W, TEST_H);
            clock_gettime(CLOCK_REALTIME, &end);
            printf("naive: \t\t %ld us\n", diff_in_us(start, end));
        }

        free(src);
        free(out0);
        free(out1);
        free(out2);
    }

    return 0;
}
