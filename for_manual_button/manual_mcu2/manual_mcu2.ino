
#define sw_emer A3
String headZone;
String dataZone;
bool stateZone = false;

String setF3;
String setF4;
byte Floor_3A[6] = {34, 38, 42, 46, 50, 23};
byte Floor_3B[6] = {36, 40, 44, 48, 52, 25};
byte Floor_3PWM[6] = {5, 6, 7, 8, 9, 10};

byte Floor_4A[6] = {30, 26, 22, 27, 31, 35};
byte Floor_4B[6] = {32, 28, 24, 29, 33, 37};
byte Floor_4PWM[6] = {2, 3, 4, 11, 12, 13};

byte F3_PWM_UP[7] = {157, 142, 122, 147, 150, 147};
byte F3_PWM_DOWN[7] = {187, 152, 177, 157, 192, 120};

byte F4_PWM_UP[7] = {205, 170, 170, 177, 175, 225};
byte F4_PWM_DOWN[7] = {205, 147, 220, 195, 150, 220};

byte Limit_F3[2][2] = {{41, 39}, {45, 43}}; //-> [0][D-U]F & [1][D-U]B
byte Limit_F4[2][2] = {{49, 47} , {53, 51}}; //-> [0][D-U]F & [1][D-U]B

String CH_BIT[4] = {"1010", "1011", "1100", "1110"};
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
}

