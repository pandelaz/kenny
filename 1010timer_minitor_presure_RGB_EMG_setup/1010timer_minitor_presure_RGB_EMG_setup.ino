//-----------------------宣告及定義------------------------------
//*********************時間模組Start************************
#include <DS3231.h>  //外掛 時間模組 檔名: DS3231
DS3231 rtc(SDA , SCL);   //arduino (20SDA,21SCL)  時間模組
//*********************時間模組End************************
//*********************顯示器Start************************
#include <Wire.h>  // Arduino IDE 內建
#include <LiquidCrystal_I2C.h>//外掛 顯示器模組 檔名: NewliquidCrystal_1.3.4
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址
//*********************顯示器End**************************
//*********************壓力感測&RGB燈號 Start************************
#define redpin  5//設定紅燈的PIN 2
#define bluepin  6//設定藍燈的PIN 3
#define greenpin  4//設定綠燈的PIN 4
#define fsr_pin A0// 壓力感測器的PIN A0
//*********************壓力感測&RGB燈號 End************************
//*********************資料讀寫入 Start************************
#include <SPI.h>// Arduino  內建
#include <SD.h>// Arduino  內建
#define data_file  53//設定資料啟動的PIN 53 (MOSI,MISO,CLK,CS)arduinopin(51,50,52,53)
File myFile; //建立宣告檔案
boolean sd_set = false;
//*********************資料讀寫入 End************************
//*********************EMG Start************************
#define emg_value A6 //EMG腳位設定 PINA6
//*********************EMG  End************************
//*********************按壓次數&滿足時間 Start************************
bool hightrue = false;
bool lowtrue = false;
int timer_open ;
unsigned long startTime;//按壓開始時間
unsigned long duration;//按壓持續時間
int hold_time=5000;//按壓的持續時間
//*********************按壓次數&滿足時間  End************************
//*********************額外設定 Start************************
int delaytime = 100;//延遲時間
#define shutdown_button  4 //停止按鈕
String ID = ""; //使用者的編號  (可更改)
int num, pisok, emgisok, finalok; //按壓次數,壓力判斷,EMG判斷,是否滿足壓力跟EMG
//*********************額外設定 End************************

//wifi
String ssid = "";
String password = "";
String server = "api.thingspeak.com";
String uri = "/update";
String apikey = "0JFBHJP721DRYBHX";
String data = apikey + "&field1=73";

//new var
int mode = 2;
String title_name[] = {"ID","Date","time","num","pressure","pisok","emg","emgisok","finalok"};
String tests="";

