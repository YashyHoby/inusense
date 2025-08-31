
#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#define NUM_PIXELS 24
#define NUM_FRAMES 48
#define NUM_COLORS 3

extern const uint8_t anime01[NUM_COLORS][NUM_FRAMES]; // Matrix of animation when IR sensor detect (NON-ALERT MODE) (1s, no-loop)
extern const uint8_t anime02[NUM_COLORS][NUM_FRAMES]; // Matrix of animation when IR sensor detect (ALERT MODE) (1s, non-loop)
extern const uint8_t anime03[NUM_COLORS][NUM_FRAMES]; // Matrix of animation while speeching (NON-ALERT MODE) (2s, loop)
extern const uint8_t anime04[NUM_COLORS][NUM_FRAMES]; // Matrix of animation while speeching (ALERT MODE) (2s, loop)
extern const uint8_t anime05[NUM_COLORS][NUM_FRAMES]; // Matrix of animation while listening (NON-ALERT MODE) (2s, loop)
extern const uint8_t anime06[NUM_COLORS][NUM_FRAMES]; // Matrix of animation while listening (ALERT MODE) (2s, loop)
extern const uint8_t anime07[NUM_COLORS][NUM_FRAMES]; // Matrix of animation while thinking (processing on server) (2s, loop)
extern const uint8_t anime08[NUM_COLORS][NUM_FRAMES]; // Matrix of animation when someone brake into (2s, loop)
extern const uint8_t anime09[NUM_COLORS][NUM_FRAMES]; // Matrix of animation while transition from NON-ALERT MODE to ALERT MODE (1s, non-loop)
extern const uint8_t anime10[NUM_COLORS][NUM_FRAMES]; // Matrix of animation while transition from ALERT MODE to NON-ALERT MODE (1s, non-loop)
extern const uint8_t anime11[NUM_COLORS][NUM_FRAMES]; // Matrix of animation turn off neopixel after a certain period of time (1s, non-loop)

#endif`AZ