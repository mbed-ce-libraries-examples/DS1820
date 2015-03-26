#ifndef DS1820_H_
    #define DS1820_H_

    #include <OneWire.h>

class   DS1820
{
    OneWire oneWire;
    uint8_t type_s;
    uint8_t data[12];
    uint8_t addr[8];
    float   toFloat(uint16_t word);
public:
    uint8_t present;

    DS1820(PinName pin);
    DS1820(char model, PinName pin);
    bool   begin(void);
    void   setResolution(uint8_t res);
    void   startConversion(void);
    float  read(void);
};
#endif /* DS1820_H_ */