//**************************************************** setup*********************************************************************************************
void setup() {

  // put your setup code here, to run once:
  // Setup Serial connection
  Serial.begin(9600);
  Serial3.begin(115200);
  // Uncomment the next line if you are using an Arduino Leonardo
  while (!Serial) {}
  
  //*********************額外設定 Start************************
  pinMode(shutdown_button, OUTPUT);   //停止按鈕
  //*********************額外設定 End************************
  //*********************顯示器Start************************
  //啟動時會閃爍
  lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.backlight(); // 開啟背光
  delay(250);
  lcd.noBacklight(); // 關閉背光
  delay(250);
  lcd.backlight();
  //*********************顯示器End**************************
  //*********************時間模組Start************************
  // Initialize the rtc object
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  // rtc.setDOW(TUESDAY);     // Set Day-of-Week to SUNDAY
  // rtc.setTime(0 , 19 , 0);     // Set the time to 12:00:00 (24hr format)
  // rtc.setDate(27 , 9 , 2017);   // Set the date to January 1st, 2014 (date,mon,year)
  // 顯示的實驗的時間-------------------------------------
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  // Send time
  Serial.println(rtc.getTimeStr());
  // 顯示的實驗的時間-------------------------------------
  // 顯示的實驗的時間-------------------------------------
  //lcd.clear();//清除LCD畫面
  lcd.setCursor(0, 0); //將游標移至第一行第一列
  //lcd.print(rtc.getDOWStr()); //禮拜 MONDAY TUESDAY WEDNESDAY THURDAY FRIDAY SATURDAY SUNDAY
  lcd.print(rtc.getDateStr());//
  lcd.setCursor(0, 1); //將游標移至第一行第二列
  lcd.print("time:");
  lcd.print(rtc.getTimeStr());
  delay(3000);
  lcd.clear();
  // 顯示的實驗的時間-------------------------------------
  //*********************時間模組End************************

  //*********************壓力感測&RGB燈號 Start************************
  pinMode(redpin, OUTPUT); //LED腳位為輸出
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  //*********************壓力感測&RGB燈號 End************************
  //*********************資料讀寫入 Start************************
  pinMode(data_file, OUTPUT);//資料寫入讀取腳位為輸出
  lcd.setCursor(0, 0);
  lcd.println("Set up SD card...");//正在讀取記憶卡
  Serial.println("Set up SD card...");

  if (!SD.begin(data_file)) {
    Serial.println("failed!");
    lcd.setCursor(0, 1);
    lcd.println("Failed!");//面板顯示fail 表示記憶卡有問題
    return;
  }
  Serial.println("Done.");
  lcd.setCursor(0, 1);
  lcd.println("Done.");//面板顯示Done 表示讀取到記憶卡
  delay(1000);
  sd_set = true;
  lcd.clear();
  //*********************資料讀寫入 End************************




  
  String tmp_string[12];
  int cnt = 0;
  //Serial.println("file 1 read");
  //file1 檔讀取
  if (sd_set)
  {
    myFile = SD.open("info.txt");       // 開啟檔案，一次僅能開啟一個檔案
    if (myFile)
    {
      //while (myFile.available()) {
      for(cnt=0;cnt<12;cnt++) {
        tmp_string[cnt] = myFile.readStringUntil('\r');
        //Serial.println(cnt);
      }
    }
    //Serial.println("close");
    myFile.close();
  }

  ID = tmp_string[1].substring(1);
  hold_time = tmp_string[9].toInt()*1000;
  ssid = tmp_string[3].substring(1);
  password = tmp_string[5].substring(1);
  Serial.println("test");
  Serial.println(ID);
  Serial.println(password);
  
  delay(1000);

  //wifi
  reset();
  connectWifi();
  /*
  //*********************資料寫入 Start************************
  //file2 檔寫入
  if (sd_set)
  {
    if (!SD.exists("data.txt")) {
      myFile = SD.open("data.txt", FILE_WRITE);       // 開啟檔案，一次僅能開啟一個檔案
      if (myFile)
      {
        //#ID,data time,num,pressure,pisok(1=ok;0=faile),emg,emgisok(1=ok;0=faile),finalok(1=ok;0=faile)
        myFile.print("#ID"); myFile.print(","); //ID
        //myFile.print(rtc.getDOWStr());myFile.print(",");//寫入星期幾
        myFile.print("Date"); myFile.print(","); //寫入日期
        myFile.print("Time"); myFile.print(","); //寫入時間
        myFile.print("num"); myFile.print(",");//按壓次數
        myFile.print("pressure"); myFile.print(","); //壓力值
        myFile.print("pisok(1=ok;0=faile)"); myFile.print(","); //壓力是否滿足條件
        myFile.print("emg"); myFile.print(","); //肌肉訊號值
        myFile.print("emgisok(1=ok;0=faile)"); myFile.print(",");  //肌肉訊號是否滿足條件
        myFile.print("finalok(1=ok;0=faile)"); myFile.println(); //以上兩點是否滿足條件
      }
      myFile.close();
    }
  }*/
  //*********************資料寫入 End************************


}
//*********************************************************************************** LOOP**************************************************************************************
void loop() {
  while (!sd_set) {
    return;
  }

  if(Serial.available()) {
    mode = Serial.read()-48;
    Serial.println(mode);
  }
  
  if(mode==0)
    mloop();
  else if(mode==1)
    httppost();
  else if(mode == 2)
    test();
  else if(mode == 3) {
    SD.remove("data.txt");
    mode = 4;
  }

}

