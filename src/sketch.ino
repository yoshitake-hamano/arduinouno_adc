#include <MsTimer2.h>

class Enc03rDriver
{
private:
    // pin: the number of the analog input pin to read from
    // (0 to 5 on most boards, 0 to 7 on the Mini and Nano, 0 to 15 on the Mega)
    //
    // @see https://www.arduino.cc/en/Reference/AnalogRead
    const static int    ANALOG_PIN0           = 0;
    const static int    ANALOG_PIN1           = 1;
    const static int    READ_INTERVAL_MS      = 10;
    const static int    CALIBRATION_MAX_TIMES = 10;
    const static int    SAMPLING_RATE         = 100;
    const static double DT                    = 1.0 / SAMPLING_RATE;
    enum State {
        UNINITALIZED_STATE,
        CALIBRATING_STATE,
        NORMAL_STATE,
    };
    static volatile State  state;
    static volatile int    calibratingTimes;
    static int             calibratingVoltage0;
    static int             calibratingVoltage1;
    static volatile int    voltageBase0;
    static volatile int    voltageBase1;
    static volatile double velocity0;
    static volatile double velocity1;
    static volatile double angle0;
    static volatile double angle1;

public:
    static void initialize() {
        state = UNINITALIZED_STATE;
        MsTimer2::set(1000 / SAMPLING_RATE,
                      Enc03rDriver::flash);
        MsTimer2::start();
        calibrate();
    }

    static void calibrate() {
        velocity0           = 0;
        velocity1           = 0;
        angle0              = 0;
        angle1              = 0;
        voltageBase0        = 0;
        voltageBase1        = 0;
        calibratingTimes    = 0;
        calibratingVoltage0 = 0;
        calibratingVoltage1 = 0;
        state = CALIBRATING_STATE;
        while (state == CALIBRATING_STATE) {
            delay(1);
        }
    }

    static void flash() {
        if (state == UNINITALIZED_STATE) {
            return;
        }

        // This means that it will map input voltages
        // between 0 and 5 volts into integer values between 0 and 1023.
        //
        // @see https://www.arduino.cc/en/Reference/AnalogRead
        int v0 = analogRead(ANALOG_PIN0);
        int v1 = analogRead(ANALOG_PIN1);
        switch (state) {
        case CALIBRATING_STATE: flashCalibrateState(v0, v1); break;
        case NORMAL_STATE:      flashNormalState(v0, v1);    break;
        }
    }

    static double getVelocity(int index) {
        if (state != NORMAL_STATE) {
            return -1;
        }

        switch(index) {
        case 0:  return velocity0;
        case 1:  return velocity1;
        default: return -1;
        }
    }

    static double getAngle(int index) {
        if (state != NORMAL_STATE) {
            return -1;
        }

        switch(index) {
        case 0:  return angle0;
        case 1:  return angle1;
        default: return -1;
        }
    }

private:
    static double voltage2Velocity(int v) {
        return v / 1023.0 * 5 / 0.00067;
    }

    static void flashCalibrateState(int v0, int v1) {
        if (CALIBRATION_MAX_TIMES <= calibratingTimes) {
            voltageBase0 = calibratingVoltage0 / CALIBRATION_MAX_TIMES;
            voltageBase1 = calibratingVoltage1 / CALIBRATION_MAX_TIMES;
            state = NORMAL_STATE;
            return;
        }

        calibratingVoltage0 += v0;
        calibratingVoltage1 += v1;
        
        calibratingTimes++;
    }

    static void flashNormalState(int v0, int v1) {
        velocity0 =  voltage2Velocity(v0 - voltageBase0);
        angle0    += velocity0 * DT;

        velocity1 =  voltage2Velocity(v1 - voltageBase1);
        angle1    += velocity1 * DT;
    }

};
volatile Enc03rDriver::State Enc03rDriver::state;
volatile int    Enc03rDriver::calibratingTimes;
int             Enc03rDriver::calibratingVoltage0;
int             Enc03rDriver::calibratingVoltage1;
volatile int    Enc03rDriver::voltageBase0;
volatile int    Enc03rDriver::voltageBase1;
volatile double Enc03rDriver::velocity0;
volatile double Enc03rDriver::velocity1;
volatile double Enc03rDriver::angle0;
volatile double Enc03rDriver::angle1;

void setup()
{
    Serial.begin(115200);
    Enc03rDriver::initialize();
    Serial.println("velocity0	angle0	velocity1	angle1");
}

void loop()
{
    int ch = Serial.read();
    if (ch == 'c') {
        Enc03rDriver::calibrate();
        return;
    }
    double v0 = Enc03rDriver::getVelocity(0);
    double v1 = Enc03rDriver::getVelocity(1);
    double a0 = Enc03rDriver::getAngle(0);
    double a1 = Enc03rDriver::getAngle(1);
    Serial.print(v0);
    Serial.print("	");
    Serial.print(a0);
    Serial.print("	");
    Serial.print(v1);
    Serial.print("	");
    Serial.println(a1);
    delay(50);
}
