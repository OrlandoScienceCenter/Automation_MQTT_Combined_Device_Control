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
    if (OTA_HOSTNAME == "KTTheaterProjector"){KTTIROff();}
    if (OTA_HOSTNAME == "WFTVMitsubishiTV"){WFTVMitsubishiTVOff();}
    if (OTA_HOSTNAME == "SOSProjector1"){SOSIROff();}
    if (OTA_HOSTNAME == "SOSProjector2"){SOSIROff();}
    if (OTA_HOSTNAME == "SOSProjector3"){SOSIROff();}
    if (OTA_HOSTNAME == "SOSProjector4"){SOSIROff();}
    
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
    if (OTA_HOSTNAME == "KTTheaterProjector"){KTTIROn();}
    if (OTA_HOSTNAME == "WFTVMitsubishiTV"){WFTVMitsubishiTVOn();}
    if (OTA_HOSTNAME == "SOSProjector1"){SOSIROn();}
    if (OTA_HOSTNAME == "SOSProjector2"){SOSIROn();}
    if (OTA_HOSTNAME == "SOSProjector3"){SOSIROn();}
    if (OTA_HOSTNAME == "SOSProjector4"){SOSIROn();}
    
    client.publish(TOPIC_T, "Starting");  
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already on");
    startupFlag = 1;
  }
}

// OSC Specific IR codes:
// -----===== Begin Mitsubishi WFTV TV =====-----
void WFTVMitsubishiTVOn(){
  irsend.sendMitsubishi(0xE240, 16);
  delay(1500);
}

void WFTVMitsubishiTVOff(){
  irsend.sendMitsubishi(0xE240, 16);
  delay(1000);
}
// -----===== End Mitsubishi WFTV TV =====-----
// -----===== Begin AR Sandbox BenQ Projector =====-----
void ARSandIROff(){
  irsend.sendNEC(0xC728D, 32);
  delay(1500);
  irsend.sendNEC(0xC728D, 32);
  delay(1500);
  irsend.sendNEC(0xC728D, 32);
  delay(1500);
}

void ARSandIROn(){
  irsend.sendNEC(0xCF20D, 32);
  delay(1000);
}
// -----===== End AR Sandbox BenQ Projector =====-----

// -----===== Begin KT Theater Projector =====-----
void KTTIROff(){
  uint16_t powerOff[103] = {9090, 4462, 576, 578, 554, 570, 552, 574, 548, 1702, 554, 1696, 550, 576, 556, 570, 552, 574, 548, 1702, 554, 572, 552, 574, 548, 1702, 554, 572, 548, 1700, 556, 1694, 550, 1700, 556, 570, 584, 542, 580, 1670, 576, 550, 582, 1668, 578, 548, 584, 542, 580, 546, 576, 1672, 582, 1668, 578, 548, 572, 1676, 580, 546, 576, 1674, 582, 1668, 576, 1672, 582, 544, 578, 546, 576, 552, 582, 544, 578, 550, 574, 552, 580, 544, 578, 548, 574, 1676, 580, 1670, 576, 1674, 580, 1668, 576, 1672, 582, 1668, 578, 1672, 584, 1668, 578, 14470, 9088, 2208, 604};
  irsend.sendRaw(powerOff, 103, 38);
  delay(1000);
  irsend.sendRaw(powerOff, 103, 38);
  delay(1000);
}

void KTTIROn(){
  uint16_t powerOn[103] = {9054, 4496, 574, 578, 556, 570, 552, 574, 548, 1700, 556, 1694, 552, 574, 550, 578, 556, 570, 552, 1698, 548, 576, 546, 578, 554, 1696, 550, 574, 548, 1702, 554, 1694, 552, 1698, 548, 576, 556, 570, 552, 574, 550, 1700, 556, 570, 554, 574, 548, 576, 546, 578, 554, 1696, 550, 1700, 546, 1704, 552, 572, 550, 1698, 548, 1702, 554, 1696, 550, 1700, 556, 570, 552, 574, 582, 544, 578, 548, 584, 540, 582, 546, 578, 548, 576, 550, 582, 1668, 578, 1672, 574, 1676, 582, 1668, 578, 1670, 574, 1676, 582, 1668, 578, 1672, 584, 14464, 9086, 2210, 604};
  irsend.sendRaw(powerOn, 103, 38);
  delay(1000);
}
// -----===== End  KT Theater Projector =====-----

// -----===== Begin SOS Sanyo Proxtra Projectors =====-----
void SOSIROff(){
  irsend.sendNEC(0xCC0000FF, 32);
  delay(1000);
  irsend.sendNEC(0xCC0000FF, 32);  
  delay(1000);
}

void SOSIROn(){
  irsend.sendNEC(0xCC0000FF, 32);
  delay(1000);
}
// -----===== End SOS Sanyo Proxtra Projectors =====-----