//-------------------宣告程式--------------------

void mloop() {
  //*********************時間模組Start************************
  // put your main code here, to run repeatedly:
  // Send Day-of-Week
  //  Serial.print(rtc.getDOWStr());
  //  Serial.print(" ");
  // Send date
  //  Serial.print(rtc.getDateStr());
  //  Serial.print(" -- ");
  // Send time
  //Serial.print(rtc.getTimeStr());
  // Wait one second before repeating :)
  //*********************時間模組End************************
  //*********************顯示器Start************************
  //lcd.clear();//清除LCD畫面
  lcd.setCursor(0, 0); //將游標移至第一行第一列
  //lcd.print(rtc.getDOWStr());
  lcd.print(rtc.getDateStr());
  lcd.print("   num");
  lcd.setCursor(0, 1); //將游標移至第一行第二列
  lcd.print("time:");
  lcd.print(rtc.getTimeStr());
  lcd.print(" ");
  lcd.print(num);//顯示按壓次數
  //*********************顯示器End**************************
  //*********************壓力感測&RGB燈號 Start************************
  int fsr_value = analogRead(fsr_pin); //讀取 FSR 壓力感測器值
  int led_value = map(fsr_value, 0, 1023, 0, 255); //從FSR壓力值映射到LED燈0~255值
  int red_value, green_value, blue_value;
  if (led_value > 124)
  {
    red_value = 0;
    blue_value = 0;
    green_value = 255;
    pisok = 1; //表示壓力值OK
  }
  else if (10 < led_value && led_value <= 124)
  {
    red_value = 0;
    blue_value = 255;
    green_value = 0;
    pisok = 0; //表示壓力值faile
  }
  else if (led_value <= 10)
  {
    red_value = 255;
    blue_value = 0;
    green_value = 0;
    pisok = 0; //表示壓力值faile
  }
  analogWrite(redpin, red_value);
  analogWrite(bluepin, blue_value);
  analogWrite(greenpin, green_value);

  //*********************壓力感測&RGB燈號 End************************
  //*********************按壓次數  Start************************
  if (led_value <= 10)
  {
    hightrue = false;
    lowtrue = true;
    timer_open = 0;//啟動計時條件
  }
  if (led_value > 124 && timer_open == 0)//當燈號值及啟動計時滿足則記錄當下的時間
  {
    startTime = millis();
    timer_open = 1;
    hightrue = true;
  }
  if (hightrue == true )
  {    
    duration = millis() - startTime;
    if (( lowtrue == true) && (duration >= hold_time)) //持續滿足時間則加一次按壓次數
    {
      num=num+1;
      lowtrue = false;
    }    
  }

  //*********************按壓次數 End************************
  //*********************EMG Start************************
  // read the input on analog pin 0:
  int sensorValue = analogRead(emg_value);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //float voltage = sensorValue * (5.0 / 1023.0);
  float voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  if (voltage > 0.6)//EMG判斷是否OK
  {
    emgisok = 1;
  }
  else
  {
    emgisok = 0;
  }

  if (emgisok == 1 && pisok == 1) //FINAL判斷是否OK
  {
    finalok = 1;
  }
  else
  {
    finalok = 0;
  }

  //*********************EMG  End************************
  //*********************資料寫入 Start************************
  myFile = SD.open("data.txt", FILE_WRITE);       // 開啟檔案，一次僅能開啟一個檔案
  if (myFile)
  {
    //#ID,data time,num,pressure,pisok(1=ok;0=faile),emg,emgisok(1=ok;0=faile),finalok(1=ok;0=faile)
    myFile.print(",{");
    myFile.print(title_name[0]); myFile.print(":"); myFile.print(ID); myFile.print(","); //ID寫入
    myFile.print(title_name[1]); myFile.print(":"); myFile.print(rtc.getDateStr()); myFile.print(",");//寫入日期
    myFile.print(title_name[2]); myFile.print(":"); myFile.print(rtc.getTimeStr()); myFile.print(",");//寫入時間
    myFile.print(title_name[3]); myFile.print(":"); myFile.print(num); myFile.print(",");//按壓次數
    myFile.print(title_name[4]); myFile.print(":"); myFile.print(fsr_value); myFile.print(","); //壓力值
    myFile.print(title_name[5]); myFile.print(":"); myFile.print(pisok); myFile.print(","); //壓力pisok(1=ok;0=faile)
    myFile.print(title_name[6]); myFile.print(":"); myFile.print(voltage); myFile.print(","); //EMG值
    myFile.print(title_name[7]); myFile.print(":"); myFile.print(emgisok); myFile.print(","); //EMG值emgisok(1=ok;0=faile)
    myFile.print(title_name[8]); myFile.print(":"); myFile.print(finalok); //final OK 壓力值跟EMG值
    myFile.print("}");
  }
  
  Serial.print(ID); Serial.print(","); //ID寫入
  Serial.print(rtc.getDateStr()); Serial.print(" ");  //寫入日期
  Serial.print(rtc.getTimeStr()); Serial.print(",");//寫入時間
  Serial.print(num); Serial.print(",");//按壓次數
  Serial.print(fsr_value); Serial.print(","); //壓力值
  Serial.print(pisok); Serial.print(","); //壓力pisok(1=ok;0=faile)
  Serial.print(voltage); Serial.print(","); //EMG值
  Serial.print(emgisok); Serial.print(","); //EMG值emgisok(1=ok;0=faile)
  Serial.print(finalok); //final OK 壓力值跟EMG值
  Serial.println();
  
  //*********************資料寫入 End************************


  //*********************停止按鈕  End************************
  //  if (digitalRead(shutdown_button) == HIGH)
  //  {
  //    delay(500);
  //    return;
  //  }
  //*********************停止按鈕  End************************

  myFile.close();  
}

