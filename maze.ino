//motor
#define left_motor_positive 2
#define left_motor_negative 3
#define right_motor_positive 4
#define right_motor_negative 5
//speed
#define en1 10
#define en2 11 
#define button 13
int reading;
//sensor
#define far_left_sensor 6
#define near_left_sensor 7
#define centre 8
#define near_right_sensor 12
#define far_right_sensor 9

#define led A5

int i=0;
int j=0;

int b=0;
int w=1;
int error;
char mode;
int Status = 0;
int ir[5] = {0, 0, 0, 0, 0};

int kp =42; 
int ki = 0; 
int kd = 0; 
int pid_value;
int previouserror = 0;

int initial_motor_speed =100;
int rotating_speed = 67;
int forward_speed = 80;
int right_motor_speed = 0; //for the speed after PID control
int left_motor_speed = 0;
int move_certain =15;




void dryrun();
void readirsensor();
void motion();
void turnleft();
void turnright();
void forward(int,int);
void backward(int,int);
void moveextra();
void moveinch();
void endmaze();
void calculatepid();
void pidmotor();
void stopmotor();
void left(int);
void right(int);
void  sharpturn();
void move_extra();
void recIntersection(char);
char path[100] = "";
unsigned char pathLength = 0; // the length of the path
int pathIndex = 0;
void setmotionactual();
void mazeTurn (char dir);
void loopreducing();
void finalreducing();
void manage();
void manageR();

void setup() {
  Serial.begin(9600);
  pinMode(left_motor_positive,OUTPUT);
  pinMode(left_motor_negative,OUTPUT);
  pinMode(right_motor_positive,OUTPUT);
  pinMode(right_motor_negative,OUTPUT);
  pinMode(en1,OUTPUT);
  pinMode(en2,OUTPUT);
  pinMode(far_left_sensor,INPUT);
  pinMode(near_left_sensor,INPUT);
  pinMode(centre,INPUT);
  pinMode(near_right_sensor,INPUT);
  pinMode(far_right_sensor,INPUT);
  pinMode(button,INPUT);
  pinMode(led,OUTPUT);

}

void loop() {
 reading=digitalRead(button);
 if(reading==1 && Status==0)
    {
      dryrun();
    }   
 else if(reading==1 && Status==1)
 {
  
  actualrun();
  Status = 1;
    pathIndex = 0;
 }
 }

 void dryrun(){
  while(Status==0)
  {
  readirsensor();
  motion();
  }
 }



 void readirsensor(){
  ir[0]=digitalRead(far_left_sensor);
  ir[1]=digitalRead(near_left_sensor);
  ir[2]=digitalRead(centre);
  ir[3]=digitalRead(near_right_sensor);
  ir[4]=digitalRead(far_right_sensor);
  if(ir[0]==b && ir[1]==b && ir[2]==b && ir[3]==b && ir[4]==b)
  {
    mode='N';
    error=0;
  }
  else if(ir[0]==w && ir[1]==w &&ir[2]==w&& ir[3]==w && ir[4]==w)
  {
   mode='S';
   error=0;
  }
  else if(ir[0]==w && ir[1]==w && ir[2]==w && ir[3]==b && ir[4]==b)
  {
    mode='L';
    error=0;
  }
  else if(ir[0]==w && ir[1]==w && ir[2]==w && ir[3]==w && ir[4]==b)
  {
    mode='L';
    error=0;
  }
 
  else if(ir[0]==b && ir[1]==b && ir[2]==w && ir[3]==w && ir[4]==w)
  {
   
    mode='R';
    error=0;
  }
  else if(ir[0]==b && ir[1]==w && ir[2]==w && ir[3]==w && ir[4]==w)
  {
     
    mode='R';
    error=0;
  }
  else if(ir[0]== b && ir[1]== b && ir[2]== b && ir[3]== b && ir[4]== w )
  {
    mode ='F';
    error = 4;
  }
  else if(ir[0]== b && ir[1]== b && ir[2]== b && ir[3]== w && ir[4]== w )
  {
    mode ='F';
    error = 3;
  }
  else if(ir[0]== b && ir[1]== b && ir[2]== b && ir[3]== w  && ir[4]== b)
  {
    mode ='F';
    error = 2;
    }
  else if(ir[0]== b && ir[1]== b && ir[2]== w && ir[3]== w && ir[4]== b )
  {
    mode ='F';
    error = 1;
    }
  else if(ir[0]== b && ir[1]== b && ir[2]== w && ir[3]== b && ir[4]== b)
  {
    mode ='F';
    error = 0;
    }
  else if(ir[0]== b && ir[1]== w && ir[2]== w && ir[3]== b && ir[4]== b )
  {
    mode ='F';
    error =- 1;
    }
  else if(ir[0]== b && ir[1]== w && ir[2]== b && ir[3]== b && ir[4]== b)
  {
    mode ='F';
    error = -2;
    }
  else if(ir[0]== w && ir[1]== w && ir[2]== b && ir[3]== b && ir[4]== b)
  {
    mode ='F';
    
    error = -3;
    }
  else if(ir[0]== w && ir[1]== b && ir[2]== b && ir[3]== b && ir[4]== b )
  {
    mode ='F';
    error = -4;
    }

  
 }
 void motion()
 {
  readirsensor();
  switch(mode)
    {
      case 'N':
      stopmotor();
            sharpturn();
            recIntersection('B');
          break;
      case 'S':
      stopmotor();
      moveinch();
          readirsensor();
          if (mode == 'S')
        {
          endmaze();
        }
      else if(mode=='F')
      {
         turnleft();
         recIntersection('L');
      }
      break;
    case 'R':
    stopmotor();
    moveinch();
        readirsensor();
        if (mode == 'N')
         {
           turnright();
           recIntersection('R');
         }
         else
      {
        recIntersection('S');
      }

      break;
      case 'L':
       turnleft();
       recIntersection('L');
      break;
      case 'F':
      calculatepid();
      pidmotor();
      break;
    }
 }
 
