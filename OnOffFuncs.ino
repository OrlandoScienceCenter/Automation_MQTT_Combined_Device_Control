void powerOnComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    client.publish(TOPIC_T, "Warning: Device already on");
    startupFlag = 1;
  } else {
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);
    digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);
    delay(200);
    digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);
    pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);
    client.publish(TOPIC_T, "Starting");
    startupFlag = 1;
  }
}

void powerOffComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);
    digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);
    delay(200);
    digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);
    pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);
    client.publish(TOPIC_T, "Turning off");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already off");
    startupFlag = 1;
  }
}

void hardPowerOffComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);
    digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);
    delay(7000);
    digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);
    pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);
    client.publish(TOPIC_T, "CritErr: Device forced off");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already off");
    startupFlag = 1;
  }
}

void powerOnRelay() {
  if (curState == 0) {
    digitalWrite(RELAY_POSPIN, HIGH);
    curState = 1;
    client.publish(TOPIC_T, "Starting");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already on");
    startupFlag = 1;
  }
}

void powerOffRelay() {
  if (curState == 1) {
    digitalWrite(RELAY_POSPIN, LOW);
    curState = 0;
    client.publish(TOPIC_T, "Turning off");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already off");
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
    client.publish(TOPIC_T, "Warning: Device already off");
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
    client.publish(TOPIC_T, "Warning: Device already on");
    startupFlag = 1;
  }
}
