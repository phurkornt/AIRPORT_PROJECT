#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial portOne(D5, D6);
SoftwareSerial portTwo(D1, D2);

String CH_BIT[5] = {"1010", "1011", "1100", "1110", "1111"};
int dat[5] = {0b10101010, 0b10111011, 0b11001100, 0b11101110, 0b11111111};
String state = "";


const char* ssid     = "22thMAYSA_2.4G";
const char* password = "RoboAC22";///@S123456

IPAddress local_IP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 254, 0);
IPAddress primaryDNS(8, 8, 8, 8);

WiFiServer server(80);

String header;
String output0State = "DOWN";
String output1State = "DOWN";
String output2State = "DOWN";
String output3State = "DOWN";

int OUT1State = 0;
int OUT2State = 0;
int OUT3State = 0;
int OUT4State = 0;

int output0 = 4;//4
int output1 = 0;//0
int output2 = 2;//2
int output3 = 14;//14

int Floor = 0;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

unsigned long count_time_sender = 0;


bool Emer_ON = false;

void setup()
{
  Serial.begin(115200);

  pinMode(output0, OUTPUT);
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);

  digitalWrite(output0, LOW);
  digitalWrite(output1, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(output3, LOW);

  portOne.begin(9600);
  portTwo.begin(9600);

  WiFi.config(local_IP, primaryDNS, gateway, subnet);
  Serial.print("Connecting");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connect seccess");
  Serial.println("IP = ");
  Serial.println(WiFi.localIP());
  server.begin();
  StateUpdate();

}
void StateUpdate()
{
  if (Floor == 0)
  {
    output0State = "DOWN";
    output1State = "DOWN";
    output2State = "DOWN";
    output3State = "DOWN";
  }
  else if (Floor == 1)
  {
    output0State = "UP";
    output1State = "DOWN";
    output2State = "DOWN";
    output3State = "DOWN";
  }
  else if (Floor == 2)
  {
    output0State = "UP";
    output1State = "UP";
    output2State = "DOWN";
    output3State = "DOWN";
  }
  else if (Floor == 3)
  {
    output0State = "UP";
    output1State = "UP";
    output2State = "UP";
    output3State = "DOWN";
  }
  else if (Floor == 4)
  {
    output0State = "UP";
    output1State = "UP";
    output2State = "UP";
    output3State = "UP";
  }
}
void loop()
{
  WebUpdate();
}


void SendData_B1(int num) {
  portOne.write(dat[num - 1]);
}

bool ch_lastB1 = false;
int counter_GetData_B1 = 0;

bool GetData_B1(int num) {
  if (portOne.available() > 0) {
    char txt = portOne.read();
    Serial.println(txt);
    Serial.println(txt, BIN);
    String x = String(txt, BIN);
    if (x.length() == 8) { // ตรวจสอบขั้น 1
      String head = x.substring(0, 4);
      String tail = x.substring(4, x.length());
      if ( head == tail && head == CH_BIT[num - 1] ) { // ตรวจสอบขั้น 2 และ 3
        Serial.println("Receive B1 at number : " + String(num));
        ch_lastB1 = true;
        Emer_ON = false;
        //        return true;
      } else  if ( head == tail && head == CH_BIT[4] ) {
        // Emer on
        Serial.println("Emer ON");
        ch_lastB1 = true;
        Emer_ON = true;
      }
    }
    counter_GetData_B1 = 0;
  } else {

    if (ch_lastB1 == true) {
      if (counter_GetData_B1 > 15000) {
        Serial.println("OUT GO B1");
        ch_lastB1 = false;
        return true;
      } else {
        counter_GetData_B1++;
      }
    } else {
      counter_GetData_B1 = 0;
    }
  }

  return false;
}


void Control_B1(int num) {
  for (int i = 0; i < 3; i++)SendData_B1(num);
  while (true) {
    if (GetData_B1(num) == true)break;
  }

}

void SendData_B2(int num) {
  portTwo.write(dat[num - 1]);
}


