// V2
#define sw_emer A3
#define sw1 A0
#define sw2 A1

#define sw3 A2
#define sw4 A4


byte Floor_1A[6] = {22, 26, 30, 34, 38, 42};
byte Floor_1B[6] = {24, 28, 32, 36, 40, 44};
byte Floor_1PWM[6] = {2, 3, 4, 5, 6, 7};

byte Floor_2A[6] = {46, 50, 23, 27, 31, 35};
byte Floor_2B[6] = {48, 52, 25, 29, 33, 37};
byte Floor_2PWM[6] = {8, 9, 10, 13, 12, 11};

byte F1_PWM_UP[7] = {187, 150, 197, 167, 164, 183};
byte F1_PWM_DOWN[7] = {127, 140, 93, 107, 145, 134};

byte F2_PWM_UP[7] = {142, 147, 165, 165, 137, 152};
byte F2_PWM_DOWN[7] = {130, 125, 130, 177, 120, 170};

byte Limit_F1[2][2] = { {63, 62}, {65, 64}}; //-> [0][D-U]F & [1][D-U]B
byte Limit_F2[2][2] = {{67, 66}, {69, 68}}; //-> [0][D-U]F & [1][D-U]B

String CH_BIT[4] = {"1010", "1011", "1100", "1110"};
/*
  **encode floor control**
  String CH_BIT[4] = {"1010", "1011", "1100", "1110" ,   };
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
  Serial.println("Start");
  Setup_Pin();

}

void loop() {
  for (int i = 0; i < 2; i++) {
    for (int x = 0; x < 2; x++) {
      Serial.print(digitalRead(Limit_F1[i][x]));
      Serial.print("  ");
    }
  }
  for (int i = 0; i < 2; i++) {
    for (int x = 0; x < 2; x++) {
      Serial.print(digitalRead(Limit_F2[i][x]));
      Serial.print("  ");
    }

  }
  Serial.println("  ");
  //  Serial.println(String(digitalRead(sw1))+String(digitalRead(sw2))+String(digitalRead(sw3))+String(digitalRead(sw4))+String(digitalRead(sw_emer)));
  manual_control();
}
/*    ------ toggle sw ------*/
int toggle_sw1 = 1;
int toggle_sw2 = 1;
int toggle_sw3 = 1;
int toggle_sw4 = 1;

void manual_control() {

  if ( digitalRead(sw1) == 0) {
    delay(50);
    while (digitalRead(sw1) == 0) {}
    if (toggle_sw1 == 1) {
      control_Auto_Floor1('U');
      toggle_sw1 = 2;
    } else if (toggle_sw1 == 2) {
      control_Auto_Floor1('D');
      toggle_sw1 = 1;
    }
  }
  if ( digitalRead(sw2) == 0) {
    delay(50);
    while (digitalRead(sw2) == 0) {}
    if (toggle_sw2 == 1) {
      control_Auto_Floor2('U');
      toggle_sw2 = 2;
    } else if (toggle_sw2 == 2) {
      control_Auto_Floor2('D');
      toggle_sw2 = 1;
    }
  }
  if ( digitalRead(sw3) == 0) {
    delay(50);
    while (digitalRead(sw3) == 0) {}
    if (toggle_sw3 == 1) {
      Serial3.print("[F3:U]");
      toggle_sw3 = 2;
    } else if (toggle_sw3 == 2) {
      Serial3.print("[F3:D]");
      toggle_sw3 = 1;
    }
  }

  if ( digitalRead(sw4) == 0) {
    delay(50);
    while (digitalRead(sw4) == 0) {}
    if (toggle_sw4 == 1) {
      Serial3.print("[F4:U]");
      toggle_sw4 = 2;
    } else if (toggle_sw4 == 2) {
      Serial3.print("[F4:D]");
      toggle_sw4 = 1;
    }
  }
}

