/*!
 * @file
 * @brief
 */

#include <stdbool.h>
#include <stddef.h>
#include "stm8s.h"
#include "stm8s_tim1.h"
#include "stm8s_gpio.h"
#include "stm8s_clk.h"
#include "cap_touch.h"
#include "tiny_utils.h"

enum {
  setup_delay = 2,
  sample_period = 23,
  input_filter_8 = (8 << 4),
  pin_3 = (1 << 3),
  pin_5 = (1 << 5),
  threshold = 150
};

static struct {
  tiny_timer_t timer;
} self;

static void start_charge(void) {
  // Configure pin as push pull output, output high
  GPIOC->CR1 |= pin_3;
  GPIOC->ODR |= pin_3;
  GPIOC->DDR |= pin_3;
}

static void stop_charge(void) {
  // Configure pin as floating input
  GPIOC->CR1 &= ~(pin_3);
  GPIOC->DDR &= ~(pin_3);
}

static void start_capture(void) {
  // Enable capture on channel 3
  TIM1->CCER2 |= TIM1_CCER2_CC3E;

  // Reset counter
  TIM1->CNTRH = 0;
  TIM1->CNTRL = 0;

  // Start counting
  TIM1->CR1 |= TIM1_CR1_CEN;
}

static void end_capture(void) {
  // Disable capture on channel 3
  TIM1->CCER2 &= ~(TIM1_CCER2_CC3E);

  // Stop counting
  TIM1->CR1 &= ~TIM1_CR1_CEN;
}

static void measure(void) {
  bool pressed;

  uint16_t count = (TIM1->CCR3H << 8) + TIM1->CCR3L;
  pressed = count < threshold;

  if(pressed) {
    GPIOB->ODR |= pin_5;
  }
  else {
    GPIOB->ODR &= ~pin_5;
  }
}

static void sample(tiny_timer_group_t* timer_group, void* context);

static void setup(tiny_timer_group_t* timer_group, void* context) {
  (void)context;

  end_capture();
  measure();
  start_charge();

  tiny_timer_start(timer_group, &self.timer, setup_delay, sample, NULL);
}

static void sample(tiny_timer_group_t* timer_group, void* context) {
  (void)context;

  stop_charge();
  start_capture();

  tiny_timer_start(timer_group, &self.timer, sample_period, setup, NULL);
}

static void initialize_led(void) {
  GPIOB->CR1 |= pin_5;
  GPIOB->DDR |= pin_5;
}

static void initialize_tim1(void) {
  // Un-gate clock for TIM1
  CLK->PCKENR1 |= (1 << CLK_PERIPHERAL_TIMER1);

  // Prescalar 1, 16 MHz
  TIM1->PSCRH = 0;
  TIM1->PSCRL = 0;

  // Count up, no auto-reload, update event (UEV) disabled
  TIM1->CR1 = TIM1_CR1_UDIS;
}

static void initialize_tim1_channel3(void) {
  // Direct input capture, filter of 8, no prescalar
  TIM1->CCMR3 = TIM1_ICSELECTION_DIRECTTI | input_filter_8;

  // Trigger on falling edge
  TIM1->CCER2 |= TIM1_CCER2_CC3P;
}

void cap_touch_init(tiny_timer_group_t* timer_group) {
  initialize_tim1();
  initialize_tim1_channel3();
  initialize_led();

  setup(timer_group, NULL);
}
