
#define sw4 57

#define manual_3 A8
#define manual_4 A9



byte Floor_3A[6] = { 34, 38, 42, 46, 50, 23 };
byte Floor_3B[6] = { 36, 40, 44, 48, 52, 25 };
byte Floor_3PWM[6] = { 5, 6, 7, 8, 9, 10 };

byte Floor_4A[6] = { 30, 26, 22, 27, 31, 35 };
byte Floor_4B[6] = { 32, 28, 24, 29, 33, 37 };
byte Floor_4PWM[6] = { 2, 3, 4, 11, 12, 13 };

byte F3_PWM_UP[7] = { 157, 142, 152, 147, 155, 147 };
byte F3_PWM_DOWN[7] = { 187, 152, 177, 157, 192, 120 };

byte F4_PWM_UP[7] = { 235, 185, 170, 157, 175, 205 };
byte F4_PWM_DOWN[7] = { 185, 147, 200, 225, 130, 250 };

byte Limit_F3[2][2] = { { 41, 39 }, { 45, 43 } };  //-> [0][D-U]F & [1][D-U]B
byte Limit_F4[2][2] = { { 49, 47 }, { 53, 51 } };  //-> [0][D-U]F & [1][D-U]B

String CH_BIT[4] = { "1010", "1011", "1100", "1110" };
/*
  **encode floor control**
  String CH_BIT[4] = {"1010", "1011", "1100", "1110"};
                        1U      1D       2U      2D
  1U = Floor 1 move up
  1D = Floor 1 move Down
  2U = Floor 2 move up
  2D = Floor 2 move Down
*/

bool Emer_ON = false;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial3.begin(9600);
  Serial.println("Start MCU 2");
  Setup_Pin();
  pinMode(manual_3, INPUT_PULLUP);
  pinMode(manual_4, INPUT_PULLUP);
}

void loop() {

  /*---- !!เปิดครั้งล่ะ 1 function!!---*/

  /*----------Codeหลัก----------*/
  // Serial.print(digitalRead(A8));
  //   Serial.print(" ");
  //   Serial.println(digitalRead(A9));
  MainControl();

  /*----------Codeหลัก----------*/



  /*----------Code test อ่านค่า & Drive motor----------*/

  // For_test_Get_Send_Data();
  // For_test_Limit_3();
  // For_test_Limit_4();

  /*  motor 
      การใช้งานเลือก motor ด้วยเลข 1-6 
      เลือกการ Drive
      u = ขึ้น
      d = ลง
      s = หยุด
  */

  // For_test_motorF3();
  // For_test_motorF4();
  // Serial.println(isStop());

  /*----------Code test อ่านค่า & Drive motor----------*/
}

char mode_select_test;

char motor_select[6];
char motor_test[6] = { '1', '2', '3', '4', '5', '6' };

void For_test_motorF4() {
  // void drive_floor1(byte sel, char selUD) {
  // drive_floor1(1,'U');
  // Serial.println(isStop());
  if (isStop() == false) {
    if (Serial.available() > 0) {

      char r = Serial.read();
      if (r == 'u') {
        mode_select_test = 'U';
      } else if (r == 'd') {
        mode_select_test = 'D';
      } else if (r == 's') {
        mode_select_test = 'S';
      } else {
        for (int i = 0; i < 6; i++) {
          if (r == motor_test[i]) {
            motor_select[i] = motor_test[i];
          }
        }
      }

      for (int i = 0; i < 6; i++) {
        if (motor_select[i] == motor_test[i]) {
          drive_floor4((i + 1), mode_select_test);
        }
        Serial.println(String(motor_select[i]) + " ");
        // Serial.println(String(motor_select[i])+" ");
      }
      delay(100);
      mode_select_test = 'S';
      for (int i = 0; i < 6; i++) {

        drive_floor4((i + 1), 'S');
      }

      Serial.println("MODE : " + String(mode_select_test));
    }
  } else {
    StopAll();
    Serial.println("STOP");
  }
}

