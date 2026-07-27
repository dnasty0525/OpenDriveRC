#pragma once

// Call once in setup()
void escInit();

// Call every time a valid packet arrives (drives the motor)
void escUpdate(int throttle);

// Raw output only - writes a neutral pulse, does NOT change arm state.
// Kept for cases where you just want to force the output.
void escNeutral();

// Safety failsafe - writes neutral AND forces the state machine back
// to WAIT_FOR_NEUTRAL, so the pedal must return to center before the
// ESC will respond to throttle again. Call this from your .ino's
// failsafe block, NOT escNeutral().
void escFailsafe();

bool escArmed();
