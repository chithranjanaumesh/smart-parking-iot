#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Parking House"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

#define SENSOR_EXIT 33
#define SENSOR_ENTRY 32

#define SERVO_ENTRY 25
#define SERVO_EXIT 26

#define IR_LED_ENABLE 13

#define I2C_SDA 21
#define I2C_SCL 22

#define GATE_DELAY 1500

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// Parking slot sensors
const int slotSensorPin[5] = {-1, 34, 35, 36, 39};

// Physical LEDs for booking indication
const int bookingLEDPin[5] = {-1, 4, 16, 17, 18};

// Blynk virtual pins for booking switches
const int switchVirtualPin[5] = {0, V1, V2, V3, V4};

// Blynk virtual pins for parking status LEDs
const int ledVirtualPin[5] = {0, V11, V12, V13, V14};

// Parking status variables
bool entryDetected;
bool exitDetected;

bool slot[5]; // Parking slot occupancy status
bool led[5]; // Blynk LED status
bool booked[5]; // Parking slot booking status
bool sw[5]; // Blynk switch visibility status

// Gate timing
unsigned long entryClearTime = 0;
unsigned long exitClearTime = 0;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo motors
Servo entryServo;
Servo exitServo;


// ============================================================
// SETUP
// ============================================================

void setup()
{
    // Configure parking slot sensors and booking LEDs
    for (int i = 1; i <= 4; i++)
    {
        pinMode(slotSensorPin[i], INPUT);
        pinMode(bookingLEDPin[i], OUTPUT);
    }

    // Configure IR illumination
    pinMode(IR_LED_ENABLE, OUTPUT);
    digitalWrite(IR_LED_ENABLE, LOW);

    // Configure entry and exit sensors
    pinMode(SENSOR_ENTRY, INPUT);
    pinMode(SENSOR_EXIT, INPUT);

    // Initialize I2C
    Wire.begin(I2C_SDA, I2C_SCL);

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Connecting Blynk");

    // Read current sensor status
    readSensors();

    // Connect to Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    // Attach servo motors
    entryServo.attach(SERVO_ENTRY);
    exitServo.attach(SERVO_EXIT);

    // Set gates to closed position
    entryServo.write(10);
    exitServo.write(10);

    lcd.setCursor(0, 1);
    lcd.print("Blynk Connected.");

    delay(1000);
    lcd.clear();
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop()
{
    Blynk.run();

    readSensors();
    gateControl();
    updateBlynk();
    updateDisplay();

    delay(200);
}


// ============================================================
// SENSOR READING
// ============================================================

void readSensors()
{
    // Turn ON IR illumination
    digitalWrite(IR_LED_ENABLE, HIGH);

    // Allow sensors to settle
    delay(2);

    // Read parking slot sensors
    for (int i = 1; i <= 4; i++)
    {
        slot[i] = digitalRead(slotSensorPin[i]);
    }

    // Read entry and exit sensors
    entryDetected = digitalRead(SENSOR_ENTRY);
    exitDetected = digitalRead(SENSOR_EXIT);

    // Turn OFF IR illumination
    digitalWrite(IR_LED_ENABLE, LOW);
}


// ============================================================
// GATE CONTROL
// ============================================================

void gateControl()
{
    bool parkingFull =
        slot[1] &&
        slot[2] &&
        slot[3] &&
        slot[4];

    // Entry gate
    if (entryDetected && !parkingFull)
    {
        entryServo.write(110);
        entryClearTime = millis();
    }
    else if (millis() - entryClearTime > GATE_DELAY)
    {
        entryServo.write(10);
    }

    // Exit gate
    if (exitDetected)
    {
        exitServo.write(110);
        exitClearTime = millis();
    }
    else if (millis() - exitClearTime > GATE_DELAY)
    {
        exitServo.write(10);
    }
}


// ============================================================
// LCD DISPLAY AND PHYSICAL LED CONTROL
// ============================================================

void updateDisplay()
{
    for (int i = 1; i <= 4; i++)
    {
        // Set LCD position for each parking slot
        if (i == 1)
        {
            lcd.setCursor(0, 0);
        }
        else if (i == 2)
        {
            lcd.setCursor(10, 0);
        }
        else if (i == 3)
        {
            lcd.setCursor(0, 1);
        }
        else if (i == 4)
        {
            lcd.setCursor(10, 1);
        }

        // Display parking status
        if (slot[i])
        {
            lcd.print("FULL ");
            digitalWrite(bookingLEDPin[i], LOW);
        }
        else if (booked[i])
        {
            lcd.print("BOOKED");
            digitalWrite(bookingLEDPin[i], HIGH);
        }
        else
        {
            lcd.print("EMPTY ");
            digitalWrite(bookingLEDPin[i], LOW);
        }
    }
}


// ============================================================
// BLYNK STATUS UPDATE
// ============================================================

void updateBlynk()
{
    for (int i = 1; i <= 4; i++)
    {
        if (slot[i])
        {
            // Cancel booking when slot becomes occupied
            if (booked[i])
            {
                booked[i] = false;
                Blynk.virtualWrite(switchVirtualPin[i], 0);
            }

            // Hide booking switch when slot is occupied
            if (sw[i])
            {
                if (!led[i])
                {
                    Blynk.setProperty(
                        switchVirtualPin[i],
                        "isHidden",
                        true
                    );

                    sw[i] = false;
                }

                Blynk.virtualWrite(ledVirtualPin[i], 1);
                led[i] = true;
            }
        }
        else
        {
            // Show booking switch when slot is available
            if (!sw[i])
            {
                if (led[i])
                {
                    Blynk.setProperty(
                        switchVirtualPin[i],
                        "isHidden",
                        false
                    );

                    sw[i] = true;
                }

                Blynk.virtualWrite(ledVirtualPin[i], 0);
                led[i] = false;
            }
        }
    }
}


// ============================================================
// BLYNK CONNECTION
// ============================================================

BLYNK_CONNECTED()
{
    for (int i = 1; i <= 4; i++)
    {
        Blynk.syncVirtual(switchVirtualPin[i]);

        Blynk.virtualWrite(
            ledVirtualPin[i],
            slot[i]
        );

        Blynk.setProperty(
            switchVirtualPin[i],
            "isHidden",
            slot[i]
        );
    }
}


// ============================================================
// BLYNK BOOKING SWITCHES
// ============================================================

BLYNK_WRITE(V1)
{
    booked[1] = param.asInt();
}

BLYNK_WRITE(V2)
{
    booked[2] = param.asInt();
}

BLYNK_WRITE(V3)
{
    booked[3] = param.asInt();
}

BLYNK_WRITE(V4)
{
    booked[4] = param.asInt();
}