void forward(int spd1, int spd2)
{

  analogWrite(en1, spd1);
  analogWrite(en2, spd2);
  digitalWrite(left_motor_positive, HIGH);
  digitalWrite(left_motor_negative, LOW);
  digitalWrite(right_motor_positive, HIGH);
  digitalWrite(right_motor_negative, LOW);
}
void backward(int spd1, int spd2)
{
  
  analogWrite(en1, spd1);
  analogWrite(en2, spd2);
  digitalWrite(left_motor_positive, LOW);
  digitalWrite(left_motor_negative, HIGH);
  digitalWrite(right_motor_positive, LOW);
  digitalWrite(right_motor_negative, HIGH);
}


void stopmotor()
{
  
  analogWrite(en1, 0);
  analogWrite(en2, 0);
  digitalWrite(left_motor_positive, LOW);
  digitalWrite(left_motor_negative, LOW);
  digitalWrite(right_motor_positive, LOW);
  digitalWrite(right_motor_negative, LOW);
}

void right(int spd)
{

  analogWrite(en1, spd);
  analogWrite(en2, spd);
  digitalWrite(left_motor_positive, HIGH);
  digitalWrite(left_motor_negative, LOW);
  digitalWrite(right_motor_positive, LOW);
  digitalWrite(right_motor_negative, HIGH);
}

void left(int spd)
{

  analogWrite(en1, spd);
  analogWrite(en2, spd);
  digitalWrite(left_motor_positive, LOW);
  digitalWrite(left_motor_negative, HIGH);
  digitalWrite(right_motor_positive, HIGH);
  digitalWrite(right_motor_negative, LOW);
}

void turnleft()
{
  stopmotor();
  delay(50);
  readirsensor();
  left(rotating_speed);
  delay(425);
  do {
    left(rotating_speed);
    readirsensor();
  } while (ir[2]!=w);
  stopmotor();
  right(rotating_speed);
  delay(50);
}
void turnright()
{
  readirsensor();
  right(rotating_speed);
  delay(425);
  do {
    right(rotating_speed);
    readirsensor();
  } while (ir[1]!=w || ir[2]!=w);
  stopmotor();
  left(rotating_speed);
  delay(50);
}

void moveinch(){
  forward(forward_speed, forward_speed);
  delay(50);
//  stopmotor();
}
void endmaze(){
  stopmotor();
  leds();
  Status++;
}
void moveextra(){
  forward(forward_speed, forward_speed);
  delay(70);
//  stopmotor();
}
void calculatepid()
{
  pid_value = (kp * error);
}

void pidmotor()
{
  right_motor_speed = initial_motor_speed - pid_value;
  left_motor_speed = initial_motor_speed + pid_value;
  right_motor_speed = constrain(right_motor_speed, 0, initial_motor_speed);
  left_motor_speed = constrain(left_motor_speed, 0, initial_motor_speed);
  forward(left_motor_speed, right_motor_speed);
}
void sharpturn(){ 
  stopmotor();
  delay(425);
  do {
    left(75);
    readirsensor();
  } while (ir[2]!=w );
  stopmotor();
  right(75);
  delay(50);
}

void leds()
{
  digitalWrite(led,HIGH);
  delay(2000);
  digitalWrite(led,LOW);
}
void actualrun()
{
  
  while (Status == 1)
  {
    readirsensor();
    actualmotion();
  }
}



