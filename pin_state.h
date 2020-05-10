#include <gpiod.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIN_PINS (1)
#define TOTAL_PINS (14)
#define MIN_SLEEP (500000)
#define MAX_SLEEP (2500000)
#define RANDOM_SLEEP ((rand() % MAX_SLEEP) + MIN_SLEEP)
#define VALID_NUM_PINS(n) (n >= MIN_PINS && n <= TOTAL_PINS)
#define VALID_GPIO_PINS                                                        \
  { 4, 5, 6, 12, 13, 16, 17, 18, 22, 23, 24, 25, 26, 27 }
#define IS_VALID_GPIO(p)                                                       \
  ({                                                                           \
    bool result = false;                                                       \
    int valid_pins[TOTAL_PINS] = VALID_GPIO_PINS;                              \
    for (int i = 0; i < TOTAL_PINS; ++i) {                                     \
      if (valid_pins[i] == p) {                                                \
        result = true;                                                         \
        result;                                                                \
      }                                                                        \
    }                                                                          \
    result;                                                                    \
  })

typedef struct pin_state {
  int pin, sleep;
  pthread_t thread;
} pin_state;

void *toggle_pin(void *vargp);

int sleep_for_ms(void *vargp);