void MainControl() {
  Control();
}
void FeedBack(byte num) {
  if (num > 4)return ;
  int dat[5] = {0b10101010, 0b10111011, 0b11001100, 0b11101110, 0b11111111};

  /*
  **encode floor control**
    dat[4] = {0b10101010, 0b10111011, 0b11001100, 0b11101110,0b11111111};
                1U          1D           2U        2D        Emer ON
    1U = Floor 1 move up
    1D = Floor 1 move Down
    2U = Floor 2 move up
    2D = Floor 2 move Down
  */
  if (isStop() == true)num = 5;
  for (int i = 0; i < 3; i++) {
    Serial3.write(dat[num - 1]);
    delay(20);
  }
  Serial.println("Done at number : " + String(num));
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
    if (x.length() == 8) { // ตรวจสอบขั้น 1
      String head = x.substring(0, 4);
      String tail = x.substring(4, x.length());
      for (int i = 0; i < 4; i++) {
        if ( head == tail && head == CH_BIT[i] ) { // ตรวจสอบขั้น 2 และ 3
          Serial.println(" True with floor " + String(i + 1));
          if ((i + 1) == 1 ) {
            state = "1U";
            ch_last = true;
          } else if ((i + 1) == 2) {
            state = "1D";
            ch_last = true;
          } else if ((i + 1) == 3) {
            state = "2U";
            ch_last = true;
          } else if ((i + 1) == 4) {
            state = "2D";
            ch_last = true;
          }
        }
      }
    }
    counter_Control = 0;
  } else {
    if (ch_last == true) {
      if (counter_Control > 1000) {
        if (state == "1U") {
          Serial.println("Run Floor 1 UP");
          control_Auto_Floor1('U');
          FeedBack(1);
          state = "";
        } else if (state == "1D") {
          Serial.println("Run Floor 1 Down");
          control_Auto_Floor1('D');
          FeedBack(2);
          state = "";
        } else if (state == "2U") {
          Serial.println("Run Floor 2 Up");
          control_Auto_Floor2('U');
          FeedBack(3);
          state = "";
        } else if (state == "2D") {
          Serial.println("Run Floor 2 Down");
          control_Auto_Floor2('D');
          FeedBack(4);
          state = "";
        }
        ch_last = false;
      } else {
        counter_Control++;
      }
    } else {
      counter_Control = 0;
    }

  }
}

