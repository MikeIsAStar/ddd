#include <string.h>

#include <common/Types.h>

void *memcpy(void *dest, const void *src, size_t n) {
    u8 *d = dest;
    const u8 *s = src;

    while (n-- > 0) {
        *d++ = *s++;
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    u8 *p = s;
    for (size_t i = 0; i < n; i++) {
        p[i] = c;
    }
    return s;
}

size_t strlen(const char *s) {
    const char *f = s;
    while (*s != '\0') {
        s++;
    }
    return s - f;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] < s2[i]) {
            return -1;
        }

        if (s1[i] > s2[i]) {
            return 1;
        }

        if (s1[i] == '\0') {
            return 0;
        }
    }

    return 0;
}
