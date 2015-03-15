#include "DS1820.h"

#define DEBUG 1

#if DEBUG
extern Serial serial;
#endif

/**
 * @brief   Constructs a generic DS1820 sensor
 * @note    begin() must be called to detect and initialize the actual model
 * @param   Name of data pin
 * @retval
 */
DS1820::DS1820(PinName pin) :
    oneWire(pin) {
    present = 0;
    type_s = 0;
}

/**
 * @brief   Constructs a specific model
 * @note    No need to call begin() to detect and initialize the model
 * @param   One character model name: 'S', 's', 'B' or 'b'
 *          Name of data pin
 * @retval
 */
DS1820::DS1820(char model, PinName pin) :
    oneWire(pin) {
    if((model == 'S') or (model == 's')) {
        present = 1;
        type_s = 1;
    }
    else if((model == 'B') or (model == 'b')) {
        present = 1;
        type_s = 0;
    }
    else
        present = 0;
}

/**
 * @brief   Detects and initializes the actual DS1820 model
 * @note
 * @param
 * @retval  true:   if a DS1820 family sensor was detected and initialized
            false:  otherwise
 */
bool DS1820::begin(void) {
    oneWire.reset_search();
    wait_ms(250);
    if(!oneWire.search(addr)) {
#if DEBUG
        serial.printf("No addresses.\r\n");
#endif
        oneWire.reset_search();
        wait_ms(250);
        return false;
    }

#if DEBUG
    serial.printf("ROM =");
    for(uint8_t i = 0; i < 8; i++) {
        serial.printf(" %x", addr[i]);
    }
    serial.printf("\r\n");
#endif

    if(OneWire::crc8(addr, 7) == addr[7]) {
        present = 1;

        // the first ROM byte indicates which chip
        switch(addr[0]) {
        case 0x10:
            type_s = 1;
#if DEBUG
            serial.printf("DS18S20 or old DS1820\r\n");
#endif            
            break;

        case 0x28:
            type_s = 0;
#if DEBUG
            serial.printf("DS18B20\r\n");
#endif            
            break;

        case 0x22:
            type_s = 0;
#if DEBUG
            serial.printf("DS1822\r\n");
#endif            
            break;

        default:
            present = 0;
#if DEBUG
            serial.printf("Device doesn't belong to the DS1820 family\r\n");
            return false;
#endif            
        }
        return true;
    }
#if DEBUG    
    else {
        serial.printf("Invalid CRC!\r\n");
        return false;
    }
#endif    
}

/**
 * @brief   Starts temperature conversion
 * @note    The time to complete the converion depends on the selected resolusion
 * @param
 * @retval
 */
void DS1820::startConversion(void) {
    if(present) {
        oneWire.reset();

        //oneWire.select(addr);
        oneWire.skip();
        oneWire.write(0x44);    //start conversion
    }
}

/**
 * @brief   Reads temperature from the chip's scratchpad
 * @note
 * @param
 * @retval  Floating point temperature value
 */
float DS1820::read(void) {
    if(present) {
        oneWire.reset();
        oneWire.skip();
        oneWire.write(0xBE);            // to read Scratchpad
        for(uint8_t i = 0; i < 9; i++)
            data[i] = oneWire.read();

        // Convert the raw bytes to a 16bit signed fixed point value :
        // 1 sign bit, 7 integer bits, 8 fractional bits (two’s compliment
        // ie. the LSB of the 16bit binary number represents 1/256th of a unit).

        uint16_t*   p_word = reinterpret_cast < uint16_t * > (&data[0]);

#if DEBUG
        serial.printf("raw = %#x\r\n", *p_word);
#endif            


        if(type_s) {
            *p_word = *p_word << 3;     // default 9 bit resolution
            if(data[7] == 0x10) {

                // "count remain" gives full 12 bit resolution
                *p_word = (*p_word & 0xFFF0) + 12 - data[6];
            }

            *p_word = *p_word << 4;
            return(toFloat(*p_word));
        }
        else {
            uint8_t cfg = (data[4] & 0x60);
            // at lower res, the low bits are undefined, so let's zero them

            if(cfg == 0x00)
                *p_word = *p_word &~7;  // 9 bit resolution, 93.75 ms
            else
            if(cfg == 0x20)
                *p_word = *p_word &~3;  // 10 bit res, 187.5 ms
            else
            if(cfg == 0x40)
                *p_word = *p_word &~1;  // 11 bit res, 375 ms
            *p_word = *p_word << 4;     // default is 12 bit resolution
            return(toFloat(*p_word));
        }
    }
    else
        return 0;
}

/**
 * @brief   Converts a 16bit signed fixed point value to float
 * @note    The 16bit unsigned integer represnts actually
 *          a 16bit signed fixed point value:
 *          1 sign bit, 7 integer bits, 8 fractional bits
 *          (two’s compliment ie. the LSB of the 16bit binary number 
 *           represents 1/256th of a unit).       
 * @param   16bit unsigned integer
 * @retval  Floating point temperature value
 */
float DS1820::toFloat(uint16_t word) {
    //word = word << 4;
    if(word & 0x8000)
        return (-float(uint16_t(~word + 1)) / 256.0f);
    else
        return (float(word) / 256.0f);
}