void control_Auto_Floor1(char pick) {
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
    // ถ้า Limit เจอเเล้ว ไม่ให้เข้า while
    if (digitalRead(Limit_F1[0][1]) != 0 && digitalRead(Limit_F1[1][1]) != 0) {

      while (true)
      {
        Serial.println("Floor 1 UP");
        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 1 up");
          StopAll();
          break;
        }
        if (digitalRead(Limit_F1[0][1]) == 0 && digitalRead(Limit_F1[1][1]) == 0)
        {
          Serial.println("have stop while");
          Drive_Floor_1('F', 'S');
          Drive_Floor_1('B', 'S');
          break;
        }

        if ( digitalRead(Limit_F1[0][1]) != 0) {
          Drive_Floor_1('F', 'U');
        } else {
          Serial.println("UPFONT FOUND");
          Drive_Floor_1('F', 'S');
        }
        if ( digitalRead(Limit_F1[1][1]) != 0) {

          Drive_Floor_1('B', 'U');
        } else {
          Serial.println("UPBACK FOUND");
          Drive_Floor_1('B', 'S');
        }

      }
    }
  } else if (pick == 'D') {
    byte counter = 0;
    if (digitalRead(Limit_F1[0][0]) != 0 && digitalRead(Limit_F1[1][0]) != 0) {
      while (true) {
        Serial.println("Floor 1 Down");
        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 1 Down");
          StopAll();
          break;
        }
        if (digitalRead(Limit_F1[0][0]) == 0 && digitalRead(Limit_F1[1][0]) == 0) {

          Drive_Floor_1('F', 'S');
          Drive_Floor_1('B', 'S');
          break;
        }
        if ( digitalRead(Limit_F1[0][0]) != 0) {
          Serial.println("Down font FOUND");
          Drive_Floor_1('F', 'D');
        } else {
          Drive_Floor_1('F', 'S');
        }
        if ( digitalRead(Limit_F1[1][0]) != 0) {
          Serial.println("Down back FOUND");
          Drive_Floor_1('B', 'D');
        } else {
          Drive_Floor_1('B', 'S');
        }

      }
    }
  }
}
void control_Auto_Floor2(char pick) {
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
    if (digitalRead(Limit_F2[0][1]) != 0 && digitalRead(Limit_F2[1][1]) != 0) {
      while (true)
      {
        Serial.println("Floor 2 UP");
        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 2 UP");
          StopAll();
          break;
        }
        if (digitalRead(Limit_F2[0][1]) == 0 && digitalRead(Limit_F2[1][1]) == 0)
        {
          Drive_Floor_2('F', 'S');
          Drive_Floor_2('B', 'S');
          break;

        }
        if ( digitalRead(Limit_F2[0][1]) != 0) {

          Drive_Floor_2('F', 'U');
        } else {
          Serial.println("up font FOUND");
          Drive_Floor_2('F', 'S');
        }
        if ( digitalRead(Limit_F2[1][1]) != 0) {
          
          Drive_Floor_2('B', 'U');
        } else {
          Serial.println("up back FOUND");
          Drive_Floor_2('B', 'S');
        }
      }
    }
  } else if (pick == 'D') {
    byte counter = 0;
    if (digitalRead(Limit_F2[0][0]) != 0 && digitalRead(Limit_F2[1][0]) != 0) {
      while (true) {
        Serial.println("Floor 2 Down");
        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 2 Down");
          StopAll();
          break;
        }
        if ( digitalRead(Limit_F2[0][0]) != 0) {
          
          Drive_Floor_2('F', 'D');
        } else {
          Serial.println("down font FOUND");
          Drive_Floor_2('F', 'S');
        }
        if ( digitalRead(Limit_F2[1][0]) != 0) {
         
          Drive_Floor_2('B', 'D');
        } else {
           Serial.println("down back FOUND");
          Drive_Floor_2('B', 'S');
        }
        if (digitalRead(Limit_F2[0][0]) == 0 && digitalRead(Limit_F2[1][0]) == 0) {
          Drive_Floor_2('F', 'S');
          Drive_Floor_2('B', 'S');
          break;
        }
      }
    }
  }
}

void Setup_Pin() {
  for (int i = 0; i < 6; i++) {
    pinMode(Floor_1A[i], OUTPUT);
    pinMode(Floor_1B[i], OUTPUT);
    pinMode(Floor_1PWM[i], OUTPUT);

    pinMode(Floor_2A[i], OUTPUT);
    pinMode(Floor_2B[i], OUTPUT);
    pinMode(Floor_2PWM[i], OUTPUT);
  }
  for (int i = 0; i < 2; i++) {
    for (int x = 0; x < 2; x++) {
      pinMode(Limit_F1[i][x], INPUT_PULLUP);
      pinMode(Limit_F2[i][x], INPUT_PULLUP);
    }
  }
  pinMode(sw_emer, INPUT_PULLUP);
  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);
  pinMode(sw3, INPUT_PULLUP);
  pinMode(sw4, INPUT_PULLUP);

}

void StopAll() {
  for (int i = 0; i < 6; i++) {
    drive_floor1( (i + 1) , 'S');
    drive_floor2( (i + 1) , 'S');
  }
}


bool isStop() {
  //  return digitalRead(sw_emer) == 1; //USE THIS
  return digitalRead(sw_emer) == 0; //JUST TESE 1/5/2565
}


