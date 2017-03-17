void powerOnComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    client.publish(TOPIC_T, "Error: Device already on");
    startupFlag = 1;
  } else {
    pinMode (D5, OUTPUT);
    digitalWrite(D5, LOW);
    delay(200);
    digitalWrite(D5, HIGH);
    pinMode (D5, INPUT);
    client.publish(TOPIC_T, "Starting");
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
    client.publish(TOPIC_T, "Turning off");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Error: Device already off");
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
    client.publish(TOPIC_T, "CritErr: Device forced off");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Error: Device already off");
    startupFlag = 1;
  }
}

void powerOnRelay() {
  if (curState == 0) {
    digitalWrite(D1, HIGH);
    curState = 1;
    client.publish(TOPIC_T, "Starting");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Error: Device already on");
    startupFlag = 1;
  }
}

void powerOffRelay() {
  if (curState == 1) {
    digitalWrite(D1, LOW);
    curState = 0;
    client.publish(TOPIC_T, "Turning off");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Error: Device already off");
    startupFlag = 1;
  }
}

void powerOffInfrared(){
  if (curState) {
    irsend.send_raw("NEC", 0xCC0000FF, 32);
    delay(1000);
    irsend.send_raw("NEC", 0xCC0000FF, 32);
    delay(1000);
    client.publish(TOPIC_T, "Turning off");  
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Error: Device already off");
    startupFlag = 1;
  }
}

void powerOnInfrared(){
  if (!curState) {
    irsend.send_raw("NEC", 0xCC0000FF, 32);
    delay(1000);
    irsend.send_raw("NEC", 0xCC0000FF, 32);
    delay(1000);
    client.publish(TOPIC_T, "Starting");  
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Error: Device already on");
    startupFlag = 1;
  }
}
