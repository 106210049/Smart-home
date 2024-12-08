class Emgergency{
  protected:
  int Buzzer_pin;
  public:
  void setupBuzzer();
  void Turn_On_Buzzer();
  void EmergencyMode();
};
unsigned long lastBlinkTime = 0;
bool lcdBlinking = false;

void Emgergency::setupBuzzer(){
  Buzzer_pin=2;
}

void Emgergency::Turn_On_Buzzer(){
  tone(Buzzer_pin, 3000, 300);
}

void Emgergency::EmergencyMode(){
  // Tất cả đèn phải tắt.
  unsigned long currentTime=millis(); 
  lcdController.setCursor(1, 0);
  lcdController.print("Emergency mode");
  if (currentTime - lastBlinkTime >= 500) {
      lcdBlinking = !lcdBlinking;  // Toggle blinking state
      
      if (lcdBlinking) {
        lcdController.backlight();           // Turn on LCD backlight
        Turn_On_Buzzer();     // Buzzer ON
      } else {
        lcdController.noBacklight();         // Turn off LCD backlight
      }
      
      lastBlinkTime = currentTime; // Update last blink time
    }
}
