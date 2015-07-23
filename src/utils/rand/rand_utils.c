#include <stdint.h>
#include <stdlib.h>

inline static int randto(int n) {
  int r;
  int maxrand = (RAND_MAX / n) * n;
  do r = rand(); while (r >= maxrand);
  return r % n;
}

inline static void shuffle(unsigned *x, size_t n) {
  while (--n) {
    size_t j = randto(n + 1);
    unsigned tmp = x[n];
    x[n] = x[j];
    x[j] = tmp;
  }
}

uint32_t nrand32(int n) {
  uint32_t v = 0;
  static unsigned pos[32] = { 0,  1,  2,  3,  4,  5,  6,  7,
                              8,  9, 10, 11, 12, 13, 14, 15,
                             16, 17, 18, 19, 20, 21, 22, 23,
                             24, 25, 26, 27, 28, 29, 30, 31};
  shuffle(pos, 32);
  while (n--) v |= (1U << pos[n]);
  return v;
}

int8_t nrand8_t(int n) {
  int8_t v = 0;
  static signed pos[8] = { 0,  1,  2,  3,  4,  5,  6,  7 };
  shuffle(pos, 8);
  while (n--) v |= (1 << pos[n]);
  return v;
}