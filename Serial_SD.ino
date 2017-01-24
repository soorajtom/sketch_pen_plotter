#include<Stepper.h>
#include<SPI.h>
#include<SD.h>
#define linegap 0.6
#define linelength 1.0
#define scale 1
#define LINE_MAX 650

const short M2A1 = 2, M2A2 = 3, M2B1 = 4, M2B2 = 5, M1A1 = A5, M1A2 = A4, M1B1 = A3, M1B2 = A2, M1lim = 1600, M2lim = 15000, ir = scale;
const short p_enable = 7, pendelay = 100, finishled = 8, errled = A0, led1 = 6, led2 = A1;
short penstat;
Stepper myStepper[2] = {Stepper(M1lim, M1A1, M1A2, M1B1, M1B2), Stepper(M2lim, M2A1, M2A2, M2B1, M2B2)};
class Motor
{

  public:
    int stepDelay, stepstat, curpos, edgeuplim, mindex;
    float steppermm;
    int inA1 ; // input 1 of the stepper
    int inA2 ; // input 2 of the stepper
    int inB1 ; // input 3 of the stepper
    int inB2 ; // input 4 of the stepper

    Motor(int, float, int, int, int, int, int, int, int, int);
    void stepit(float lmm);
    void stopMotor();
};
Motor::Motor(int sd, float sm, int cp, int el, int a1, int a2, int b1, int b2, int mi, int mo = 1)
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
  stepstat = 4;
}
void Motor::stepit(float lmm)
{
  if (lmm == 0)return;
  int steps = lmm * steppermm, turndir = (lmm / abs(lmm));
  while (steps != 0)
  {
    if (curpos > 800 && mindex == 0)exit(5);
    curpos = curpos + turndir;
    steps = steps - turndir;
    myStepper[mindex].step(turndir);
    delayMicroseconds(stepDelay);
  }
  if (mindex == 1) {
    delay(100);
    stopMotor();
  }
}
Motor Carr(2400, scale, 0, M1lim, M1A1, M1A2, M1B1, M1B2, 0); //4.716 
Motor Roll(3000, 10, 0, M2lim, M2A1, M2A2, M2B1, M2B2, 1); //28.169
void penup()
{
  if (!penstat)return;
  digitalWrite(p_enable, LOW);
  penstat = 0;
  delay(pendelay);
}
void pendown()
{
  if (penstat)return;
  digitalWrite(p_enable, HIGH);
  penstat = 1;
  delay(pendelay);
}
void gotopos(int required_pos)
{
  int current = ((int)(Carr.curpos / Carr.steppermm)) * linelength;
  Carr.stepit((required_pos - current)*linelength);
}
void posreport()
{
  Serial.print("x:");
  Serial.print((int)(Carr.curpos / Carr.steppermm));
  Serial.print(" ");
  Serial.print("y:");
  Serial.println((int)(Roll.curpos / Roll.steppermm));
}
void setup() {
  pinMode(M1A1, OUTPUT);
  pinMode(M1A2, OUTPUT);
  pinMode(M1B1, OUTPUT);
  pinMode(M1B2, OUTPUT);
  pinMode(M2A1, OUTPUT);
  pinMode(M2A2, OUTPUT);
  pinMode(M2B1, OUTPUT);
  pinMode(M2B2, OUTPUT);
  pinMode(finishled, OUTPUT);
  pinMode(errled, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(p_enable, OUTPUT);
  penup();

  digitalWrite(led1, HIGH);
}
void Motor::stopMotor() {
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, LOW);
}
// the loop routine runs over and over again forever:
void loop()
{
  File imgfile;
  short i, j, linebegin = 0, lineend, breadth = 0;
  if (!SD.begin(10)) {
    digitalWrite(errled, HIGH);
    Serial.println("initialization failed!");
    exit(1);
  }

  Serial.begin(19200);
  byte incom = '\0';
  char command[LINE_MAX];
  char ch;
  if (SD.exists("image.txt"))
  {
    SD.remove("image.txt");
  }

  imgfile = SD.open("image.txt", FILE_WRITE);
  if (!imgfile)
  {
    digitalWrite(errled, HIGH);
    Serial.println("File opening failed_1");
    exit(1);
  }

  while (!Serial.available());
  while ( (incom = Serial.read()) != ';' )
  {
    if (incom == '0')
      imgfile.print('0');
    else if (incom == '1')
      imgfile.print('1');
    else if (incom == ':')
    {
      imgfile.print(':');
      imgfile.flush();
    }
    while (!Serial.available());
  }
  imgfile.print(';');

  imgfile.close();
  imgfile = SD.open("image.txt", FILE_READ);
  if (!imgfile)
  {
    digitalWrite(errled, HIGH);
    Serial.println("File opening failed_2");
    exit(1);
  }
  while(imgfile.read() != ':')
    breadth++;
  while (imgfile.peek() != ';')
  {
    //Serial.println("line read as:");
    for (i = 0; (ch = imgfile.read()) != ':'; i++)
    {
      command[i] = ch;
    }
    command[i] = '\0';
    //Serial.println(command);
    if(breadth != i)
    {
      digitalWrite(errled, HIGH);
      continue;
    }
    for (linebegin = 0, j = 0; j < i; j++)
    {
      if (command[j] == '1')
      {
        linebegin = j;
        break;
      }
    }

    for (lineend = i, j = i - 1; j >= 0; j--)
    {
      if (command[j] == '1')
      {
        break;
      }
      lineend--;
    }
    if (linebegin > lineend)
    {
      Roll.stepit(ir * linegap);
    }
    else
    {
      for (int t = 0; t < ir; t++)
      {
        gotopos(linebegin);
        for (j = linebegin; j <= lineend; j++) //line spe
        {
          if (penstat == command[j] - '0');
          else if (command[j] == '1') {
            pendown();
          }
          else if (command[j] == '0') {
            penup();
          }
          Carr.stepit(linelength);
        }
        penup();
        Roll.stepit(linegap);
        delay(10);
      }
    }
    Carr.stopMotor();
  }
  Roll.stepit(100);
  digitalWrite(finishled, HIGH);
  while (1);
}