void For_test_motorF3() {
  // void drive_floor1(byte sel, char selUD) {
  // drive_floor1(1,'U');
  // Serial.println(isStop());
  if (isStop() == false) {
    if (Serial.available() > 0) {

      char r = Serial.read();
      if (r == 'u') {
        mode_select_test = 'U';
      } else if (r == 'd') {
        mode_select_test = 'D';
      } else if (r == 's') {
        mode_select_test = 'S';
      } else {
        for (int i = 0; i < 6; i++) {
          if (r == motor_test[i]) {
            motor_select[i] = motor_test[i];
          }
        }
      }

      for (int i = 0; i < 6; i++) {
        if (motor_select[i] == motor_test[i]) {
          drive_floor3((i + 1), mode_select_test);
        }
        Serial.println(String(motor_select[i]) + " ");
        // Serial.println(String(motor_select[i])+" ");
      }
      delay(100);
      mode_select_test = 'S';
      for (int i = 0; i < 6; i++) {

        drive_floor3((i + 1), 'S');
      }

      Serial.println("MODE : " + String(mode_select_test));
    }
  } else {
    StopAll();
    Serial.println("STOP");
  }
}
void For_test_Limit_3() {
  StopAll();
  /*

    Limit_F1[0][0] -> Limit ชุดหน้า ตัวล่าง
    Limit_F1[1][0] -> Limit ชุดหลัง ตัวล่าง

    Limit_F1[0][1] -> Limit ชุดหน้า ตัวบน
    Limit_F1[1][1] -> Limit ชุดหลัง ตัวบน
  
  */

  if (digitalRead(Limit_F3[0][0]) == 0) {
    Serial.println("Font3 Down");
  }
  if (digitalRead(Limit_F3[1][0]) == 0) {
    Serial.println("Back3 Down");
  }

  if (digitalRead(Limit_F3[0][1]) == 0) {
    Serial.println("Font3 Top");
  }


  if (digitalRead(Limit_F3[1][1]) == 0) {
    Serial.println("Back3 Top");
  }
}
void For_test_Limit_4() {
  StopAll();
  /*

    Limit_F1[0][0] -> Limit ชุดหน้า ตัวล่าง
    Limit_F1[1][0] -> Limit ชุดหลัง ตัวล่าง

    Limit_F1[0][1] -> Limit ชุดหน้า ตัวบน
    Limit_F1[1][1] -> Limit ชุดหลัง ตัวบน
  
  */

  if (digitalRead(Limit_F4[0][0]) == 0) {
    Serial.println("Font4 Down");
  }
  if (digitalRead(Limit_F4[1][0]) == 0) {
    Serial.println("Back4 Down");
  }

  if (digitalRead(Limit_F4[0][1]) == 0) {
    Serial.println("Font4 Top");
  }


  if (digitalRead(Limit_F4[1][1]) == 0) {
    Serial.println("Back4 Top");
  }
}

