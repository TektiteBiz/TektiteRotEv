#include <TektiteRotEv.h>

RotEv rotev;
bool going = false;

void setup() {
  rotev.begin();
  rotev.ledWrite(0.0, 0.0, 0.0);
  rotev.motorEnable(false);
  delay(10);
}

// Apply gamma correction (default gamma = 2.2)
float gammaCorrect(float value, float gamma = 2.2) { return pow(value, gamma); }

// HSV -> RGB conversion
void hsv2rgb(float h, float s, float v, float& r, float& g, float& b) {
  float i = floor(h * 6.0);
  float f = h * 6.0 - i;
  float p = v * (1.0 - s);
  float q = v * (1.0 - f * s);
  float t = v * (1.0 - (1.0 - f) * s);

  switch ((int)i % 6) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }
}

bool goPressed = false;
uint32_t lastPrint = 0;
void loop() {
  static float hue = 0.0;
  float r, g, b;

  // Convert HSV -> RGB
  hsv2rgb(hue, 1.0, 0.5, r, g, b);

  // Apply gamma correction
  r = gammaCorrect(r);
  g = gammaCorrect(g);
  b = gammaCorrect(b);

  // Check for go and stop
  if (rotev.goButtonPressed()) {
    rotev.ledWrite(0.0, 0.5, 0.0);
    goPressed = true;
  } else if (rotev.stopButtonPressed()) {
    rotev.ledWrite(0.5, 0.0, 0.0);
    going = false;
  } else if (!rotev.goButtonPressed() && goPressed) {
    rotev.ledWrite(0.0, 0.5, 0.0);
    going = true;
    goPressed = false;
  } else if (going) {
    rotev.ledWrite(0.0, 0.0, 0.3);
  } else {
    // Send to LED
    rotev.ledWrite(r, g, b);
  }

  // Animate
  hue += 0.0005;
  if (hue > 1.0) hue -= 1.0;
  delay(1);

  // Motor stuff
  if (going) {
    rotev.motorEnable(true);
    rotev.motorWrite1(0.2);
    rotev.motorWrite2(-0.2);
  } else {
    rotev.motorEnable(false);
  }

  // Printing
  if (millis() - lastPrint > 500) {
    float yawRate = rotev.readYawRateDegrees();
    Serial.print("Hello, RotEv! Yaw Rate: ");
    Serial.print(yawRate);
    Serial.print(" deg/s, Encoder 1: ");
    Serial.print(rotev.enc1AngleDegrees());
    Serial.print(" deg, Encoder 2: ");
    Serial.print(rotev.enc2AngleDegrees());
    Serial.println(" deg");
    lastPrint = millis();
  }
}