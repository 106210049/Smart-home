class Motor{
  int motor_left;
  int motor_right;
  int pwm_pin;
  public:
  void setupMotor();
  void MotorOn();
  void MotorOff();
};

void Motor::setupMotor(){
  pwm_pin = 13;
  motor_left = 12;
  motor_right = 14;
  pinMode(motor_left, OUTPUT);
  pinMode(motor_right, OUTPUT);
  pinMode(pwm_pin, OUTPUT);
}
void Motor::MotorOn(){
  digitalWrite(motor_left, HIGH);
  digitalWrite(motor_right, LOW);
  analogWrite(pwm_pin, 125);
}

void Motor::MotorOff(){
  digitalWrite(motor_left, LOW);
  digitalWrite(motor_right, LOW);
}
