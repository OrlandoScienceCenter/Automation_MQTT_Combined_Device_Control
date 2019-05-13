void powerOnComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    client.publish(TOPIC_T, "Warning: Device already on");
    startupFlag = 1;
  } else {
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);

    if (computerButtonStateReversed){digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);} else {digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);}

    delay(200);
    
    if (computerButtonStateReversed){digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);} else {digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);}
    
    if (!computerButtonStateReversed){pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);} // For KZ Sims keeps driving relay low
    
    client.publish(TOPIC_T, "Starting");
    startupFlag = 1;
  }
}

void powerOffComputer() {
  int curState = analogRead(A0);
  if (curState > 900) {
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);
    
    if (computerButtonStateReversed){digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);} else {digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);}
    
    delay(200);
    
    if (computerButtonStateReversed){digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);} else {digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);}
    
    if (!computerButtonStateReversed){pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);} // For KZ Sims keeps driving relay low
    
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
    if (computerButtonStateReversed){digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);} else {digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);}
    delay(7000);
    if (computerButtonStateReversed){digitalWrite(COMPUTER_SWITCHSENSEPIN, LOW);} else {digitalWrite(COMPUTER_SWITCHSENSEPIN, HIGH);}
    
    if (!computerButtonStateReversed){pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);} // For KZ Sims keeps driving relay low
    
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
    Serial.println("PowerOnCommandRecieved");
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
    Serial.println("PowerOffCommandRecieved");
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already off");
    startupFlag = 1;
  }
}

void powerOffInfrared(){
  if (curState) {
    client.publish(TOPIC_T, "Turning off");  
   
    if (strcmp(OTA_HOSTNAME, "ARSandboxProjector") == 0){ARSandIROff();}
    if (strcmp(OTA_HOSTNAME, "KTTheaterProjector") == 0){KTTIROff();}
    if (strcmp(OTA_HOSTNAME, "WFTVMitsubishiTV") == 0){WFTVMitsubishiTVOff();}
    if (strcmp(OTA_HOSTNAME, "ScienceLiveProjector") == 0){SLStage2ndFloorViewsonicOff();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector1") == 0){SOSIROff();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector2") == 0){SOSIROff();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector3") == 0){SOSIROff();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector4") == 0){SOSIROff();}
    if (strcmp(OTA_HOSTNAME, "KZSimOneProjector") == 0){KZSimsProjectorsOff();}
    if (strcmp(OTA_HOSTNAME, "KZSimTwoProjector") == 0){KZSimsProjectorsOff();}
    if (strcmp(OTA_HOSTNAME, "KZSimThreeProjector") == 0){KZSimsProjectorsOff();}
    if (strcmp(OTA_HOSTNAME, "TheraminProjector") == 0){TheraminProjectorOff();}
    
    startupFlag = 1;
  } else {
    client.publish(TOPIC_T, "Warning: Device already off");
    startupFlag = 1;
  }
}

void powerOnInfrared(){
  if (!curState) {
    client.publish(TOPIC_T, "Starting");  
    
    if (strcmp(OTA_HOSTNAME, "ARSandboxProjector") == 0){ARSandIROn();}
    if (strcmp(OTA_HOSTNAME, "KTTheaterProjector") == 0){KTTIROn();}
    if (strcmp(OTA_HOSTNAME, "WFTVMitsubishiTV") == 0){WFTVMitsubishiTVOn();}
    if (strcmp(OTA_HOSTNAME, "ScienceLiveProjector") == 0){SLStage2ndFloorViewsonicOn();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector1") == 0){SOSIROn();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector2") == 0){SOSIROn();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector3") == 0){SOSIROn();}
    if (strcmp(OTA_HOSTNAME, "SOSProjector4") == 0){SOSIROn();}
    if (strcmp(OTA_HOSTNAME, "TheraminProjector") == 0){TheraminProjectorOn();}
    if (strcmp(OTA_HOSTNAME, "KZSimOneProjector") == 0){KZSimsProjectorsOn();}
    if (strcmp(OTA_HOSTNAME, "KZSimTwoProjector") == 0){KZSimsProjectorsOn();}
    if (strcmp(OTA_HOSTNAME, "KZSimThreeProjector") == 0){KZSimsProjectorsOn();}
    
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

// -----===== Begin Science Live Stage Viewsonic Projector =====-----
void SLStage2ndFloorViewsonicOff(){
  irsend.sendNEC(0xC12FE817, 32);
  delay(1000);
  irsend.sendNEC(0xC12FE817, 32); 
}

void SLStage2ndFloorViewsonicOn(){
   irsend.sendNEC(0xC12FE817, 32);
}
// -----===== End Science Live Stage Viewsonic Projector =====-----

// -----===== Begin KZ Sims Projectors =====-----
void KZSimsProjectorsOn(){
  irsend.sendNEC(0x4CB340BF, 32);    
}

void KZSimsProjectorsOff(){
  irsend.sendNEC(0x4CB3748B, 32);
  delay(1000);
  yield();
  irsend.sendNEC(0x4CB3748B, 32);
  yield();
}
// -----===== End KZ Sims Projectors =====-----

// -----===== Begin STEAM Theramin Projector =====-----
uint16_t theraminRawPower[239] = {472, 230,  334, 806,  3486, 1720,  446, 446,  412, 1292,  446, 448,  420, 444,  414, 452,  416, 422,  446, 420,  438, 454,  414, 426,  442, 422,  448, 446,  412, 452,  416, 422,  444, 1286,  502, 364,  442, 450,  418, 420,  438, 428,  440, 452,  416, 450,  418, 446,  412, 454,  414, 452,  416, 1288,  440, 452,  416, 450,  418, 448,  410, 1294,  444, 448,  420, 446,  412, 1294,  444, 420,  448, 446,  412, 454,  414, 450,  418, 420,  448, 444,  414, 452,  416, 450,  418, 446,  410, 1294,  444, 448,  420, 1286,  442, 1290,  448, 1284,  442, 1290,  438, 454,  414, 452,  418, 1288,  438, 454,  414, 1290,  448, 446,  412, 1292,  446, 1286,  440, 1292,  498, 1234,  442, 32166,  1066, 40676,  3490, 1716,  440, 452,  418, 1288,  438, 454,  416, 450,  418, 448,  420, 446,  412, 452,  416, 450,  418, 448,  410, 454,  414, 452,  416, 448,  420, 448,  412, 1292,  444, 448,  420, 446,  412, 452,  416, 450,  418, 448,  410, 456,  412, 452,  418, 448,  420, 446,  412, 1292,  444, 422,  448, 444,  414, 452,  416, 1288,  448, 416,  440, 426,  444, 1288,  438, 426,  442, 424,  444, 420,  448, 418,  440, 426,  442, 424,  446, 420,  448, 418,  440, 426,  442, 1288,  438, 428,  440, 1292,  446, 1286,  442, 1290,  448, 1284,  442, 424,  444, 420,  448, 1286,  442, 422,  446, 1286,  442, 426,  444, 1288,  440, 376,  492, 424,  444, 1288,  440, 1292,  446};  // UNKNOWN FD722909

void TheraminProjectorOn(){
  irsend.sendRaw(theraminRawPower, 239, 38);
  yield();
}

void TheraminProjectorOff(){
  irsend.sendRaw(theraminRawPower, 239, 38);
  delay(500);
  yield();
  irsend.sendRaw(theraminRawPower, 239, 38);
  delay(500);
  yield();
  irsend.sendRaw(theraminRawPower, 239, 38);
  delay(500);
  yield();
}

// -----===== End STEAM Theramin Projector =====-----