void MainControl() {
  if (isStop() == true) {
    if (digitalRead(manual_3) == 0) {
      // Serial.println("man 3");
      for (int i = 0; i < 6; i++) {
        drive_floor3((i + 1), 'D');
      }
    } else {
      for (int i = 0; i < 6; i++) {
        drive_floor3((i + 1), 'S');
      }
    }
    if (digitalRead(manual_4) == 0) {
      // Serial.println("man 4");
      for (int i = 0; i < 6; i++) {
        drive_floor4((i + 1), 'D');
      }
    } else {
      for (int i = 0; i < 6; i++) {
        drive_floor4((i + 1), 'S');
      }
    }
  }
  Control();
}
void FeedBack(byte num) {
  if (num > 4) return;
  int dat[5] = { 0b10101010, 0b10111011, 0b11001100, 0b11101110, 0b11111111 };
  if (isStop() == true) num = 5;
  for (int i = 0; i < 3; i++) {
    Serial3.write(dat[num - 1]);
    delay(20);
  }
  Serial.println("Done at number : " + String(num));
}
void Detroy_Data() {
  while (Serial.available() > 0) {
    char for_leave = Serial.read();
  }
}
String state = "";
int counter_Control = 0;
bool ch_last = false;
void Control() {
  /*  Control รับข้อมูลจาก ESP ตรวจสอบข้อมูล จากนั้นเมื่อไม่มีข้อมูลมาสักระยะ ก็จะให้เริ่มทำงาน State  */
  if (Serial3.available() > 0) {
    int txt = Serial3.read();
    Serial.println(txt, BIN);
    String x = String(txt, BIN);
    //     Serial.println(x.length());
    if (x.length() == 8) {  // ตรวจสอบขั้น 1
      String head = x.substring(0, 4);
      String tail = x.substring(4, x.length());
      for (int i = 0; i < 4; i++) {
        if (head == tail && head == CH_BIT[i]) {  // ตรวจสอบขั้น 2 และ 3
          Serial.println(" True with floor B2 :" + String(i + 1));
          if ((i + 1) == 1) {
            state = "3U";
            ch_last = true;
            Detroy_Data();
          } else if ((i + 1) == 2) {
            state = "3D";
            ch_last = true;
            Detroy_Data();
          } else if ((i + 1) == 3) {
            state = "4U";
            ch_last = true;
            Detroy_Data();
          } else if ((i + 1) == 4) {
            state = "4D";
            ch_last = true;
            Detroy_Data();
          }
        }
      }
    }
    counter_Control = 0;
  } else {
    if (ch_last == true) {
      if (counter_Control > 1000) {
        if (state == "3U") {
          Serial.println("Run Floor 3 UP");
          // 3 ขึ้นได้ 2 ขึ้นก่อน
          control_Auto_Floor3('U');
          FeedBack(1);

          state = "";
        } else if (state == "3D") {
          Serial.println("Run Floor 3 Down");
          // 3ลงได้ 4ลงก่อน
          if (digitalRead(Limit_F4[0][0]) == 0 && digitalRead(Limit_F4[1][0]) == 0) {
            control_Auto_Floor3('D');
            FeedBack(2);
          }


          state = "";
        } else if (state == "4U") {
          Serial.println("Run Floor 4 Up");
          // 4ขึ้นได้ 3ขึ้นก่อน
          if (digitalRead(Limit_F3[0][1]) == 0 && digitalRead(Limit_F3[1][1]) == 0) {
            control_Auto_Floor4('U');
            FeedBack(3);
          }

          state = "";
        } else if (state == "4D") {
          Serial.println("Run Floor 4 Down");
          // 4ลงได้ตลอด
          control_Auto_Floor4('D');
          FeedBack(4);
          state = "";
        }
        ch_last = false;
      } else {
        counter_Control++;
        // add on 02/12/65
        if (counter_Control > 2000) counter_Control = 2000;
        // add on 02/12/65
      }
    } else {
      counter_Control = 0;
    }
  }
}