void loop() {
  for (int i = 0; i < 2; i++) {
    for (int x = 0; x < 2; x++) {
      Serial.print(digitalRead(Limit_F3[i][x]));
      Serial.print("  ");
    }
  }
  for (int i = 0; i < 2; i++) {
    for (int x = 0; x < 2; x++) {
      Serial.print(digitalRead(Limit_F4[i][x]));
      Serial.print("  ");
    }

  }
  Serial.println("  ");
  //  Serial.println(digitalRead(sw_emer));
  manual_control();
}
void manual_control() {
    Serial.println("F3 : " + setF3 + "  F4 : " + setF4);

  if (setF3 == "U") {
    control_Auto_Floor3('U');
    setF3 = "";
  } else if (setF3 == "D") {
    control_Auto_Floor3('D');
    setF3 = "";
  } if (setF4 == "U") {
    control_Auto_Floor4('U');
    setF4 = "";
  } else if (setF4 == "D") {
    control_Auto_Floor4('D');
    setF4 = "";
  }


  if (Serial3.available() > 0) {
    char r = Serial3.read();

    if (r == '[' && stateZone == false) {
      stateZone = true;
      return;
    } else if (r == ']' && stateZone == true) {
      stateZone = false;
      headZone = "";
      dataZone = "";
      return;
    }
    if (stateZone == true) {

      if (r == ',') {
        headZone = "";
        dataZone = "";
        return;
      }
      bool isMatch = false;
      if (headZone == "F3:") {
        dataZone += r;
        setF3 = dataZone;
        isMatch = true;
      }
      if (headZone == "F4:") {
        dataZone += r;
        setF4 = dataZone;
        isMatch = true;
      }
      if (isMatch == false) {
        headZone += r;
      }
    }
  }

}
void MainControl() {
  Control();
}
void FeedBack(byte num) {
  if (num > 4)return ;
  int dat[5] = {0b10101010, 0b10111011, 0b11001100, 0b11101110, 0b11111111};
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
          Serial.println(" True with floor B2 :" + String(i + 1));
          if ((i + 1) == 1 ) {
            state = "3U";
            ch_last = true;
          } else if ((i + 1) == 2) {
            state = "3D";
            ch_last = true;
          } else if ((i + 1) == 3) {
            state = "4U";
            ch_last = true;
          } else if ((i + 1) == 4) {
            state = "4D";
            ch_last = true;
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
          control_Auto_Floor3('U');
          FeedBack(1);
          state = "";
        } else if (state == "3D") {
          Serial.println("Run Floor 3 Down");
          control_Auto_Floor3('D');
          FeedBack(2);
          state = "";
        } else if (state == "4U") {
          Serial.println("Run Floor 4 Up");
          control_Auto_Floor4('U');
          FeedBack(3);
          state = "";
        } else if (state == "4D") {
          Serial.println("Run Floor 4 Down");
          control_Auto_Floor4('D');
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
      while (true)
      {


        Serial.println("Floor 3 UP");
        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 3 UP");
          StopAll();
          break;
        }
        if (digitalRead(Limit_F3[0][1]) == 0 && digitalRead(Limit_F3[1][1]) == 0)
        {
          Serial.println("have stop while");

          Drive_Floor_3('F', 'S');
          Drive_Floor_3('B', 'S');
          //          while (Serial3.available() > 0)char t = Serial3.read();
          break;
        }
        if ( digitalRead(Limit_F3[0][1]) != 0) {
          Drive_Floor_3('F', 'U');
        } else {
          Serial.println("UPFONT FOUND");
          Drive_Floor_3('F', 'S');
        }
        if ( digitalRead(Limit_F3[1][1]) != 0) {

          Drive_Floor_3('B', 'U');
        } else {
          Serial.println("UPBACK FOUND");
          Drive_Floor_3('B', 'S');
        }

      }
    }
  } else if (pick == 'D') {

    byte counter = 0;
    if (digitalRead(Limit_F3[0][0]) != 0 && digitalRead(Limit_F3[1][0]) != 0) {
      while (true) {
        //        while (Serial3.available() > 0)char t = Serial3.read();
        Serial.println("Floor 3 Down");
        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 3 DOWN");
          StopAll();
          break;
        }
        if (digitalRead(Limit_F3[0][0]) == 0 && digitalRead(Limit_F3[1][0]) == 0) {
          Drive_Floor_3('F', 'S');
          Drive_Floor_3('B', 'S');
          //          while (Serial3.available() > 0)char t = Serial3.read();
          break;

        }
        if ( digitalRead(Limit_F3[0][0]) != 0) {
          Drive_Floor_3('F', 'D');
        } else {
          Serial.println("DOWN FONT FOUND");
          Drive_Floor_3('F', 'S');
        }
        if ( digitalRead(Limit_F3[1][0]) != 0) {
          Drive_Floor_3('B', 'D');
        } else {
          Drive_Floor_3('B', 'S');
          Serial.println("DOWN BACK FOUND");
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
      while (true)
      {
        //        while (Serial3.available() > 0)char t = Serial3.read();
        Serial.println("Floor 4 UP");
        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 4 UP");
          StopAll();
          break;
        }
        if (digitalRead(Limit_F4[0][1]) == 0 && digitalRead(Limit_F4[1][1]) == 0)
        {
          Drive_Floor_4('F', 'S');
          Drive_Floor_4('B', 'S');
          //          while (Serial3.available() > 0)char t = Serial3.read();
          break;

        }
        if ( digitalRead(Limit_F4[0][1]) != 0) {
          Drive_Floor_4('F', 'U');
        } else {
          Drive_Floor_4('F', 'S');
          Serial.println("UP FONT FOUND");
        }
        if ( digitalRead(Limit_F4[1][1]) != 0) {
          Drive_Floor_4('B', 'U');
        } else {
          Drive_Floor_4('B', 'S');
           Serial.println("UP BACK FOUND");
        }


      }
    }
  } else if (pick == 'D') {
    byte counter = 0;
    if (digitalRead(Limit_F4[0][0]) != 0 && digitalRead(Limit_F4[1][0]) != 0) {
      while (true) {
        //        while (Serial3.available() > 0)char t = Serial3.read();
        Serial.println("Floor 4 Down");

        if (isStop() == true)
        {
          //Emergency
          Serial.println("EM WORK At Floor 4 Down");
          StopAll();
          break;
        }
        if (digitalRead(Limit_F4[0][0]) == 0 && digitalRead(Limit_F4[1][0]) == 0) {
          Drive_Floor_4('F', 'S');
          Drive_Floor_4('B', 'S');

          break;
        }
        if ( digitalRead(Limit_F4[0][0]) != 0) {
          Drive_Floor_4('F', 'D');
        } else {
          Drive_Floor_4('F', 'S');
           Serial.println("DOWN FONT FOUND");
        }
        if ( digitalRead(Limit_F4[1][0]) != 0) {
          Drive_Floor_4('B', 'D');
        } else {
           Serial.println("DOWN BACK FOUND");
          Drive_Floor_4('B', 'S');
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
  pinMode(sw_emer, INPUT_PULLUP);
}
void StopAll() {
  for (int i = 0; i < 6; i++) {
    drive_floor3( (i + 1) , 'S');
    drive_floor4( (i + 1) , 'S');
  }
  //  while (Serial3.available() > 0)char t = Serial3.read();
}

bool isStop() {
  //  return digitalRead(sw_emer) == 1; //USE THIS
  return digitalRead(sw_emer) == 0; //JUST TESE 1/5/2565
}

void Drive_Floor_3(char sel_LR_ch , char selUD ) {
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
void Drive_Floor_4(char sel_LR_ch , char selUD ) {
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
