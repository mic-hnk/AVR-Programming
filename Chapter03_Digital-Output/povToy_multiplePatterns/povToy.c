// POV toy demo framework //

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>

#define BUTTON_PIN PD2

// -------- Functions --------- //
void POVDisplay(uint8_t oneByte) {
  PORTB = oneByte;
  _delay_ms(2);
}

uint8_t checkIsButtonPressed(void)
{
	// pinD reading 0 means button connected to the pin is pressed
	if ((PIND & (1 << BUTTON_PIN)) == 0) {
		return 1;
	} else {
		return 0;
	}
}

void awaitButtonReleased(void) {
	while (checkIsButtonPressed()) {
		_delay_ms(10);
	}
}

uint8_t monitorPatternChange(void)
{
	uint16_t buttonHeldForMs = 0;
	
	while (checkIsButtonPressed()) {
		if (buttonHeldForMs >= 1000) {
			awaitButtonReleased();
			return 1;
		}

		_delay_ms(10);
		buttonHeldForMs += 10;
	}
	
	return 0;
}

static const uint8_t pattern0[] = {
	0b00001110, 0b00011000, 0b10111101, 0b01110110, 0b00111100, 
	0b00111100 ,0b00111100, 0b01110110, 0b10111101, 0b00011000,
	0b00001110
};

static const uint8_t pattern1[] = {
	0b00000000, 0b00011000, 0b00111100, 0b01111110,
    0b11111111, 0b01111110, 0b00111100, 0b00011000,
    0b00000000, 0b00011000, 0b00000000
};

static const uint8_t pattern2[] = {
    0b00000000, 0b00011000, 0b00111100, 0b01111110, 0b11111111,
	0b01111110, 0b00111100, 0b00011000, 0b00000000, 0b00011000,
	0b00111100, 0b00011000, 0b00000000
};

typedef struct {
	const uint8_t *steps;
	size_t stepCount;
} Pattern;

static const Pattern patterns[] = {
	// step count =
	// the pattern array size in bytes /
	// total size of one element 
	{ pattern0, sizeof pattern0 / sizeof pattern0[0] },
	{ pattern1, sizeof pattern1 / sizeof pattern1[0] },
	{ pattern2, sizeof pattern2 / sizeof pattern2[0] }
};

int main(void) {
  // -------- Inits --------- //
  DDRB = 0xff; /* Set up all of LED pins for output */
  DDRD &= 0b11111011;  // Set PD2 as input; leave other DDRD bits unchanged
  PORTD |= 0b00000100;  // Enable PD2's internal pull-up
  
  const size_t patternCount = sizeof patterns / sizeof patterns[0];
  size_t activePattern = 0;

  // ------ Event loop ------ //
  while (1) {                                              /* mainloop */
    
    for (size_t i = 0; i < patterns[activePattern].stepCount; ++i) {
		POVDisplay(patterns[activePattern].steps[i]);
	}
	
	PORTB = 0;
	_delay_ms(10);
	
	if (monitorPatternChange()) {
		activePattern = (activePattern + 1) % patternCount;
	}
  }    
                                                  /* end mainloop */
  return 0;
}
