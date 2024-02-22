# D1820 library for MbedCE
Library for interfacing DS1820 sensor.

## How to start
1. Create a new project according to [MbedCE instructions](https://github.com/mbed-ce/mbed-os/wiki)
2. Add this as submodule to your project via `git submodule add --depth 1 https://github.com/mbed-ce-libraries-examples/DS1820 DS1820`
3. Update OneWire submodule via `git submodule update --remote OneWire`
3. The top level `CMakeList.txt` (in root of your project) should be modified according to [this wiki page](https://github.com/mbed-ce/mbed-os/wiki/MbedOS-configuration#libraries-in-your-application)
4. Create your main.cpp file and copy & Paste one of example code below.
5. Build the project

## Example code
### Single sensor over onewire
```
#include "mbed.h"
#include "DS1820.h"

DS1820  ds1820(D8);  // fill your pin
 
int main() {
    printf("MbedCE_lib_example_DS1820\n");
    float   temp = 0;
    int     error = 0; 
                             
    if(ds1820.begin()) {
        while(1) {
            ds1820.startConversion();       // start temperature conversion
            thread_sleep_for(1000);         // let DS1820 complete the temperature conversion
            error = ds1820.read(temp);      // read temperature from DS1820 and perform cyclic redundancy check (CRC)
            switch(error) {
            case 0: // no errors -> 'temp' contains the value of measured temperature
                printf("temp = %3.1f\r\n", temp);
                break;
            case 1: // no sensor present -> 'temp' is not updated
                printf("no sensor present\n\r");
                break;
            case 2: // CRC error -> 'temp' is not updated
                printf("CRC error\r\n");
            } 
        }
    } else
        printf("No DS1820 sensor found!\r\n");
}

```
### Multiple sensors over onewire
```
#include "mbed.h"
#include "DS1820.h"

#define     MAX_SENSOSRS   5    // fill max num of sensors (max 256)

DS1820*     ds1820[MAX_SENSOSRS];
DigitalOut  led(LED1);
OneWire     oneWire(D8);        // fill your pin of onewire bus
int         sensorsFound = 0;   // counts the actually found DS1820 sensors

int main()
{
    printf("MbedCE_lib_example_DS1820\n");
    
    //Enumerate (i.e. detect) DS1820 sensors on the 1-wire bus
    for (sensorsFound = 0; sensorsFound < MAX_SENSOSRS; sensorsFound++) {
        ds1820[sensorsFound] = new DS1820(&oneWire);
        if (!ds1820[sensorsFound]->begin()) {
            delete ds1820[sensorsFound];
            break;
        }
    }

    switch (sensorsFound) {
        case 0:
            printf("No DS1820 sensor found!\r\n");
            return -1;

        case 1:
            printf("One DS1820 sensor found.\r\n");
            break;

        default:
            printf("Found %d DS1820 sensors.\r\n", sensorsFound);
    }

    while (1) {
        printf("----------------\r\n");
        for (int i = 0; i < sensorsFound; i++)
            ds1820[i]->startConversion();       // start temperature conversion from analog to digital
        thread_sleep_for(1000);                 // let DS1820 sensors complete the temperature conversion
        for (int i = 0; i < sensorsFound; i++) {
            if (ds1820[i]->isPresent())
                printf("temp[%d] = %3.1f%cC\r\n", i, ds1820[i]->read(), 176); // read temperature
        }
    }
}
```
### Multiple sensors over UART
```
#include "mbed.h"
#include "DS1820.h"

#define MAX_SENSOSRS    5       // fill max num of sensors (max 256)
DS1820*     ds1820[MAX_SENSOSRS];
DigitalOut  led(LED1);
OneWire     oneWire(D0, D1);    // fill your UART pins (UART Tx pin, UART Rx pin)
int         sensorsFound = 0;   // counts the actually found DS1820 sensors

int main()
{
    printf("MbedCE_lib_example_DS1820\n");

    //Enumerate (i.e. detect) DS1820 sensors on the 1-wire bus
    for (sensorsFound = 0; sensorsFound < MAX_SENSOSRS; sensorsFound++) {
        ds1820[sensorsFound] = new DS1820(&oneWire);
        if (!ds1820[sensorsFound]->begin()) {
            delete ds1820[sensorsFound];
            break;
        }
    }

    switch (sensorsFound) {
        case 0:
            printf("No DS1820 sensor found!\r\n");
            return -1;

        case 1:
            printf("One DS1820 sensor found.\r\n");
            break;

        default:
            printf("Found %d DS1820 sensors.\r\n", sensorsFound);
    }

    while (1) {
        led = !led;

        printf("----------------\r\n");
        for (int i = 0; i < sensorsFound; i++)
            ds1820[i]->startConversion();   // start temperature conversion from analog to digital
            thread_sleep_for(1000); 

        for (int i = 0; i < sensorsFound; i++) {
            if (ds1820[i]->isPresent())
                printf("temp[%d] = %3.1f%cC\r\n", i, ds1820[i]->read(), 176);   // read temperature
        }
    }
}
```

## Description
* [Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf)
* [OneWire Readme](https://github.com/mbed-ce-libraries-examples/OneWire/blob/master/README.md)

## Status:
This library was tested (02/2024) with Nucleo-F446RE, DS18B20 (3v3 and pull up 4k7), VS-Code under Win11, GCC 12.3 and MbedCE library

## Special info:
Special thanks to Zoltan Hudak for his contribution to Mbed. Peace friend.
