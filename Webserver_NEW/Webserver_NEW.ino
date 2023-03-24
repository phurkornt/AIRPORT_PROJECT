// -------------------------- Update at 24/03/2566 --------------------------

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial portOne(D5, D6);
SoftwareSerial portTwo(D1, D2);

String CH_BIT[5] = { "1010", "1011", "1100", "1110", "1111" };
int dat[5] = { 0b10101010, 0b10111011, 0b11001100, 0b11101110, 0b11111111 };
String state = "";
String color = "#00ffa5";

const char* ssid = "Creston Control";
const char* password = "";  ///@S123456
// const char* ssid     = "B 563_2.4GHz";
// const char* password = "B0625976969";///@S123456
//const char* ssid     = "22thMAYSA_2.4G";
//const char* password = "RoboAC22";///@S123456

IPAddress local_IP(192, 168, 0, 200);
IPAddress gateway(192, 168, 0, 254);
IPAddress subnet(255, 255, 255, 0);
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

int output0 = 4;   //4
int output1 = 0;   //0
int output2 = 2;   //2
int output3 = 14;  //14

int Floor = 0;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

unsigned long count_time_sender = 0;



bool Emer_ON = false;

String disable_button0 = "";
String disable_button1 = "";
String disable_button2 = "";
String disable_button3 = "";

/* --  --  --  --  -- */

String now_state = "1D";

/* --  --  --  --  -- */

