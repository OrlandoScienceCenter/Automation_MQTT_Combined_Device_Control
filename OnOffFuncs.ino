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
    if (OTA_HOSTNAME == "ARSandboxProjector"){ARSandIROff();}
    client.publish(TOPIC_T, "Turning off");  
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already off");
    startupFlag = 1;
  }
}

void powerOnInfrared(){
  if (!curState) {
    if (OTA_HOSTNAME == "ARSandboxProjector"){ARSandIROn();}
    client.publish(TOPIC_T, "Starting");  
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already on");
    startupFlag = 1;
  }
}

// OSC Specific IR codes:
// -----===== Begin AR Sandbox BenQ Projector =====-----
void ARSandIROff(){
  irsend.sendNEC(0xC728D, 32);
  delay(1000);
  irsend.sendNEC(0xC728D, 32);
  delay(2000);
}

void ARSandIROn(){
  irsend.sendNEC(0xCF20D, 32);
  delay(2000);
}
// -----===== End AR Sandbox BenQ Projector =====-----