void control_Auto_Floor3(char pick) {
  /*
     pick , U (move up) , D (move down)
    ------------- !comment --------------------
    byte Limit_F3[2][2] = {{41, 39}, {45, 43}}; //-> [0][D-U]F & [1][D-U]B

    Limit_F1[0][0] -> Limit ชุดหน้า ตัวล่าง
    Limit_F1[0][1] -> Limit ชุดหน้า ตัวบน
    Limit_F1[1][0] -> Limit ชุดหลัง ตัวล่าง
    Limit_F1[1][1] -> Limit ชุดหลัง ตัวบน

  */
  if (pick == 'U') {
    // ------------------------------ Floor 3 UP ------------------------------
    byte counter = 0;
    if (digitalRead(Limit_F3[0][1]) != 0 && digitalRead(Limit_F3[1][1]) != 0) {
      while (true) {
        // Serial.println("Floor 3 UP");
        if (isStop() == true) {
          //Emergency
          Serial.println("EM WORK At Floor 3 UP");
          StopAll();
          // break;
        } else {
          if (digitalRead(Limit_F3[0][1]) == 0 && digitalRead(Limit_F3[1][1]) == 0) {
            Serial.println("have stop while");
            Drive_Floor_3('F', 'S');
            Drive_Floor_3('B', 'S');
            break;
          }
          if (digitalRead(Limit_F3[0][1]) != 0) {
            Drive_Floor_3('F', 'U');
          } else {
            Serial.println("UPFONT FOUND");
            Drive_Floor_3('F', 'S');
          }
          if (digitalRead(Limit_F3[1][1]) != 0) {

            Drive_Floor_3('B', 'U');
          } else {
            Serial.println("UPBACK FOUND");
            Drive_Floor_3('B', 'S');
          }
        }
      }
    }
  } else if (pick == 'D') {
    byte counter = 0;
    if (digitalRead(Limit_F3[0][0]) != 0 && digitalRead(Limit_F3[1][0]) != 0) {
      while (true) {
        Serial.println("Floor 3 Down");
        if (isStop() == true) {
          //Emergency
          Serial.println("EM WORK At Floor 3 DOWN");
          StopAll();
          // break;
        } else {
          if (digitalRead(Limit_F3[0][0]) == 0 && digitalRead(Limit_F3[1][0]) == 0) {
            Drive_Floor_3('F', 'S');
            Drive_Floor_3('B', 'S');
            break;
          }
          if (digitalRead(Limit_F3[0][0]) != 0) {
            Drive_Floor_3('F', 'D');
          } else {
            Drive_Floor_3('F', 'S');
          }
          if (digitalRead(Limit_F3[1][0]) != 0) {
            Drive_Floor_3('B', 'D');
          } else {
            Drive_Floor_3('B', 'S');
          }
        }
      }
    }
  }
}

void control_Auto_Floor4(char pick) {
  /*
     pick , U (move up) , D (move down)
    ------------- !comment --------------------
    byte Limit_F3[2][2] = {{41, 39}, {45, 43}}; //-> [0][D-U]F & [1][D-U]B

    Limit_F1[0][0] -> Limit ชุดหน้า ตัวล่าง
    Limit_F1[0][1] -> Limit ชุดหน้า ตัวบน
    Limit_F1[1][0] -> Limit ชุดหลัง ตัวล่าง
    Limit_F1[1][1] -> Limit ชุดหลัง ตัวบน

  */
  if (pick == 'U') {
    // ------------------------------ Floor 1 UP ------------------------------
    byte counter = 0;
    if (digitalRead(Limit_F4[0][1]) != 0 && digitalRead(Limit_F4[1][1]) != 0) {
      while (true) {
        Serial.println("Floor 4 UP");
        if (isStop() == true) {
          //Emergency
          Serial.println("EM WORK At Floor 4 UP");
          StopAll();
          // break;
        } else {
          if (digitalRead(Limit_F4[0][1]) == 0 && digitalRead(Limit_F4[1][1]) == 0) {
            Drive_Floor_4('F', 'S');
            Drive_Floor_4('B', 'S');
            break;
          }
          if (digitalRead(Limit_F4[0][1]) != 0) {
            Drive_Floor_4('F', 'U');
          } else {
            Drive_Floor_4('F', 'S');
          }
          if (digitalRead(Limit_F4[1][1]) != 0) {
            Drive_Floor_4('B', 'U');
          } else {
            Drive_Floor_4('B', 'S');
          }
        }
      }
    }
  } else if (pick == 'D') {
    byte counter = 0;
    if (digitalRead(Limit_F4[0][0]) != 0 && digitalRead(Limit_F4[1][0]) != 0) {
      while (true) {
        Serial.println("Floor 4 Down");
        if (isStop() == true) {
          //Emergency
          Serial.println("EM WORK At Floor 4 Down");
          StopAll();
          // break;
        } else {
          if (digitalRead(Limit_F4[0][0]) == 0 && digitalRead(Limit_F4[1][0]) == 0) {
            Drive_Floor_4('F', 'S');
            Drive_Floor_4('B', 'S');
            break;
          }
          if (digitalRead(Limit_F4[0][0]) != 0) {
            Drive_Floor_4('F', 'D');
          } else {
            Drive_Floor_4('F', 'S');
          }
          if (digitalRead(Limit_F4[1][0]) != 0) {
            Drive_Floor_4('B', 'D');
          } else {
            Drive_Floor_4('B', 'S');
          }
        }
      }
    }
  }
}


