#include <WiFi.h>
#include <M5Cardputer.h>

int scan_results;
int RSSI_results;
String ssid;

int RSSI_normalized;
int graphHeight;



// M5Cardputer setup
M5Canvas canvas(&M5Cardputer.Display);
String commandBuffer              = "> ";
int cursorY                       = 0;
const int lineHeight              = 32;
unsigned long lastKeyPressMillis  = 0;
const unsigned long debounceDelay = 200;  // Adjust debounce delay as needed

void waitForInput(String& input);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);  // Initialize serial communication for debugging
    Serial.println("Starting Setup");

    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(2);  // Set text size

    M5Cardputer.Display.print("Testing Signal");
}

void loop() {
  // put your main code here, to run repeatedly:
  M5Cardputer.update();
  WiFi.scanNetworks();
  M5Cardputer.Display.clearDisplay();
  M5Cardputer.Display.setCursor(0,0);
  ssid = WiFi.SSID(0);
  RSSI_results = WiFi.RSSI(0);
  RSSI_normalized = (RSSI_results * -2) - 40;
  Serial.println(RSSI_results);
  M5Cardputer.Display.print("SSID: ");
  M5Cardputer.Display.print(ssid);
  M5Cardputer.Display.setCursor(0,20);
  M5Cardputer.Display.print("dB:");
  M5Cardputer.Display.setCursor(0,40);
  M5Cardputer.Display.print(RSSI_results);
  M5Cardputer.Display.drawFastHLine(69, 20, 102, GREEN);
  M5Cardputer.Display.drawFastHLine(69, 240, 102, GREEN);
  M5Cardputer.Display.drawFastHLine(68, 19, 104, GREEN);
  M5Cardputer.Display.drawFastVLine(69, 20, 221, GREEN);
  M5Cardputer.Display.drawFastVLine(171, 20, 221, GREEN);
  M5Cardputer.Display.drawFastVLine(68, 20, 222, GREEN);
  M5Cardputer.Display.drawFastVLine(172, 20, 221, GREEN);
  for (int i=70; i<=170; i++) {
    for (int j=RSSI_normalized; j<=240; j++){
      M5Cardputer.Display.drawPixel(i, j, WHITE);
    }

  }
}

void waitForInput(String& input) {
    unsigned long startTime           = millis();
    unsigned long lastKeyPressMillis  = 0;
    const unsigned long debounceDelay = 200;  // Adjust debounce delay as needed
    String currentInput               = input;

    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            if (status.del && currentInput.length() > 0) {
                // Handle backspace key
                currentInput.remove(currentInput.length() - 1);
                M5Cardputer.Display.setCursor(
                    M5Cardputer.Display.getCursorX() - 6,
                    M5Cardputer.Display.getCursorY());
                M5Cardputer.Display.print(" ");
                M5Cardputer.Display.setCursor(
                    M5Cardputer.Display.getCursorX() - 6,
                    M5Cardputer.Display.getCursorY());
                cursorY            = M5Cardputer.Display.getCursorY();
                lastKeyPressMillis = millis();
            }

            for (auto i : status.word) {
                if (millis() - lastKeyPressMillis >= debounceDelay) {
                    currentInput += i;
                    M5Cardputer.Display.print(i);
                    cursorY            = M5Cardputer.Display.getCursorY();
                    lastKeyPressMillis = millis();
                }
            }

            if (status.enter) {
                M5Cardputer.Display.println();  // Move to the next line
                input = currentInput;
                break;
            }
        }

        if (millis() - startTime > 180000) {  // Timeout after 3 minutes
            M5Cardputer.Display.println("\nInput timeout. Rebooting...");
            delay(1000);    // Delay for 1 second to allow the message to be
                            // displayed
            ESP.restart();  // Reboot the ESP32
        }
    }
}