void setup() {
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
  while (WiFi.status() != WL_CONNECTED) {
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
void StateUpdate() {
  if (Floor == 0) {
    output0State = "DOWN";
    output1State = "DOWN";
    output2State = "DOWN";
    output3State = "DOWN";
  } else if (Floor == 1) {
    output0State = "UP";
    output1State = "DOWN";
    output2State = "DOWN";
    output3State = "DOWN";
  } else if (Floor == 2) {
    output0State = "UP";
    output1State = "UP";
    output2State = "DOWN";
    output3State = "DOWN";
  } else if (Floor == 3) {
    output0State = "UP";
    output1State = "UP";
    output2State = "UP";
    output3State = "DOWN";
  } else if (Floor == 4) {
    output0State = "UP";
    output1State = "UP";
    output2State = "UP";
    output3State = "UP";
  }
}
void loop() {
  // Serial.println("NOW STATE : "+now_state);
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
    if (x.length() == 8) {  // ตรวจสอบขั้น 1
      String head = x.substring(0, 4);
      String tail = x.substring(4, x.length());
      if (head == tail && head == CH_BIT[num - 1]) {  // ตรวจสอบขั้น 2 และ 3

        if (num == 1) {
          now_state = "1U";
        } else if (num == 2) {
          now_state = "1D";
        } else if (num == 3) {
          now_state = "2U";
        } else if (num == 4) {
          now_state = "2D";
        }
        // now_state

        Serial.println("Receive B1 at number : " + String(num));
        ch_lastB1 = true;
        Emer_ON = false;
      } else if (head == tail && head == CH_BIT[4]) {
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

  for (int i = 0; i < 3; i++) SendData_B1(num);
  while (true) {
    if (Serial.available() > 0) {
      char t = Serial.read();
      Serial.println(t);
      if (t == '1') {
        break;
        Serial.println("Break !");
      }
    }
    if (GetData_B1(num) == true) break;
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
    if (x.length() == 8) {  // ตรวจสอบขั้น 1
      String head = x.substring(0, 4);
      String tail = x.substring(4, x.length());
      if (head == tail && head == CH_BIT[num - 1]) {  // ตรวจสอบขั้น 2 และ 3
        if (num == 1) {
          now_state = "3U";
        } else if (num == 2) {
          now_state = "3D";
        } else if (num == 3) {
          now_state = "4U";
        } else if (num == 4) {
          now_state = "4D";
        }
        Serial.println("Receive B2 at number : " + String(num));
        ch_lastB2 = true;
        Emer_ON = false;
      } else if (head == tail && head == CH_BIT[4]) {
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
  for (int i = 0; i < 3; i++) SendData_B2(num);
  while (true) {
    if (Serial.available() > 0) {
      char t = Serial.read();
      Serial.println(t);
      if (t == '1') {
        break;
        Serial.println("Break !");
      }
    }
    if (GetData_B2(num) == true) break;
  }
}

int range_up = 1;
int range_down = 0;
void WebUpdate() {
  //  Serial.println(String(digitalRead(output0)) + String(digitalRead(output1)) + String(digitalRead(output2)) + String(digitalRead(output3)));
  WiFiClient client = server.available();
  if (client) {
    Serial.println("client Connect");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();

        header += c;
        if (c == '\n') {
          //          Serial.println(header);
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /?0U") >= 0 && now_state == "1D") {
              state = "1U";
              output0State = "UP";
              Serial.println("4 roof up");

            } else if (header.indexOf("GET /?0D") >= 0 && ( now_state == "1U" || now_state == "2D"  ) ) {

              state = "1D";
              output0State = "DOWN";
              Serial.println("4 roof down");
            }

            else if (header.indexOf("GET /?1U") >= 0 && ( now_state == "1U" || now_state == "2D"  )  ) {
              state = "2U";
              Serial.println("3 up");
              output1State = "UP";

            } else if (header.indexOf("GET /?1D") >= 0 && ( now_state == "3D" || now_state == "2U"  ) ) {
              state = "2D";
              output1State = "DOWN";
              Serial.println("3 down");
            } else if (header.indexOf("GET /?2U") >= 0 && ( now_state == "2U" || now_state == "3D"  )) {
              state = "3U";
              Serial.println("2 up"); 
              output2State = "UP";

            } else if (header.indexOf("GET /?2D") >= 0 && ( now_state == "4D" || now_state == "3U"  )) {
              state = "3D";
              Serial.println("2 down");
              output2State = "DOWN";

            } else if (header.indexOf("GET /?3U") >= 0 && ( now_state == "3U" || now_state == "4D"  )) {
              state = "4U";
              Serial.println("1 up");
              output3State = "UP";

            } else if (header.indexOf("GET /?3D") >= 0 &&  now_state == "4U"  ) {
              state = "4D";
              Serial.println("1 down");
              output3State = "DOWN";
            }


            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<title>Airport Model 3</title>\n");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}");
            client.println(".button:hover{ background-color:#00ddff;} </style></head>");
            client.println("<body><h1>Airport Model 3</h1>");

            client.println("<p>Roof ( " + output0State + ")</p>");
            /* -Block Button- */
            if (output0State == "DOWN" && output1State == "DOWN" && output2State == "DOWN" && output3State == "DOWN") {
              disable_button0 = "";
              disable_button1 = "disabled";
              disable_button2 = "disabled";
              disable_button3 = "disabled";

            } else if (output0State == "UP" && output1State == "DOWN" && output2State == "DOWN" && output3State == "DOWN") {
              disable_button0 = "";
              disable_button1 = "";
              disable_button2 = "disabled";
              disable_button3 = "disabled";
            } else if (output0State == "UP" && output1State == "UP" && output2State == "DOWN" && output3State == "DOWN") {
              disable_button0 = "disabled";
              disable_button1 = "";
              disable_button2 = "";
              disable_button3 = "disabled";
            } else if (output0State == "UP" && output1State == "UP" && output2State == "UP" && output3State == "DOWN") {
              disable_button0 = "disabled";
              disable_button1 = "disabled";
              disable_button2 = "";
              disable_button3 = "";
            } else if (output0State == "UP" && output1State == "UP" && output2State == "UP" && output3State == "UP") {
              disable_button0 = "disabled";
              disable_button1 = "disabled";
              disable_button2 = "disabled";
              disable_button3 = "";
            }


            if (output0State == "DOWN") {
              client.println("<a href=\"/?0U\"><button id =\"b0\"class=\"button\" " + disable_button0 + " >UP</button></a><br>");
            } else {
              client.println("<a href=\"/?0D\"><button id =\"b0\" class=\"button button2\" " + disable_button0 + ">Down</button></a><br>");
            }
            client.println("<p>Floor3 (" + output1State + ")</p>");

            if (output1State == "DOWN") {
              client.println("<a href=\"/?1U\"><button id =\"b1\" class=\"button\"" + disable_button1 + ">UP</button></a><br>");
            } else {
              client.println("<a href=\"/?1D\"><button id =\"b1\" class=\"button button2\" " + disable_button1 + ">Down</button></a><br>");
            }
            client.println("<p>Floor2 (" + output2State + ")</p>");
            if (output2State == "DOWN") {
              client.println("<a href=\"/?2U\"><button id =\"b2\" class=\"button\" " + disable_button2 + ">UP</button></a><br>");
            } else {
              client.println("<a href=\"/?2D\"><button id =\"b2\" class=\"button button2\"" + disable_button2 + ">Down</button></a><br>");
            }
            client.println("<p>Floor1 (" + output3State + ")</p>");

            if (output3State == "DOWN") {
              client.println("<a href=\"/?3U\"><button id =\"b3\" class=\"button\"" + disable_button3 + ">UP</button></a><br>");
            } else {
              client.println("<a href=\"/?3D\"><button id =\"b3\" class=\"button button2\"" + disable_button3 + ">Down</button></a><br>");
            }
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    if (state == "1U") {

      client.println("<script>let a=document.getElementById('b0'); a.innerText=\"Moving Up\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");
      client.println("<script>document.getElementById('b1').disabled = true;</script>");

      Control_B1(1);  //Floor 1 UP

      client.println("<script>let b=document.getElementById('b0'); a.innerText=\"Down\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>document.getElementById('b1').disabled = false;</script>");
      client.println("<script>location.replace(\"/\");</script>");

      Block_State("1U");
      state = "";

    } else if (state == "1D") {

      client.println("<script>let a=document.getElementById('b0'); a.innerText=\"Moving Down\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");

      Control_B1(2);

      client.println("<script>let b=document.getElementById('b0'); a.innerText=\"UP\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>location.replace(\"/\");</script>");

      Block_State("1D");
      state = "";

    } else if (state == "2U") {

      client.println("<script>let a=document.getElementById('b1'); a.innerText=\"Moving Up\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");
      client.println("<script>document.getElementById('b2').disabled = true;</script>");
      Control_B1(3);
      client.println("<script>let b=document.getElementById('b1'); a.innerText=\"Down\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>document.getElementById('b2').disabled = false;</script>");
      client.println("<script>location.replace(\"/\");</script>");
      Block_State("2U");
      state = "";

    } else if (state == "2D") {

      client.println("<script>let a=document.getElementById('b1'); a.innerText=\"Moving Down\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");
      client.println("<script>document.getElementById('b0').disabled = true;</script>");
      Control_B1(4);
      client.println("<script>let b=document.getElementById('b1'); a.innerText=\"UP\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>document.getElementById('b0').disabled = false;</script>");
      client.println("<script>location.replace(\"/\");</script>");
      Block_State("2D");
      state = "";

    } else if (state == "3U") {

      client.println("<script>let a=document.getElementById('b2'); a.innerText=\"Moving Up\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");
      client.println("<script>document.getElementById('b3').disabled = true;</script>");
      Control_B2(1);
      client.println("<script>let b=document.getElementById('b2'); a.innerText=\"Down\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>document.getElementById('b3').disabled = false;</script>");
      client.println("<script>location.replace(\"/\");</script>");
      Block_State("3U");
      state = "";

    } else if (state == "3D") {

      client.println("<script>let a=document.getElementById('b2'); a.innerText=\"Moving Down\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");
      client.println("<script>document.getElementById('b1').disabled = true;</script>");
      Control_B2(2);
      client.println("<script>let b=document.getElementById('b2'); a.innerText=\"UP\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>document.getElementById('b1').disabled = false;</script>");
      client.println("<script>location.replace(\"/\");</script>");
      Block_State("3D");
      state = "";

    } else if (state == "4U") {

      client.println("<script>let a=document.getElementById('b3'); a.innerText=\"Moving Up\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");
      Control_B2(3);
      client.println("<script>let b=document.getElementById('b3'); a.innerText=\"Down\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>location.replace(\"/\");</script>");
      Block_State("4U");
      state = "";

    } else if (state == "4D") {

      client.println("<script>let a=document.getElementById('b3'); a.innerText=\"Moving Down\"; a.disabled = true;a.style.backgroundColor=\"" + color + "\";</script>");
      client.println("<script>document.getElementById('b2').disabled = true;</script>");
      Control_B2(4);
      client.println("<script>let b=document.getElementById('b3'); a.innerText=\"UP\"; a.disabled = false; a.style.backgroundColor= \"\";</script>");
      client.println("<script>document.getElementById('b2').disabled = false;</script>");
      client.println("<script>location.replace(\"/\");</script>");
      Block_State("4D");
      state = "";
    }

    header = "";
    client.stop();
    Serial.println("client Disconnect");
    Serial.println("");
  }
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
      range_up = 5;
      range_down = 4;
    } else if (type == "4D") {
      //      Floor = 3;
      range_up = 4;
      range_down = 3;
    }
  }
}



/* -------------------------------------------------------- */
