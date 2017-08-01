
// pin: the number of the analog input pin to read from
// (0 to 5 on most boards, 0 to 7 on the Mini and Nano, 0 to 15 on the Mega)
//
// @see https://www.arduino.cc/en/Reference/AnalogRead
#define ANALOG_PIN0 0
#define ANALOG_PIN1 1

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    // This means that it will map input voltages
    // between 0 and 5 volts into integer values between 0 and 1023.
    //
    // @see https://www.arduino.cc/en/Reference/AnalogRead
    int v0 = analogRead(ANALOG_PIN0);
    int v1 = analogRead(ANALOG_PIN1);

    Serial.print(v0);
    Serial.print(",");
    Serial.println(v1);
    delay(100);
}
