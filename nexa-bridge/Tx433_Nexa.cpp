/*
  Joakim Wesslen
  2012-06-28
  A library to handle the TX433 Nexa device.
  Version 1.0
  http://tech.jolowe.se/home-automation-rf-protocols/
*/
#include "Tx433_Nexa.h"

extern "C" {
  // AVR LibC Includes
#include <inttypes.h>
  //  #include <avr/interrupt.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif
}

#define RETRANSMIT  5

const int Tx433_Nexa::pulse_high = 250;
const int Tx433_Nexa::pulse_one_low = 250;
const int Tx433_Nexa::pulse_zero_low = 1250;
const int Tx433_Nexa::pulse_sync_low = 2500;
const int Tx433_Nexa::pulse_pause_low = 10000;

String Tx433_Nexa::GrpOn = "01";
String Tx433_Nexa::GrpOff = "10";
String Tx433_Nexa::On = "01";
String Tx433_Nexa::Off = "10";
String Tx433_Nexa::Unit[] = {
  "1010",
  "1001",
  "0110",
  "1010"
};

/* Public */

Tx433_Nexa::Tx433_Nexa(int digitalpin, String transmittercode, String channelcode)
{
  txpin = digitalpin;
  pinMode(txpin, OUTPUT);

  TxCode = transmittercode;
  ChCode = channelcode;
}

void Tx433_Nexa::Device_On(int dev)
{
  if (dev >= 3) {
    sendPackets(GrpOn, Unit[1], On);
  } else
    sendPackets(GrpOff, Unit[dev], On);
}

void Tx433_Nexa::Device_Off(int dev)
{
  if (dev >= 3) {
    sendPackets(GrpOn, Unit[1], Off);
  } else
    sendPackets(GrpOff, Unit[dev], Off);
}

int Tx433_Nexa::Get_txpin(void)
{
  return txpin;
}

String Tx433_Nexa::Get_TxCode(void)
{
  return TxCode;
}

String Tx433_Nexa::Get_ChCode(void)
{
  return ChCode;
}

/* Private */

// Protocol layer
void Tx433_Nexa::sendCode(String str, int len) {
  int i = 0;
  while (i <= len) {
    if (str.charAt(i) == '0') {
      sendZero();
    }
    if (str.charAt(i) == '1') {
      sendOne();
    }
    i++;
  }
}

void Tx433_Nexa::sendPackets(String grp, String dev, String onoff) {
  for (int i = 0; i < RETRANSMIT; i++) {
    sendSync();
    sendCode(TxCode, TxCode.length());
    sendCode(grp, grp.length());
    sendCode(onoff, onoff.length());
    sendCode(ChCode, ChCode.length());
    sendCode(dev, dev.length());
    sendPause();
  }
}

// Physical layer
void Tx433_Nexa::sendZero() {
  digitalWrite(txpin, HIGH);
  delayMicroseconds(pulse_high);
  digitalWrite(txpin, LOW);
  delayMicroseconds(pulse_zero_low);
}

void Tx433_Nexa::sendOne() {
  digitalWrite(txpin, HIGH);
  delayMicroseconds(pulse_high);
  digitalWrite(txpin, LOW);
  delayMicroseconds(pulse_one_low);
}

void Tx433_Nexa::sendSync() {
  digitalWrite(txpin, HIGH);
  delayMicroseconds(pulse_high);
  digitalWrite(txpin, LOW);
  delayMicroseconds(pulse_sync_low);
}

void Tx433_Nexa::sendPause() {
  digitalWrite(txpin, HIGH);
  delayMicroseconds(pulse_high);
  digitalWrite(txpin, LOW);
  delayMicroseconds(pulse_pause_low);
}

