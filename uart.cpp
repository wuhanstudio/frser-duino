#include <Arduino.h>

#include "uart.h"

uint8_t uart_recv(void) {
    while (Console.available() == 0) {
        // optional: low power wait could go here, but Arduino doesn't expose sleep easily
        // yield(); // let background tasks run
    }
    return (uint8_t)Console.read();
}

void uart_send(uint8_t val) {
    Console.write(val);
}
