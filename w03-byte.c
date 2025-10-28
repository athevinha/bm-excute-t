#include "iobench.h"

int main() {
    // open `data` for writing using stdio (buffered, asynchronous)
    FILE* f = fopen("data", "w");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    size_t size = 5120000;
    char ch = '6';
    double start = tstamp();

    size_t n = 0;
    while (n < size) {
        size_t r = fwrite(&ch, 1, 1, f);
        if (r != 1) {
            perror("fwrite");
            fclose(f);
            exit(1);
        }

        n += r;
        if (n % PRINT_FREQUENCY == 0) {
            report(n, tstamp() - start);
        }
    }

    if (fclose(f) != 0) {
        perror("fclose");
        exit(1);
    }

    report(n, tstamp() - start);
    fprintf(stderr, "\n");
    return 0;
}
