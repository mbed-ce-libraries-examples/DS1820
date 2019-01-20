#ifndef DS1820_H_
    #define DS1820_H_

    #include <OneWire.h>

/**
 * Dallas' DS1820 family temperature sensor.
 * This library depends on the OneWire library (Dallas' 1-Wire bus protocol implementation)
 * available at <http://developer.mbed.org/users/hudakz/code/OneWire/>
 *
 * Example of use:
 * 
 * @code
 * #include "DS1820.h"
 *
 * Serial serial(USBTX, USBRX);
 * 
 * int main() {
 *     DS1820  ds1820(PA_9);    // substitute PA_9 with actual mbed pin name connected to the DS1820 data pin
 *
 *     if(ds1820.begin()) {
 *        ds1820.startConversion();
 *        wait(1.0);
 *        while(1) {
 *            serial.printf("temp = %3.1f\r\n", ds1820.read());     // read temperature
 *            ds1820.startConversion();     // start temperature conversion
 *            wait(1.0);                    // let DS1820 complete the temperature conversion
 *        }
 *    } else
 *        serial.printf("No DS1820 sensor found!\r\n");
 * }
 *
 * @endcode
 * 
 * Note: Don't forget to connect a 4.7k Ohm resistor 
 *       between the DS1820's data pin and the +3.3V pin
 *
 */
class   DS1820
{
    OneWire oneWire;
    bool    present;    
    bool    model_s;
    uint8_t data[12];
    uint8_t addr[8];
    float   toFloat(uint16_t word);
public:

    DS1820(PinName pin);
    DS1820(char model, PinName pin);
    bool   begin(void);
    bool   isPresent();
    void   setResolution(uint8_t res);
    void   startConversion(void);
    float  read(void);
    uint8_t read(float& temp);
};
#endif /* DS1820_H_ */