bool ch_lastB2 = false;
int counter_GetData_B2 = 0;
bool GetData_B2(int num) {
  if (portTwo.available() > 0) {
    char txt = portTwo.read();
    Serial.println(txt);
    Serial.println(txt, BIN);
    String x = String(txt, BIN);
    if (x.length() == 8) { // ตรวจสอบขั้น 1
      String head = x.substring(0, 4);
      String tail = x.substring(4, x.length());
      if ( head == tail && head == CH_BIT[num - 1] ) { // ตรวจสอบขั้น 2 และ 3
        Serial.println("Receive B2 at number : " + String(num));
        ch_lastB2 = true;
        Emer_ON = false;
      } else  if ( head == tail && head == CH_BIT[4] ) {
        // Emer on
        Serial.println("Emer ON");
        ch_lastB2 = true;
        Emer_ON = true;
      }
    }
    counter_GetData_B2 = 0;
  } else {
    if (ch_lastB2 == true) {
      if (counter_GetData_B2 > 15000) {
        Serial.println("OUT GO B2");
        ch_lastB2 = false;
        return true;
      } else {
        counter_GetData_B2++;
      }
    } else {
      counter_GetData_B2 = 0;
    }
  }
  return false;
}
void Control_B2(int num) {
  for (int i = 0; i < 3; i++)SendData_B2(num);
  while (true) {
    if (GetData_B2(num) == true)break;
  }
}

int range_up   = 1;
int range_down = 1;
void WebUpdate() {
  //  Serial.println(String(digitalRead(output0)) + String(digitalRead(output1)) + String(digitalRead(output2)) + String(digitalRead(output3)));
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("client Connect");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {
      currentTime = millis();
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /0/UP") >= 0 && range_up == 1)
            {
              state = "1U";
              output0State = "UP";
              Serial.println("4 roof up");

            }
            else if (header.indexOf("GET /0/DOWN") >= 0 && range_down == 1)
            {
              state = "1D";
              output0State = "DOWN";
              Serial.println("4 roof down");
            }
            else if (header.indexOf("GET /1/UP") >= 0 && range_up == 2)
            {
              state = "2U";
              Serial.println("3 up");
              output1State = "UP";

            }
            else if (header.indexOf("GET /1/DOWN") >= 0 && range_down == 2)
            {
              state = "2D";
              output1State = "DOWN";
              Serial.println("3 down");
            }
            else if (header.indexOf("GET /2/UP") >= 0 && range_up == 3)
            {
              state = "3U";
              Serial.println("2 up");
              output2State = "UP";

            }
            else if (header.indexOf("GET /2/DOWN") >= 0 && range_down == 3)
            {
              state = "3D";
              Serial.println("2 down");
              output2State = "DOWN";

            }
            else if (header.indexOf("GET /3/UP") >= 0 && range_up == 4)
            {
              state = "4U";
              Serial.println("1 up");
              output3State = "UP";

            }
            else if (header.indexOf("GET /3/DOWN") >= 0 && range_down == 4)
            {
              state = "4D";
              Serial.println("1 down");
              output3State = "DOWN";

            }
            if (state == "1U") {
              Control_B1(1);//Floor 1 UP
              Block_State("1U");
              state = "";

            } else   if (state == "1D") {
              Control_B1(2);
              Block_State("1D");
              state = "";
            } else   if (state == "2U") {
              Control_B1(3);
              Block_State("2U");
              state = "";
            } else   if (state == "2D") {
              Control_B1(4);
              Block_State("2D");
              state = "";
            } else   if (state == "3U") {
              Control_B2(1);
              Block_State("3U");
              state = "";
            } else   if (state == "3D") {
              Control_B2(2);
              Block_State("3D");
              state = "";
            } else   if (state == "4U") {
              Control_B2(3);
              Block_State("4U");
              state = "";
            } else   if (state == "4D") {
              Control_B2(4);
              Block_State("4D");
              state = "";
            }
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<title>Airport Model 3</title>\n");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            client.println("<body><h1>Airport Model 3</h1>");

            client.println("<p>Roof ( " + output0State + ")</p>");
            if (output0State == "DOWN")
            {
              client.println("<p><a href=\"/0/UP\"><button class=\"button\">UP</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/0/DOWN\"><button class=\"button button2\">DOWN</button></a></p>");
            }

            client.println("<p>Floor3 (" + output1State + ")</p>");
            if (output1State == "DOWN")
            {
              client.println("<p><a href=\"/1/UP\"><button class=\"button\">UP</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/1/DOWN\"><button class=\"button button2\">DOWN</button></a></p>");
            }

            client.println("<p>Floor2 (" + output2State + ")</p>");
            if (output2State == "DOWN")
            {
              client.println("<p><a href=\"/2/UP\"><button class=\"button\">UP</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/2/DOWN\"><button class=\"button button2\">DOWN</button></a></p>");
            }

            client.println("<p>Floor1 (" + output3State + ")</p>");
            if (output3State == "DOWN")
            {
              client.println("<p><a href=\"/3/UP\"><button class=\"button\">UP</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/3/DOWN\"><button class=\"button button2\">DOWN</button></a></p>");
            }

            client.println("</body></html>");
            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("client Disconnect");
    Serial.println("");
  }
  //  if (state == "1U") {
  //    Control_B1(1);//Floor 1 UP
  //    Block_State("1U");
  //    state = "";
  //
  //  } else   if (state == "1D") {
  //    Control_B1(2);
  //    Block_State("1D");
  //    state = "";
  //  } else   if (state == "2U") {
  //    Control_B1(3);
  //    Block_State("2U");
  //    state = "";
  //  } else   if (state == "2D") {
  //    Control_B1(4);
  //    Block_State("2D");
  //    state = "";
  //  } else   if (state == "3U") {
  //    Control_B2(1);
  //    Block_State("3U");
  //    state = "";
  //  } else   if (state == "3D") {
  //    Control_B2(2);
  //    Block_State("3D");
  //    state = "";
  //  } else   if (state == "4U") {
  //    Control_B2(3);
  //    Block_State("4U");
  //    state = "";
  //  } else   if (state == "4D") {
  //    Control_B2(4);
  //    Block_State("4D");
  //    state = "";
  //  }
}

