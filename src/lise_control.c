#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <cyaml/cyaml.h>
#include <wiringPi.h>

// Read circuit configuration

#define MIN_CIRCUITS (1)
#define MAX_CIRCUITS (10)

typedef struct {
  int pin_in;
  int pin_out;
  bool is_pull_up;
} circuit_t;

typedef struct {
  circuit_t *circuits;
  unsigned circuits_count;
} wiring_t;

static const cyaml_schema_field_t circuit_mapping[] = {
  CYAML_FIELD_INT("pinIn", CYAML_FLAG_DEFAULT, circuit_t, pin_in),
  CYAML_FIELD_INT("pinOut", CYAML_FLAG_DEFAULT, circuit_t, pin_out),
  CYAML_FIELD_BOOL("isPullUp", CYAML_FLAG_DEFAULT, circuit_t, is_pull_up),
  CYAML_FIELD_END
};

static const cyaml_schema_value_t circuit_schema = {
  CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, circuit_t, circuit_mapping),
};

static const cyaml_schema_field_t wiring_mapping[] = {
  CYAML_FIELD_SEQUENCE(
      "wiring", CYAML_FLAG_POINTER,
      wiring_t, circuits, &circuit_schema,
      MIN_CIRCUITS, MAX_CIRCUITS),
  CYAML_FIELD_END
};

static const cyaml_schema_value_t top_schema = {
  CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, wiring_t, wiring_mapping),
};

static const cyaml_config_t config = {
  .log_level = CYAML_LOG_WARNING,
  .log_fn = cyaml_log,
  .mem_fn = cyaml_mem,
};

// Setup GPIO

typedef void (*interrupt_handler_t)(void);

typedef struct {
  int gpio_pin;
  bool is_high;
} gpio_pin_state_t;

static gpio_pin_state_t board_state[MAX_CIRCUITS];

static void error_exit() {
  fprintf(stderr, "%s", strerror(errno));
  exit(EXIT_FAILURE);
}

static void init_pin_state(unsigned circuit_index, circuit_t *circuit) {
  int pin = circuit->pin_out;
  printf("[ IN  ] Initialize %d to LOW.\n", pin);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  board_state[circuit_index].gpio_pin = pin;
  board_state[circuit_index].is_high = false;
}

static void on_btn_interrupt(unsigned circuit_index) {
  int pin = board_state[circuit_index].gpio_pin;
  bool new_state = !board_state[circuit_index].is_high;
  board_state[circuit_index].is_high = new_state;
  digitalWrite(pin, new_state ? HIGH : LOW);
}

static void on_btn_interrupt_01(void) { on_btn_interrupt(0); }
static void on_btn_interrupt_02(void) { on_btn_interrupt(1); }
static void on_btn_interrupt_03(void) { on_btn_interrupt(2); }
static void on_btn_interrupt_04(void) { on_btn_interrupt(3); }
static void on_btn_interrupt_05(void) { on_btn_interrupt(4); }
static void on_btn_interrupt_06(void) { on_btn_interrupt(5); }
static void on_btn_interrupt_07(void) { on_btn_interrupt(6); }
static void on_btn_interrupt_08(void) { on_btn_interrupt(7); }
static void on_btn_interrupt_09(void) { on_btn_interrupt(8); }
static void on_btn_interrupt_10(void) { on_btn_interrupt(9); }

static interrupt_handler_t handler_for_circuit(unsigned circuit_index) {
  switch (circuit_index) {
    case 0: return &on_btn_interrupt_01;
    case 1: return &on_btn_interrupt_02;
    case 2: return &on_btn_interrupt_03;
    case 3: return &on_btn_interrupt_04;
    case 4: return &on_btn_interrupt_05;
    case 5: return &on_btn_interrupt_06;
    case 6: return &on_btn_interrupt_07;
    case 7: return &on_btn_interrupt_08;
    case 8: return &on_btn_interrupt_09;
    case 9: return &on_btn_interrupt_10;
    default:
      fprintf(stderr, "Too many circuits configured!");
      exit(EXIT_FAILURE);
  }
}

static void register_interrupt(circuit_t *circuit, interrupt_handler_t f) {
  printf(
      "[ OUT ] Listen on %d for %s edge.\n",
      circuit->pin_in,
      circuit->is_pull_up ? "falling" : "rising");

  int err = wiringPiISR(
      circuit->pin_in,
      circuit->is_pull_up ? INT_EDGE_FALLING : INT_EDGE_RISING,
      f);

  if (err < 0) { error_exit(); }
}

int main(int argc, char* argv[]) {
  if (wiringPiSetup() < 0) { error_exit(); }

  cyaml_err_t err;
  wiring_t *wiring;

  char path[256];
  sprintf(path, "%s/.config/lise/wiring.yaml", getenv("HOME"));
  err = cyaml_load_file(
      path, &config, &top_schema, (cyaml_data_t **)&wiring, NULL);

  if (err != CYAML_OK) {
    fprintf(stderr, "ERROR: %s\n", cyaml_strerror(err));
    return EXIT_FAILURE;
  }

  for (unsigned i = 0; i < wiring->circuits_count; ++i) {
    init_pin_state(i, &wiring->circuits[i]);
    register_interrupt(&wiring->circuits[i], handler_for_circuit(i));
  }

  cyaml_free(&config, &top_schema, wiring, 0);

  for (;;) { pause(); }
}
