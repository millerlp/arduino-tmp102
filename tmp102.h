
#ifndef __LIB_TMP102_H__
#define __LIB_TMP102_H__

#include "Arduino.h"


//FIXME: I had terrible problems trying to include Wire.h so I had to make up 
//       this awful hack.
#ifdef __LIB_TMP102_TWI_AUX_HACK_
#include "twi_aux.inc"
#endif


//The fields must appear in reverse order. See Table 10 of the TMP102 datasheet
// for the layout of Byte 1 and Byte 2 of the configuration register that
// is getting stored in this struct.
typedef struct {
  //LSB
  byte reserved     : 4; // 4 empty bits
  bool extended13   : 1; // 0 or 1, 1 = extended (13bit) temperature mode
  bool alert        : 1; // read only
  bool conver_rate  : 2; // 2 bits, Conversion rate. 00 = 0.25Hz
                         // 01 = 1Hz, 10 = 4Hz(default), 11 = 8Hz
  
  //MSB
  bool shutdown_m   : 1; // 1 bit, Shutdown mode, enabled when == 1
  bool thermostat   : 1; // 1 bit, thermostat mode: comparator mode = 0, interrupt mode = 1.
  bool al_polarity  : 1; // Alert pin polarity (0 default == active low, or 1 active high)
  byte fault_queue  : 2; // 2 bits, number of times temp exceeds user defined limits, see Table 12 on datasheet.
  byte resolution   : 2; // 2 bits, converter resolution, read only. 11 = default 12 bit mode.
  bool oneShot      : 1; // 1 bit, regular mode == 0, one-shot mode == 1
} Tmp102ConfFormat;


typedef union {Tmp102ConfFormat bf; uint16_t i;} Tmp102ConfFormatConverter;


class tmp102 {
  public:
    tmp102(TwoWire* Wire);
    ~tmp102();
    
    void init(byte address, bool loadconf = false);
    bool readTemp(int &iRead); // The value put into iRead can be multipled by 0.0625 to get temperature in degrees C.
    
    bool readConf();
    bool writeConf(Tmp102ConfFormat conf);
    void getConf(Tmp102ConfFormat &conf);
    
    bool getTLimit(bool T_high, int & data); //The returned value must be divided by 16.
    bool setTLimit(bool T_high, int data);
    
    static int translateFromTmpFormat(int16_t tmp_f, bool ext13);
    //static int16_t translateToTmpFormat(int tmp_in, bool ext13);
    
  private:
    bool rawRead2(byte & data1, byte & data2);
    bool rawRead2(int16_t & data, bool reverse = false);
    bool selectRegister(byte reg, bool force = false);
    
    bool rawWrite2(int16_t data, bool reverse = false);
    
    byte currentRegPointer;
    byte address;
    TwoWire* Wire;
    
    Tmp102ConfFormat currConf;
    
};

const Tmp102ConfFormat TMP102_DEFAULT_CONF = {0,0,1,2,0,0,0,0,3,0};

#define TMP102_REG_TEMP   0
#define TMP102_REG_CONF   1
#define TMP102_REG_T_LOW  2
#define TMP102_REG_T_HIGH 3


#endif

