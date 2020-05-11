#include "pin_state.h"

int num_pins = 0;
int pins[TOTAL_PINS] = {0};

static volatile sig_atomic_t interrupt = 0;

static void signal_received(int signo) { interrupt = 1; }

void *toggle_pin(void *vargp) {
  for (;;) {
    pin_state *ps = (pin_state *)vargp;
    gpiod_ctxless_set_value("gpiochip0", ps->pin, 1, false, "unused",
                            (gpiod_ctxless_set_value_cb)sleep_for_ms,
                            (void *)ps);

    printf("pin %d OFF (%d μs)\n", ps->pin, ps->sleep);
    usleep(ps->sleep);
  }
}

int sleep_for_ms(void *vargp) {
  pin_state *ps = (pin_state *)vargp;
  printf("pin %d ON (%d μs)\n", ps->pin, ps->sleep);

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
          }
        }
      }
      printf("%d is not a valid GPIO pin\n", pin);
    }
  }
}

int main(int argc, char **argv) {
  struct sigaction sa;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = &signal_received;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  srand(time(0));

  get_user_input();

  pin_state ps[num_pins];

  for (int i = 0; i < num_pins; ++i) {
    pthread_t pthread;
    ps[i] =
        (pin_state){.pin = pins[i], .sleep = RANDOM_SLEEP, .thread = pthread};
    pthread_create(&ps[i].thread, NULL, toggle_pin, (void *)&ps[i]);
  }

  for (;;) {
    if (interrupt) {
      interrupt = 0;
      printf("\nReceived an interrupt\n");
      printf("Cancelling %d threads...\n", num_pins);

      for (int i = 0; i < num_pins; ++i) {
        pthread_cancel(ps[i].thread);
      }
      printf("DONE\n");
      exit(0);
    }
    sleep(1);
  }
}
