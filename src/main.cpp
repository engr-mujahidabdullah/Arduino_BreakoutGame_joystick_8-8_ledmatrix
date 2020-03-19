#include <Arduino.h>

// Define Pins
//Row Pins
#define ROW_1 5
#define ROW_2 4
#define ROW_3 3
#define ROW_4 2
#define ROW_5 A5
#define ROW_6 A4
#define ROW_7 A3
#define ROW_8 A2

//Columns Pin
#define COL_1 6
#define COL_2 7
#define COL_3 8
#define COL_4 9
#define COL_5 10
#define COL_6 11
#define COL_7 12
#define COL_8 13

//JoyStick pin
#define joy A0

//GLOBALS

int joy_pin = joy;
int x_val = 5;
int player_Position;
int ba_x  ; int ba_y ;
boolean ba_up = true;
int ba_dir = 2;
boolean game  = true; 
boolean l_c = false;
unsigned long last_t = 0;
unsigned long move_inter;
unsigned long now = 0; unsigned long total_t = 10000000;

//Define Pin Matrix
int rows[] = {ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8};
int col[] = {COL_1,COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8};

// Define Display Array
unsigned char shape[8][8] = 
{ 0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0, 
  0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0, 
  0,0,0,0,0,0,0,0,  
};  

unsigned char shape_up[8][8] =      //the small "heart" 
{  
  1,1,1,1,1,1,1,1,  
  1,1,1,1,1,1,1,1,  
  1,1,1,1,1,1,1,1,  
  0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0,  
  0,0,0,0,0,0,0,0, 
  0,0,0,0,0,0,0,0,  
};


//Function to turn off rows and columns
void Clear(int C[], int R[])                    
{  
  for(int k = 0;k<8;k++)  
  {  
    digitalWrite(R[k],LOW);  
    digitalWrite(C[k],HIGH);  
  }  
} 

// Function to display brick lines
void lines()
{
    for (int i = 0; i < 3;i++)
    {
      for (int j = 0; j <8; j++)
      {
        shape_up[i][j] = 1;
      }    
    }
}

//function to clear display matrix
void clear_clear(unsigned char del[8][8])
{
  for(int i = 0; i < 8; i++)
        {
          for(int j = 0; j < 8; j++)
          {
            del[i][j] = 0;
          }
        }
}

//function for display
void Display(unsigned char dat[8][8], int C[], int R[])    
{  
  for(int c = 0; c<8;c++)  
  {  
    digitalWrite(C[c],LOW);//use thr column 
    //loop
    for(int r = 0;r<8;r++)  
    {  
      digitalWrite(R[r],dat[r][c] || shape_up[r][c]);
      //digitalWrite(dat[r][c],R[r])  ;
    }  
    delay(1);  
    Clear(C, R);  //Remove empty display light
    
  }  
}  

//function to initiallize game at restart
void restartGame()
{
  for(int i =0;i<8;i++){
    for (int j=0;j<3;j++)
      shape_up[j][i] = 1;
  }
  move_inter = 700;
  ba_x = 3;
  ba_y = 6;
  ba_up = true;
  ba_dir = 2;
  game = true;
  l_c = false;
  
}

//function to control ball position and action 
void ball(unsigned char upd[8][8])
{
  if(ba_dir == 1)
    ba_x--;
  else if(ba_dir == 3)
    ba_x++;
  else if(ba_dir == 2)
    ba_x = ba_x;

  if(ba_x == 0)
    ba_dir = 3;
  else if (ba_x == 7)
    ba_dir = 1;

  if(ba_up)
    ba_y--;
  else
    ba_y++;
  

  if(ba_y == 6 && ba_x == player_Position)
  {
    ba_up = true;
    ba_dir = 1;
    move_inter -= 2;
  }

  if(ba_y == 6 && ba_x == player_Position +1)
  {
    ba_up = true;
    ba_dir = 2;
    move_inter -= 2;
  }

  if(ba_y == 6 && ba_x == player_Position + 2)
  {
    ba_up = true;
    ba_dir = 3;
    move_inter -= 2;
  }

  else if (ba_y == 0)
  {
    ba_up = false;
    move_inter -= 2;
  }

  else if (ba_y == 7)
  {
    Serial.println("GAME OVER");
    delay(1000);
    restartGame();
  }

  upd[ba_y][ba_x] = 1;
}

//function to display and move paddle
void paddle_(int pin_,unsigned char disp[8][8])
{
  int player_Value = analogRead(pin_);
      player_Position = player_Value * 6 / 1024;

      for(int i = 0; i < 8; i++)
        {
          if(i >= player_Position && i < player_Position + 3)
          {
            disp[7][i] = 1;
          }
    
          else
            {
              disp[7][i] = 0;
            }
        }
}

//function to remove bricks
void line_ball()
{
 for(int ba_xa = 0; ba_xa < 8; ba_xa++)
 {
   for(int ba_ya = 0; ba_ya < 3 ; ba_ya++)
   {
     if(ba_xa == ba_x && ba_ya == ba_y && shape_up[ba_y][ba_x] != 0)
     {
        shape_up[ba_y][ba_x] = 0;
        ba_up = false;
        if(ba_dir == 1) ba_dir = 3;
        if(ba_dir == 2) ba_dir = 2;
        if(ba_dir == 3) ba_dir = 1;
     }
   }
 }
}

//function to display Array
void update()
{
    if(now - last_t > move_inter)
     {
      clear_clear(shape);
      paddle_(joy_pin, shape);
      ball(shape);
      line_ball();
      last_t = now;      
    }   
}

//function to read joystick
void readSensors(int pin) 
{
  x_val = 7 - map(analogRead(pin), 0, 1023, 0, 7);
  Serial.println(x_val);
}

void setup() 
{
  Serial.begin(9600);
  for(int i = 0; i < 8; i++)
    {
      pinMode(rows[i], OUTPUT);
      pinMode(col[i], OUTPUT);
    }
  pinMode(joy_pin, INPUT);
  restartGame();
  
}

void loop() 
{
    now = millis(); 
    update();
    Display(shape,col, rows);
}

//end