void reset() {
  Serial3.println("AT+RST");
  delay(1000);
  if(Serial3.find("OK") ) Serial.println("Module Reset");
}

//connect to your wifi network
void connectWifi() {
  String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  Serial3.println(cmd);
  delay(4000);
  if(Serial3.find("OK")) {
    Serial.println("Connected!");
  } else {
    connectWifi();
    Serial.println("Cannot connect to wifi"); 
  }
}
void httppost () {
  
  Serial3.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.
  
  if( Serial3.find("OK")) {
    Serial.println("TCP connection ready");
  } 
  
  delay(1000);
  
  String postRequest =
  
  "POST " + uri + " HTTP/1.1\r\n" +
  "Host: " + server + "\r\n" +
  "Connection: close\r\n" + 
  "X-THINGSPEAKAPIKEY: 0JFBHJP721DRYBHX\r\n" + 
  "Content-Type: application/x-www-form-urlencoded\r\n" +
  "Content-Length: " + data.length() + "\r\n" +
  "\r\n" + data;
  
  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  
  Serial3.print(sendCmd);
  Serial3.println(postRequest.length() );
  delay(1000);
  
  if(Serial3.find(">")) { 
    Serial.println("Sending.."); 
    Serial3.print(postRequest);
    
    if( Serial3.find("SEND OK")) { 
      
      Serial.println("Packet sent");
      
      while (Serial3.available()) {
        String tmpResp = Serial3.readString();
        Serial.println(tmpResp);
      }
      
      // close the connection
      Serial3.println("AT+CIPCLOSE");
    }
  }
}

void test() {

  //Serial.println("file 1 read");
  //file2 檔讀取
  //String tests="";
  char testc;
  if (sd_set)
  {
    myFile = SD.open("data.txt");       // 開啟檔案，一次僅能開啟一個檔案
    if (myFile)
    {
      while (myFile.available()) {
         testc = myFile.read();
         //Serial.write(testc);
         
         tests += testc;
      }
    }

    //String testarr[] = tests.split
    tests = "{" + tests.substring(1) + "}";
    Serial.println(tests);
    myFile.close();
  }
  delay(3000);
}

