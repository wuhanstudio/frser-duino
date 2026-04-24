
#include "uart.h"
#include "frser.h"

void setup() {
  // put your setup code here, to run once:
    Console.begin(BAUD);
    while (!Console) {
        ; // wait for serial (needed on some boards like Leonardo)
    }
}

void loop() {
  // put your main code here, to run repeatedly:
	frser_main();
}
