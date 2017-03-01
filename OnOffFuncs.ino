void powerOnComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    client.publish(DEVICE_TOPIC, "Error: Device already on, cannot turn on.");
    startupFlag = 1;
  } else {
    pinMode (D5, OUTPUT);
    digitalWrite(D5, LOW);
    delay(200);
    digitalWrite(D5, HIGH);
    pinMode (D5, INPUT);
    client.publish(DEVICE_TOPIC, "Success: Device turning on now.");
    startupFlag = 1;
  }
}

void powerOffComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    pinMode (D5, OUTPUT);
    digitalWrite(D5, LOW);
    delay(200);
    digitalWrite(D5, HIGH);
    pinMode (D5, INPUT);
    client.publish(DEVICE_TOPIC, "Success: Device turning off now.");
    startupFlag = 1;
  } else {
    client.publish(DEVICE_TOPIC, "Error: Device already off, cannot turn off.");
    startupFlag = 1;
  }
}

void hardPowerOffComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    pinMode (D5, OUTPUT);
    digitalWrite(D5, LOW);
    delay(7000);
    digitalWrite(D5, HIGH);
    pinMode (D5, INPUT);
    client.publish(DEVICE_TOPIC, "Error: Device had to be hard powered off.");
    startupFlag = 1;
  } else {
    client.publish(DEVICE_TOPIC, "Error: Device already off, cannot turn off.");
    startupFlag = 1;
  }
}

void powerOnRelay() {
  if (curState == 0) {
    digitalWrite(D1, HIGH);
    curState = 1;
    client.publish(DEVICE_TOPIC, "Success: Device turning on now.");
    startupFlag = 1;
  } else {
    client.publish(DEVICE_TOPIC, "Error: Device already on, cannot turn on.");
    startupFlag = 1;
  }
}

void powerOffRelay() {
  if (curState == 1) {
    digitalWrite(D1, LOW);
    curState = 0;
    client.publish(DEVICE_TOPIC, "Success: Device turning off now.");
    startupFlag = 1;
  } else {
    client.publish(DEVICE_TOPIC, "Error: Device already off, cannot turn off.");
    startupFlag = 1;
  }
}
