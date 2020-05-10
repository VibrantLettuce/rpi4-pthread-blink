#include "pin_state.h"

int num_pins = 0;
int pins[TOTAL_PINS] = {0};

void *toggle_pin(void *vargp) {
  for (;;) {
    pin_state *ps = (pin_state *)vargp;
    gpiod_ctxless_set_value("gpiochip0", ps->pin, 1, false, "unused",
                            (gpiod_ctxless_set_value_cb)sleep_for_ms,
                            (void *)ps);

    printf("pin %d OFF\n", ps->pin);
    usleep(ps->sleep);
  }
}

int sleep_for_ms(void *vargp) {
  pin_state *ps = (pin_state *)vargp;
  printf("pin %d ON\n", ps->pin);

  return usleep(ps->sleep);
}

void get_user_input() {
  char line[256];

  while (!VALID_NUM_PINS(num_pins)) {
    printf("Enter the number of pins (%d-%d)\n", MIN_PINS, TOTAL_PINS);

    if (fgets(line, sizeof(line), stdin)) {
      if (1 == sscanf(line, "%d", &num_pins) && VALID_NUM_PINS(num_pins)) {
        break;
      }
    }
  }

  for (int i = 0; i < num_pins; ++i) {
    int pin = 0;

    while (!IS_VALID_GPIO(pin)) {
      printf("Enter a pin #\n");
      if (fgets(line, sizeof(line), stdin)) {
        if (1 == sscanf(line, "%d", &pin)) {
          if (IS_VALID_GPIO(pin)) {
            pins[i] = pin;
            break;
          } else {
            printf("%d is not a valid GPIO pin\n", pin);
          }
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  srand(time(0));

  get_user_input();

  pin_state ps[num_pins];

  for (int i = 0; i < num_pins; ++i) {
    pthread_t pthread;
    ps[i] =
        (pin_state){.pin = pins[i], .sleep = RANDOM_SLEEP, .thread = pthread};
    pthread_create(&ps[i].thread, NULL, toggle_pin, (void *)&ps[i]);
  }

  for (int i = 0; i < num_pins; ++i) {
    pthread_join(ps[i].thread, NULL);
  }

  printf("DONE\n");

  return 0;
}
