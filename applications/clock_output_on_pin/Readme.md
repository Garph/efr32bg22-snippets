# Clock Output on Pin

Outputs a clock (selected on line 90 of [app.c](app.c)) to expansion header 16
of the EFR32BG22 Thunderboard using the efr's "digital bus" (see section 25.3.12 Peripheral Resource Routing in the reference manual).

The EFR32BG22 has a much more flexible and straightforward peripheral resource
routing scheme than the EFR32FG12. The individual part's data sheet indicates
which resources can be mapped to a given GPIO port (A, B, C, or D), and all
pins are available.

```cpp
#include "em_device.h"
#include "em_gpio.h"


// The CMU_CLKOUT0 peripheral resource can be routed to an pin on ports C or D 
GPIO->CMUROUTE.CLKOUT0ROUTE |= 
    (gpioPortD << _GPIO_CMU_CLKOUT0ROUTE_PORT_SHIFT) | 
    (2 << _GPIO_CMU_CLKOUT0ROUTE_PIN_SHIFT);
```

In addition, LETIMER0 generates a 1 Hz toggle signal to control the on-board
LED for feedback that app is running. This is routed with an async 
peripheral-to-pin PRS signal on channel 0. If you don't see LETIMER0 blinking, the SiLabs bootloader has likely been erased.

Example with 32.768 kHz low-frequency crystal oscillator (LFXO):

![lfxo](/static/images/lfxo_output.png)