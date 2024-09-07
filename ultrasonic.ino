const int trig = D0;
const int echo = D1;

#define VEL 0.034
#define cm2inch 0.393701

long duration;
float distanceCm;
float distanceInch;

void setup()
{
  Serial.begin(9600);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
}

void loop()
{
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);

  duration = pulseIn(echo,HIGH);
  distanceCm = duration*VEL/2;
  distanceInch = distanceCm * cm2inch;
  Serial.println("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.println("Distance (Inch): ");
  Serial.println(distanceInch);
  delay(2000);
}