// Below is Drive Motor
void Drive_Floor_1(char sel_LR_ch , char selUD ) {
  // Control Drive All motor Floor 1,(F ชุดหน้า),(B ชุดหลัง)
  //  Serial.println("Drive Floor 1 ");
  if (sel_LR_ch == 'F') {
    drive_floor1(1, selUD);
    drive_floor1(2, selUD);
    drive_floor1(4, selUD);
  } else if (sel_LR_ch == 'B') {
    drive_floor1(3, selUD);
    drive_floor1(5, selUD);
    drive_floor1(6, selUD);
  }
}
void drive_floor1(byte sel, char selUD) {

  // Control Drive each motor
  sel = sel - 1;
  if (selUD == 'U') {
    digitalWrite(Floor_1A[sel], HIGH);
    digitalWrite(Floor_1B[sel], LOW);
    analogWrite(Floor_1PWM[sel], F1_PWM_UP[sel]);
    //    Serial.println("Drive Floor 1 UP ");
  } else if (selUD == 'D') {
    digitalWrite(Floor_1A[sel], LOW);
    digitalWrite(Floor_1B[sel], HIGH);
    analogWrite(Floor_1PWM[sel], F1_PWM_DOWN[sel]);
  } else if (selUD == 'S') {
    digitalWrite(Floor_1A[sel], LOW);
    digitalWrite(Floor_1B[sel], LOW);
    analogWrite(Floor_1PWM[sel], 0);
  }
}

void Drive_Floor_2(char sel_LR_ch , char selUD ) {
  // Control Drive All motor Floor 1,(F ชุดหน้า),(B ชุดหลัง)
  if (sel_LR_ch == 'F') {
    drive_floor2(1, selUD);
    drive_floor2(2, selUD);
    drive_floor2(3, selUD);
  } else if (sel_LR_ch == 'B') {
    drive_floor2(4, selUD);
    drive_floor2(5, selUD);
    drive_floor2(6, selUD);
  }
}
void drive_floor2(byte sel, char selUD) {
  // Control Drive each motor
  sel = sel - 1;
  if (selUD == 'U') {
    digitalWrite(Floor_2A[sel], HIGH);
    digitalWrite(Floor_2B[sel], LOW);
    analogWrite(Floor_2PWM[sel], F2_PWM_UP[sel]);
  } else if (selUD == 'D') {
    digitalWrite(Floor_2A[sel], LOW);
    digitalWrite(Floor_2B[sel], HIGH);
    analogWrite(Floor_2PWM[sel], F2_PWM_DOWN[sel]);
  } else if (selUD == 'S') {
    digitalWrite(Floor_2A[sel], LOW);
    digitalWrite(Floor_2B[sel], LOW);
    analogWrite(Floor_2PWM[sel], 0);
  }
}
// Below is Drive Motor






/*
  void Control() {

  if (Serial3.available() > 0) {
    int txt = Serial3.read();
    Serial.println(txt, BIN);
    String x = String(txt, BIN);
    //     Serial.println(x.length());
    if (x.length() == 8) { // ตรวจสอบขั้น 1
      String head = x.substring(0, 4);
      String tail = x.substring(4, x.length());
      for (int i = 0; i < 4; i++) {
        if ( head == tail && head == CH_BIT[i] ) { // ตรวจสอบขั้น 2 และ 3
          Serial.println(" True with floor " + String(i + 1));
          if ((i + 1) == 1 && UP_State_F1==false ) {
            control_Auto_Floor1('U');
            FeedBack(1);
          }else if ((i + 1) == 2 && UP_State_F1==true) {
            control_Auto_Floor1('D');
            FeedBack(2);
          }else if ((i + 1) == 3) {
            control_Auto_Floor2('U');
            FeedBack(3);
          }else if ((i + 1) == 4) {
            control_Auto_Floor2('D');
            FeedBack(4);
          }

        }

      }

    }

  }

  }

*/