void Setup_Pin() {
  for (int i = 0; i < 6; i++) {
    pinMode(Floor_3A[i], OUTPUT);
    pinMode(Floor_3B[i], OUTPUT);
    pinMode(Floor_3PWM[i], OUTPUT);

    pinMode(Floor_4A[i], OUTPUT);
    pinMode(Floor_4B[i], OUTPUT);
    pinMode(Floor_4PWM[i], OUTPUT);
  }
  for (int i = 0; i < 2; i++) {
    for (int x = 0; x < 2; x++) {
      pinMode(Limit_F3[i][x], INPUT_PULLUP);
      pinMode(Limit_F4[i][x], INPUT_PULLUP);
    }
  }
  pinMode(sw4, INPUT_PULLUP);
}
void StopAll() {
  for (int i = 0; i < 6; i++) {
    drive_floor3((i + 1), 'S');
    drive_floor4((i + 1), 'S');
  }
}

bool isStop() {
  return digitalRead(sw4) == 1;  //USE THIS
  // return digitalRead(sw4) == 0;  //JUST TESE 1/5/2565
}

void Drive_Floor_3(char sel_LR_ch, char selUD) {
  // Control Drive All motor Floor 1,(F ชุดหน้า),(B ชุดหลัง)
  //  Serial.println("Drive Floor 1 ");
  if (sel_LR_ch == 'F') {
    drive_floor3(1, selUD);
    drive_floor3(2, selUD);
    drive_floor3(4, selUD);
  } else if (sel_LR_ch == 'B') {
    drive_floor3(3, selUD);
    drive_floor3(5, selUD);
    drive_floor3(6, selUD);
  }
}
void drive_floor3(byte sel, char selUD) {

  // Control Drive each motor
  sel = sel - 1;
  if (selUD == 'U') {
    digitalWrite(Floor_3A[sel], HIGH);
    digitalWrite(Floor_3B[sel], LOW);
    analogWrite(Floor_3PWM[sel], F3_PWM_UP[sel]);
    Serial.println("Drive Floor 3 UP ");
  } else if (selUD == 'D') {
    digitalWrite(Floor_3A[sel], LOW);
    digitalWrite(Floor_3B[sel], HIGH);
    analogWrite(Floor_3PWM[sel], F3_PWM_DOWN[sel]);
  } else if (selUD == 'S') {
    digitalWrite(Floor_3A[sel], LOW);
    digitalWrite(Floor_3B[sel], LOW);
    analogWrite(Floor_3PWM[sel], 0);
  }
}
void Drive_Floor_4(char sel_LR_ch, char selUD) {
  // Control Drive All motor Floor 1,(F ชุดหน้า),(B ชุดหลัง)
  if (sel_LR_ch == 'F') {
    drive_floor4(1, selUD);
    drive_floor4(2, selUD);
    drive_floor4(4, selUD);
  } else if (sel_LR_ch == 'B') {
    drive_floor4(3, selUD);
    drive_floor4(5, selUD);
    drive_floor4(6, selUD);
  }
}
void drive_floor4(byte sel, char selUD) {
  // Control Drive each motor
  sel = sel - 1;
  if (selUD == 'U') {
    digitalWrite(Floor_4A[sel], HIGH);
    digitalWrite(Floor_4B[sel], LOW);
    analogWrite(Floor_4PWM[sel], F4_PWM_UP[sel]);
  } else if (selUD == 'D') {
    digitalWrite(Floor_4A[sel], LOW);
    digitalWrite(Floor_4B[sel], HIGH);
    analogWrite(Floor_4PWM[sel], F4_PWM_DOWN[sel]);
  } else if (selUD == 'S') {
    digitalWrite(Floor_4A[sel], LOW);
    digitalWrite(Floor_4B[sel], LOW);
    analogWrite(Floor_4PWM[sel], 0);
  }
}
