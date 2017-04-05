/*
  Joakim Wesslen
  2012-06-28
  A library to handle the TX433 Nexa device.
  Version 1.0
  Here is a link to some overview of some different protocols:
  http://tech.jolowe.se/home-automation-rf-protocols/
  The API:s below handles code parameter of the longer format, i.e. with redundancy bits.
*/

#ifndef TX433_NEXA_H
#define TX433_NEXA_H

#define LIB_VERSION 1.0

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif

class Tx433_Nexa
{
  public:
  /*
  @digitalpin - the digitalpin to send data on to transmitter
  @transmittercode - the unique code of the transmitter (26/52 bits)
  @channelcode - the channel code (2/4 bits)
  */
  Tx433_Nexa(int digitalpin, String transmittercode, String channelcode);

  /*
  @unit - the device to turn on. 
    0,1,2 are the three separate devices.
    3 is the complete group.
  */
  void Device_On(int unit);   

  /*
  @unit - the device to turn off. 
    0,1,2 are the three separate devices.
    3 is the complete group.
  */
  void Device_Off(int unit);    

  int Get_txpin(void);
  String Get_TxCode(void);
  String Get_ChCode(void);

  private:
  int txpin;
  
  String TxCode;
  String ChCode;
  static String GrpOn;
  static String GrpOff;
  static String On;
  static String Off;
  static String Unit[];

  const static int pulse_high;
  const static int pulse_one_low;
  const static int pulse_zero_low;
  const static int pulse_sync_low;
  const static int pulse_pause_low;
  
  // Protocol layer
  void sendCode(String str, int len);
  void sendPackets(String grp, String dev, String onoff);

  // Physical layer interfaces
  void sendZero(void);
  void sendOne(void);
  void sendSync(void);
  void sendPause(void);
};
#endif  // TX433_NEXA_H

