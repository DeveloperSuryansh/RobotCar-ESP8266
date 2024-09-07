#include <ESP8266WiFi.h>

#define COMS 15
#define TRIG D4
#define ECHO D5
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

#define STOPDIST 40

const char *essid = "ACCESS-DENIED";
const char *password = "suryahotspot";
const char *essid2 = "FTTH-TYPE4";
const char *pass2 = "Sanjaydgm@1234";

WiFiServer server(80);
WiFiClient client;

String command[COMS] = {"gpioout","gpioin","hello","forward","backward","left","right","stop","distance","play","stopplay"};
int pins[10] = {D0,D1,D2,D3,D4,D5,D6,D7,D8,LED_BUILTIN};
String user;
int count=0;

int getDistance()
{
  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
  digitalWrite(TRIG,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG,LOW);
  int duration = pulseIn(ECHO, HIGH);
  int dist = duration *  SOUND_VELOCITY/2;
  return dist;
}

void forward()
{
  digitalWrite(D1,1);
  digitalWrite(D3,1);
}

void backward()
{
  digitalWrite(D0,1);
  digitalWrite(D2,1);
}

void left()
{
  digitalWrite(D1,1);
  digitalWrite(D2,1);
}

void right()
{
  digitalWrite(D0,1);
  digitalWrite(D3,1);
}

void stop()
{
  digitalWrite(D0,0);
  digitalWrite(D1,0);
  digitalWrite(D2,0);
  digitalWrite(D3,0);
}

void stopplay()
{
  count = 0;
}

int rcount = 0,lcount = 0;

void play()
{
  if(getDistance()<STOPDIST)
  {
    delay(200);

    if(rcount<4)
    {
      while(getDistance()<STOPDIST)
      {
        right();
        delay(50);
        stop();
        rcount++;
      }

      delay(200);
    }

    else if(lcount<4)
    {
      while(getDistance()<STOPDIST)
      {
        left();
        delay(50);
        stop();
        lcount++;
      }

      delay(200);
    }
    else
    {
      while(getDistance()<150)
      {
        backward();
        delay(100);
        stop();
        rcount = 0;
        lcount = 0;
      }

      delay(200);
    }
    
  }

  while(getDistance()>STOPDIST)
  {
    rcount = 0;
    lcount = 0;
    forward();
    delay(int(getDistance()*1.5));
    stop();
  }
}

void action(String user)
{
  for(int i = 0;i<COMS;i++)
    {
      int comsize = command[i].length();
      // if command = gpioout
      if(user.substring(0,comsize)==command[0])
      {
        int pin = user[comsize,comsize+1] - '0';
        int mode = user[comsize+1,comsize+2] - '0';
        pinMode(pins[pin],OUTPUT);
        digitalWrite(pins[pin],mode);
        Serial.write("GpioOUT: ");
        Serial.printf("%d => %d\n\n",pin,mode);
      }

      // if command = gpioin
      if(user.substring(0,comsize)==command[1])
      {
        int pin = user[comsize,comsize+1] - '0';
        int result = digitalRead(pins[pin]);
        pinMode(pins[pin],INPUT);
        Serial.write("GpioIN: ");
        Serial.printf("%d => %d\n\n",pin,result);
      }

      if(user.substring(0,comsize)==command[2])
      {
        Serial.write("Hello There, How are you?\n");
      }

      if(user.substring(0,comsize)==command[3])
      {
        Serial.println("Moving Forward");
        forward();
      }

      if(user.substring(0,comsize)==command[4])
      {
        Serial.println("Moving Backward");
        backward();
      }

      if(user.substring(0,comsize)==command[5])
      {
        Serial.println("Moving Left");
        left();
      }

      if(user.substring(0,comsize)==command[6])
      {
        Serial.println("Moving Right");
        right();
      }

      if(user.substring(0,comsize)==command[7])
      {
        Serial.println("Moving Stopped");
        stop();
      }

      if(user.substring(0,comsize)==command[8])
      {
        int c;
        c = getDistance();
        Serial.print("Distance in CM: => ");
        Serial.println(c);

        client.println(c);
  
      }

      if(user.substring(0,comsize)==command[9])
      {
        count = 5000;
        play();
      }
      if(user.substring(0,comsize)==command[10])
      {
        stopplay();
      }
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(essid,password);

  for(int i = 0; i<10;i++)
  {
    pinMode(pins[i],OUTPUT);
  }
  delay(10000);
  if(WiFi.status()!=WL_CONNECTED)
  {
    WiFi.begin(essid2,pass2);
  }
  delay(10000);
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    user="";
    user = Serial.readString();
    action(user);
    count=0;
  }

  client = server.available();
  if (client) {
    while (client.connected()) {
      user="";
      while (client.available() > 0) {
        user += char(client.read());
        delay(10);
      }
      if(user!="")
      {
        action(user);
        count = 0;
      }
    }
    client.stop();
  }
  count++;
  Serial.println(count);
  if(count>2000)
  {
    Serial.println("Robot is Playing");
    play();
  }
}