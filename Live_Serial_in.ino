/*Author :Sooraj Tom
   Date:15-07-2016
   takes serial input commands and prints
*/
int M2A1 = 2, M2A2 = 3, M2B1 = 4, M2B2 = 5, M1A1 = A5, M1A2 = A4, M1B1 = A3, M1B2 = A2, M1lim = 600, M2lim = 1500;
int p_trig = 6, p_reset = 12, pendelay = 200, p_angle = 0;
class Motor
{
  public:
    int stepDelay, stepstat, curpos, edgeuplim, mindex;
    int steppermm;
    int inA1 ; // input 1 of the stepper
    int inA2 ; // input 2 of the stepper
    int inB1 ; // input 3 of the stepper
    int inB2 ; // input 4 of the stepper

    Motor(int, int, int, int, int, int, int, int, int, int);
    void stepit(int lmm);
    void stopMotor();
};
Motor::Motor(int sd, int sm, int cp, int el, int a1, int a2, int b1, int b2, int mi, int mo = 1)
{
  stepDelay = sd;
  steppermm = sm;
  curpos = cp;
  edgeuplim = el;
  inA1 = a1;
  inA2 = a2;
  inB1 = b1;
  inB2 = b2;
  mindex = mi;
  stepstat = 0;
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, HIGH);
  digitalWrite(inB2, LOW);
  delayMicroseconds(stepDelay);
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, LOW);
}
void Motor::stepit(int lmm)
{
  if (lmm == 0)return;
  int steps = lmm * steppermm, turndir = (lmm / abs(lmm));
  digitalWrite(13, HIGH);
  curpos = curpos + lmm;
  while (steps != 0)
  {
    stepstat = stepstat + turndir;
    if(stepstat == -1)stepstat = 3;
    else if(stepstat == 4)stepstat = 0;
    
    switch (stepstat)
    {
      case 0:
        digitalWrite(inA1, HIGH);
        digitalWrite(inA2, LOW);
        digitalWrite(inB1, HIGH);
        digitalWrite(inB2, LOW);
        break;
      case 1:
        digitalWrite(inA1, LOW);
        digitalWrite(inA2, HIGH);
        digitalWrite(inB1, HIGH);
        digitalWrite(inB2, LOW);
        break;
      case 2:
        digitalWrite(inA1, LOW);
        digitalWrite(inA2, HIGH);
        digitalWrite(inB1, LOW);
        digitalWrite(inB2, HIGH);
        break;
      case 3:
        digitalWrite(inA1, HIGH);
        digitalWrite(inA2, LOW);
        digitalWrite(inB1, LOW);
        digitalWrite(inB2, HIGH);
        break;
    }
    steps = steps - turndir;
    delayMicroseconds(stepDelay);
  }
  digitalWrite(13, LOW);
  //stopMotor();
}
Motor Carr(2400, 5, 0, M1lim, M1A1, M1A2, M1B1, M1B2, 0); //4.716
Motor Roll(3000, 28, 0, M2lim, M2A1, M2A2, M2B1, M2B2, 1); //28.169
//Motor(int sd,int sm,int cp,int el,int a1,int a2,int b1,int b2)
void penup()
{
  digitalWrite(p_trig, HIGH);
  digitalWrite(p_reset, LOW);
  delay(pendelay);
}
void pendown()
{
  digitalWrite(p_trig, LOW);
  digitalWrite(p_reset, HIGH);
  delay(pendelay);
}
void posreport()
{
  Serial.print("x:");
  Serial.print(Carr.curpos);
  Serial.print(" ");
  Serial.print("y:");
  Serial.println(Roll.curpos);
}
void setup() {
  Serial.begin(9600);
  pinMode(M1A1, OUTPUT);
  pinMode(M1A2, OUTPUT);
  pinMode(M1B1, OUTPUT);
  pinMode(M1B2, OUTPUT);
  pinMode(M2A1, OUTPUT);
  pinMode(M2A2, OUTPUT);
  pinMode(M2B1, OUTPUT);
  pinMode(M2B2, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(p_trig, OUTPUT);
  pinMode(p_reset, OUTPUT);
  penup();
}
void Motor::stopMotor() {
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, LOW);
}
byte incom = NULL;
// the loop routine runs over and over again forever:
void loop() {
  int len = 0;
  int i = 0;
  char command[6];
  while (1)
  {
    if (Serial.available())
    {
      incom = Serial.read();
      if (incom == 59)
      {
        command[i] = '\0';
        break;
      }
      else
      {
        command[i] = (char)incom;
        i++;
      }
    }
  }
  incom = NULL;
  for (int j = i - 1; j > 0; j--)
  {
    len = len + (command[j] - '0') * pow(10, (i - 1) - j); //extract number
  }
  switch (command[0])
  {
    case 'u': Roll.stepit(len); break;
    case 'd': Roll.stepit(-1 * len); break;
    case 'l': Carr.stepit(len); break;
    case 'r': Carr.stepit(-1 * len); break;
    case '1': pendown(); Serial.println("Pendown");; break;
    case '0': penup(); Serial.println("Penup"); break;
    case 'e': Roll.stepit(100);
    default: Serial.println("Command error"); break;
  }
  Carr.stopMotor();
  Roll.stopMotor();
  posreport();
}