void Block_State(String type) {
  if (Emer_ON == true) {
    if (type == "1U") {
      output0State = "DOWN";
      range_up = 1;
      range_down = 0;
    } else if (type == "1D") {
      output0State = "UP";
      range_up = 0;
      range_down = 1;
    } else if (type == "2U") {
      output1State = "DOWN";
      range_up = 2;
      range_down = 0;
    } else if (type == "2D") {
      output1State = "UP";
      range_up = 0;
      range_down = 2;
    } else if (type == "3U") {
      output2State = "DOWN";
      range_up = 3;
      range_down = 0;
    } else if (type == "3D") {
      output2State = "UP";
      range_up = 3;
      range_down = 3;
    } else if (type == "4U") {
      output3State = "DOWN";
      range_up = 4;
      range_down = 0;
    } else if (type == "4D") {
      output3State = "UP";
      range_up = 0;
      range_down = 4;
    }
    Emer_ON = false;
  } else {
    if (type == "1U") {
      //      Floor = 1;
      range_up = 2;
      range_down = 1;
    } else if (type == "1D") {
      //      Floor = 0;
      range_up = 1;
      range_down = 1;
    } else if (type == "2U") {
      //      Floor = 2;
      range_up = 3;
      range_down = 2;
    } else if (type == "2D") {
      //      Floor = 1;
      range_up = 2;
      range_down = 1;
    } else if (type == "3U") {
      //      Floor = 3;
      range_up = 4;
      range_down = 3;
    } else if (type == "3D") {
      //      Floor = 2;
      range_up = 3;
      range_down = 2;
    } else if (type == "4U") {
      //      Floor = 4;
      range_up = 4;
      range_down = 4;
    } else if (type == "4D") {
      //      Floor = 3;
      range_up = 4;
      range_down = 3;
    }
  }

}
