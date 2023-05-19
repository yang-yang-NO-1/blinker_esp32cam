/* *****************************************************************
 *
 * Download latest Blinker library here:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 *
 *
 * Blinker is a cross-hardware, cross-platform solution for the IoT.
 * It provides APP, device and server support,
 * and uses public cloud services for data transmission and storage.
 * It can be used in smart home, data monitoring and other fields
 * to help users build Internet of Things projects better and faster.
 *
 * Make sure installed 2.7.4 or later ESP8266/Arduino package,
 * if use ESP8266 with Blinker.
 * https://github.com/esp8266/Arduino/releases
 *
 * Make sure installed 1.0.5 or later ESP32/Arduino package,
 * if use ESP32 with Blinker.
 * https://github.com/espressif/arduino-esp32/releases
 *
 * Docs: https://diandeng.tech/doc
 *
 *
 * *****************************************************************
 *
 * Blinker 库下载地址:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 *
 * Blinker 是一套跨硬件、跨平台的物联网解决方案，提供APP端、设备端、
 * 服务器端支持，使用公有云服务进行数据传输存储。可用于智能家居、
 * 数据监测等领域，可以帮助用户更好更快地搭建物联网项目。
 *
 * 如果使用 ESP8266 接入 Blinker,
 * 请确保安装了 2.7.4 或更新的 ESP8266/Arduino 支持包。
 * https://github.com/esp8266/Arduino/releases
 *
 * 如果使用 ESP32 接入 Blinker,
 * 请确保安装了 1.0.5 或更新的 ESP32/Arduino 支持包。
 * https://github.com/espressif/arduino-esp32/releases
 *
 * 文档: https://diandeng.tech/doc
 *
 *
 * *****************************************************************/

#define BLINKER_WIFI
// #define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include "ESP32_CAM_SERVER.h"

char auth[] = "e8ea2877eef7";
char ssid[] = "dell";
char pswd[] = "1136759016";
bool setup_camera = false;
int trim = 0;

#define JOY_1 "JOYKey"
// Define Servos
const int ServoPinL = 13; // Left Servo assigned to GPIO 13
const int ServoPinR = 14; // Right Servo assigned to GPIO 14

BlinkerJoystick JOY1("JOY_1");

void initServo() // Here we setup PWM, and attach it to physical pins.
{
  ledcSetup(3, 50, 16);        // 50 hz PWM, 16-bit resolution. Servo uses 1ms - 2ms pulse width as a fraction of 20ms period.
  ledcSetup(4, 50, 16);        // 50 hz PWM, 16-bit resolution. Pulse width is represented as integer fraction of 65536 ie 2ms = 6553.
  ledcAttachPin(ServoPinL, 3); // Attach PWM 3 to GPIO 12
  ledcAttachPin(ServoPinR, 4); // Attach PWM 4 to GPIO 13
}

void joystick1_callback(uint8_t xAxis, uint8_t yAxis)
{
  BLINKER_LOG("Joystick1 X axis: ", xAxis);
  BLINKER_LOG("Joystick1 Y axis: ", yAxis);
  ledcWrite(3, 4915 - ((yAxis - 128) + (128 - xAxis) + trim) * 13);
  ledcWrite(4, 4915 + ((yAxis - 128) - (128 - xAxis) - trim) * 13);
  Serial.printf("%d", 4915 - ((yAxis - 128) + (128 - xAxis) + trim) * 13);
  Serial.println();
  Serial.printf("%d", 4915 + ((yAxis - 128) - (128 - xAxis) - trim) * 13);
  Serial.println();
}

BlinkerSlider Slider1("trim");
BlinkerSlider Slider2("light");
BlinkerSlider Slider3("quality");
BlinkerSlider Slider4("resolution");

void slider1_callback(int32_t value)
{
  BLINKER_LOG("get slider value: ", value);
  trim = value;
}
void slider2_callback(int32_t value)
{
  BLINKER_LOG("get slider value: ", value);
  ledcWrite(7, value);
}
void slider3_callback(int32_t value)
{
  sensor_t *s = esp_camera_sensor_get();
  int res = 0;
  BLINKER_LOG("get slider value: ", value);
  res = s->set_quality(s, value);
}
void slider4_callback(int32_t value)
{
  sensor_t *s = esp_camera_sensor_get();
  int res = 0;
  BLINKER_LOG("get slider value: ", value);
  if (s->pixformat == PIXFORMAT_JPEG)
    res = s->set_framesize(s, (framesize_t)value);
}

BlinkerButton Button1("vflip");
BlinkerButton Button2("hmirror");

void button1_callback(const String &state)
{
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("Toggle on!");
    Button1.color("#0066b4");
    Button1.print("on");
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("Toggle off!");
    Button1.color("#b2b2b2");
    Button1.print("off");
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 0);
  }
  else
  {
    BLINKER_LOG("Get user setting: ", state);
    Button1.color("#e23d30");
    Button1.print("error");
  }
}

void button2_callback(const String &state)
{
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON)
  {
    BLINKER_LOG("Toggle on!");
    Button2.color("#0066b4");
    Button2.print("on");
    sensor_t *s = esp_camera_sensor_get();
    s->set_hmirror(s, 1);
  }
  else if (state == BLINKER_CMD_OFF)
  {
    BLINKER_LOG("Toggle off!");
    Button2.color("#b2b2b2");
    Button2.print("off");
    sensor_t *s = esp_camera_sensor_get();
    s->set_hmirror(s, 0);
  }
  else
  {
    BLINKER_LOG("Get user setting: ", state);
    Button2.color("#e23d30");
    Button2.print("error");
  }
}

void dataRead(const String &data)
{
  BLINKER_LOG("Blinker readString: ", data);
  Blinker.vibrate();
  uint32_t BlinkerTime = millis();
  Blinker.print("millis", BlinkerTime);
}

void setup()
{
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);

  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, LOW);

  // Blinker.begin(auth);
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);
  JOY1.attach(joystick1_callback);
  Slider1.attach(slider1_callback);
  Slider2.attach(slider2_callback);
  Slider3.attach(slider3_callback);
  Slider4.attach(slider4_callback);
  Button1.attach(button1_callback);
  Button2.attach(button2_callback);

  // drop down frame size for higher initial frame rate
  // sensor_t *s = esp_camera_sensor_get();
  // s->set_framesize(s, FRAMESIZE_QVGA);
  // s->set_vflip(s, 1);
  // s->set_hmirror(s, 1);

  // Initialize Servos & LEDs
  initServo();
  ledcSetup(7, 5000, 8);
  // ledcAttachPin(4, 7); // GPIO 4 is LED(板载补光灯)
  pinMode(33, OUTPUT); // GPIO 33 is LED（板子led）
  digitalWrite(33, 0);
  ledcAttachPin(2, 7); // GPIO 2 is LED（小车前照灯）
}

void loop()
{
  Blinker.run();

  if (Blinker.connected() && !setup_camera)
  {
    setupCamera();
    setup_camera = true;

    Blinker.printObject("video", "{\"str\":\"mjpg\",\"url\":\"http://" + WiFi.localIP().toString() + "\"}");
  }
}