void actualmotion()
{
  switch (mode)
  {
    case 'F':
      calculatepid();
      pidmotor();
      break;
    case 'N':
      if (pathIndex >= pathLength)
        endmaze();
      else
      {
        mazeTurn (path[pathIndex]);
        pathIndex++;
      }
      break;
    case 'L':
      if (pathIndex >= pathLength)
        endmaze();
      else
      {
        mazeTurn (path[pathIndex]);
        pathIndex++;
      }
      break;
    case 'R':
      if (pathIndex >= pathLength)
        endmaze();
      else
      {
        mazeTurn (path[pathIndex]);
        pathIndex++;
      }
      break;
  }

}

void recIntersection(char Direction)
{
  path[pathLength] = Direction; // Store the intersection in the path variable.
  pathLength ++;
  simplifyPath(); // Simplify the learned path.
}

void simplifyPath()
{
  // only simplify the path if the second-to-last turn was a 'B'
  if (pathLength < 3 || path[pathLength - 2] != 'B')
    return;

  int totalAngle = 0;
  int i;
  for (i = 1; i <= 3; i++)
  {
    switch (path[pathLength - i])
    {
      case 'R':
        totalAngle += 90;
        break;
      case 'L':
        totalAngle += 270;
        break;
      case 'B':
        totalAngle += 180;
        break;
    }
  }

  // Get the angle as a number between 0 and 360 degrees.
  totalAngle = totalAngle % 360;

  // Replace all of those turns with a single one.
  switch (totalAngle)
  {
    case 0:
      path[pathLength - 3] = 'S';
      break;
    case 90:
      path[pathLength - 3] = 'R';
      break;
    case 180:
      path[pathLength - 3] = 'B';
      break;
    case 270:
      path[pathLength - 3] = 'L';
      break;
  }
  // The path is now two steps shorter.
  pathLength -= 2;
}

void mazeTurn (char dir)
{
  switch (dir)
  {
    case 'L': // Turn Left
      turnleft();
      break;

    case 'R': // Turn Right
      turnright();
      break;

    case 'B': // Turn Back
      turnleft();
      break;

    case 'S': // Go Straight
      move_extra();
      break;
  }
}

//loop fixing functions--------------------------------------------------------------------------------------------
void loopreducing(){
  i = 0;
  while((i+4) < pathLength){
    if(path[i+1]== 'R' && path[i+2]== 'R' && path[i+3]== 'R' && path[i]== 'S' && path[i+4]== 'L'){
      path[i]= 'B';
      j=i+1;
      manage();
    }
    else if(path[i+1]== 'R' && path[i+2]== 'R' && path[i+3]== 'R' && path[i]== 'L' && path[i+4]== 'S'){
      path[i]= 'B';
      j=i+1;
      manage();
    }
    else if(path[i+1]== 'R' && path[i+2]== 'R' && path[i+3]== 'R' && path[i]== 'L' && path[i+4]== 'L'){
      path[i]= 'R';
      j=i+1;
      manage();
    }
    else if(path[i+1]== 'R' && path[i+2]== 'R' && path[i+3]== 'R' && path[i+4]== 'R' && path[i]== 'L' && path[i+5]== 'L'){
      path[i]= 'B';
      j=i+1;
      manageR();
    }
    i++;  
  }
}

void finalreducing(){
  j = 0;
  while((j+2) < pathLength){
    // only simplify the path if the second-to-last turn was a 'B'
  if (path[j + 1] != 'B'){
    j++;
    return;
  }
  int totalAngle = 0;
  int i;
  for (i = 0; i <= 2; i++)
  {
    switch (path[j+i])
    {
      case 'R':
        totalAngle += 90;
        break;
      case 'L':
        totalAngle += 270;
        break;
      case 'B':
        totalAngle += 180;
        break;
    }
  }

  // Get the angle as a number between 0 and 360 degrees.
  totalAngle = totalAngle % 360;

  // Replace all of those turns with a single one.
  switch (totalAngle)
  {
    case 0:
      path[j] = 'S';
      break;
    case 90:
      path[j] = 'R';
      break;
    case 180:
      path[j] = 'B';
      break;
    case 270:
      path[j] = 'L';
      break;
  }
  int k = j + 1;
  while((k+2) < pathLength){
    path[k] = path[k+2];
    k++;  
  }
  // The path is now two steps shorter.
  pathLength -= 2;
  j++;
  }
}

void manage(){
  while((j+4) < pathLength){
    path[j] = path[j+4];
    j++;  
  }
  pathLength = pathLength - 4;
}

void manageR(){
  while((j+5) < pathLength){
    path[j] = path[j+5];
    j++;  
  }
  pathLength = pathLength - 5;
}


void move_extra()
{
 forward(forward_speed, forward_speed);
  delay(50);
//  backward(forward_speed, forward_speed);
//  stopmotor();
}
