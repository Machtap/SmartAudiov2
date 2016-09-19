#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
int first = 0xAA;
int second = 0x55;

void setup() {
  mySerial.begin(4800);
  pinMode(13, OUTPUT);
}

void loop() { 
  //checkStatus();
  //changeChannelAirWave(1);
  //changeChannel(26);
  //exitPitmode();
  //enterPitmode();
  changePower(0);
  //selectFrequency(5800);
}

uint8_t crc8(const uint8_t *data, uint8_t len) {
  #define POLYGEN 0xd5
  uint8_t crc = 0;
  uint8_t currByte;

  for (int i = 0 ; i < len ; i++) {
    currByte = data[i];
    crc ^= currByte;
    for (int i = 0; i < 8; i++) {
      if ((crc & 0x80) != 0) {
        crc = (byte)((crc << 1) ^ POLYGEN);
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

int changeChannel(int channel) { //1-40 by the manual (expects 0-39)
  int command = 0x07;
  int frameLength = 0x01;
  byte setChannel = channel;
  byte crcCheck[5] = {first, second, command, frameLength, setChannel};
  int crcbyte = {crc8(crcCheck, 5)};
  byte changechannelcommand[9] = {0x00, first, second, command, frameLength, setChannel, crcbyte, 0x00};
  mySerial.write(changechannelcommand, 9);
}

int changePower(int level){ // expects 1-4 for 25, 200, 500, 800
  int command = 0x05;
  int frameLength = 0x01;
  byte power = level;
  byte crcCheck[5] = {first, second, command, frameLength, power};
  int crcbyte = {crc8(crcCheck, 5)};
  byte changechannelcommand[8] = {0x00, first, second, command, frameLength, power, crcbyte, 0x00};
  mySerial.write(changechannelcommand, 8);
}

int enterPitmode(){ //sets frequency to 5600 and enters 1 microwatt power mode
  int setFrequency[2] = {0x16, 0x1A};
  selectFrequency(setFrequency);
  int command = 0x0B;
  int frameLength = 0x02;
  int pitmodeID = 0x02;
  byte crcCheck[5] = {first, second, command, frameLength, 0x02};
  int crcbyte = {crc8(crcCheck, 5)};
  byte enterPitmodeCommand [8] = {0x00, first, second, command, frameLength, pitmodeID, crcbyte, 0x00};
  mySerial.write(enterPitmodeCommand, 8);
}

int exitPitmode(){ //exits 1 microwatt power mode, sets power to 25mw
  int command = 0x0B;
  int frameLength = 0x02;
  int pitmodeID = 0x04;
  byte crcCheck[5] = {first, second, command, frameLength, 0x02};
  int crcbyte = {crc8(crcCheck, 5)};
  byte exitPitmodeCommand [8] = {0x00, first, second, command, frameLength, pitmodeID, crcbyte, 0x00};
  mySerial.write(exitPitmodeCommand, 8);
  changePower(0);
}

int selectFrequency(int frequency){ // expects frequency
  int command = 0x09;
  int frameLength = 0x02;
  unsigned char setFrequency[2];
  unsigned long n = frequency;
  setFrequency[0] = (n >> 8);
  setFrequency[1] = n & 0xFF;
  byte crcCheck[6] = {first, second, command, frameLength, setFrequency[0], setFrequency[1]};
  int crcbyte = {crc8(crcCheck, 6)};
  byte changechannelcommand[10] = {0x00, first, second, command, frameLength, setFrequency[0], setFrequency[1], crcbyte, 0x00};
  mySerial.write(changechannelcommand, 9);
}

int checkStatus(){ //fires check status, not observing response... yet.
  byte statusreq[7] = {0x00, first, second, 0x03, 0x00, 0x9F, 0x00}; //manual hex values to request status
  mySerial.write(statusreq, 7);
}
