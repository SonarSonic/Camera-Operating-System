#include <Servo.h>

Servo servoX; //instance of Servo X
Servo servoY; //instance of Servo Y

//servo X
int pinX = 0; // the pin Servo X is connected to
int valX; // the current position of potentiometer X
int moveX; //current movement of the Joystick on the X potentiometer, when this equals 1 or -1 it will be reset to 0 and the x servo will be moved accordingly

//servo Y
int pinY = 1; // the pin Servo Y is connected t
int valY; // the current position of potentiometer Y
int moveY;  //current movement of the Joystick on the Y potentiometer, when this equals 1 or -1 it will be reset to 0 and the x servo will be moved accordingly

//servo X start and finish of currentMove
int currentX=0;
int targetX=0;

//servo Y start and finish of currentMove
int currentY=0;
int targetY=0;

//millisecond delay of each servo's movement to help keep their movemement in time.
float delayX;
float delayY;

bool move1;
bool move2;

unsigned long currentDelayX = 0; //milliseconds since Servo X was last moved
unsigned long currentDelayY = 0; //milliseconds since Servo Y was last moved
unsigned long time = 0; //how long the servo has been runned on for in milliseconds

int currentMove; //the movement in the array list which is currently being performed
int timePerMovement=5000; //the time taken to perfom each movement, used for calculating the delay given to each servo.

//current activity
int currentState = 0; // the current state of the Arduino e.g. Active, Inactive, Idle etc.

//arrays
const int maxArraySize = 50; //the maximum amount of points which can be stored
int pointCount=0; // the current amount of points stored
int positionsX[maxArraySize]; //the list of positions for X Servo
int positionsY[maxArraySize]; //the list of positions for Y Servo

//buttons
int start = 2; //the pin the start button is connected to
int startVal; //the current state of the start button
int add = 3; //the pin the add button is connected to
int addVal; //the current state of the add button
int remove = 4; //the pin the remove button is connected to
int removeVal; //the current state of the remove button
int clear = 5; //the pin the clear button is connected to
int clearVal; //the current state of the clear button
int pause = 6;
int pauseVal;
bool paused = false;

void setup(){ //this method is called when the Ardunio is started and gives you the chance change settings and connect servos as needed
  //SERVOS
  servoX.attach(9); //connects the Servo X to pin number 9
  servoY.attach(10); //connects the Servo Y to pin number 10
   //LEDS
  pinMode(13, OUTPUT); //lets pin 13 output a signal to the red LED
  pinMode(12, OUTPUT); //lets pin 12 output a signal to the green LED
  pinMode(11, OUTPUT); //lets pin 11 output a signal to the blue LED
}

void loop(){    
  if(currentState==2){ //checks if an error has been found
    return; //if an error was found the loop method is terminatated
  }
  startVal = digitalRead(start); //start button
  if(currentState!=1 && startVal == HIGH){ //if someone pressed the start button
    if(pointCount==0){//if no points have been added signal an error
      setError();
      delay(1000);
      setNotActive();
    }else{ //otherwise activate the movement
      setActive();
      runCycle();
    }
  }
  //move Servo X according to potentiometer
  valX = analogRead(pinX);
  valX = map(valX, 0, 1023, -90, 90);
  
  if(!(valX<=3 && valX>=-3)){ //compensate for Joy-Stick inaccuracy
    moveX += valX;
    if(moveX>90){
      valX = servoX.read();
      //if(valX+1<=180){
      servoX.write(valX+moveX/90);
      //}    
      moveX=0;
    }else if(moveX<-90){
      valX = servoX.read();
      //if(valX-1>=0){
      servoX.write(valX-(moveX/-90));
      //}
      moveX=0;
    }
  }
  //move Servo Y according to potentiometer
  valY = analogRead(pinY);
  valY = map(valY, 0, 1023, -90, 90);
  if(!(valY<=3 && valY>=-3)){ //compensate for Joy-Stick inaccuracy
    moveY += valY;
    if(moveY>90){
      valY = servoY.read();
      //if(valY+1<=180){
      servoY.write(valY+moveY/90);
      //}    
      moveY=0;
    }else if(moveY<-90){
      valY = servoY.read();
      // if(valY-1>=0){
      servoY.write(valY-(moveY/-90));
      //}
      moveY=0;
  }
  }
  delay(15);
  
  addVal = digitalRead(add);
  if(addVal == HIGH){ //if the add button is being pressed, add a new point
    addPoint();
  }
  
  removeVal = digitalRead(remove);
  if(removeVal == HIGH){  //if the remove button is being pressed, remove the last point
    removePoint();
  }
  
  clearVal = digitalRead(clear);
  if(clearVal == HIGH){ //if the clear button is being pressed, remove all points
    clearPoints();
  }
}

void runCycle(){
  
 for(currentMove=0;currentMove<pointCount;currentMove+=1){
    
    if(currentMove==0){//if the current movement is 0 set it to the start      
  servoX.write(positionsX[0]);
  servoY.write(positionsY[0]);
  delay(1000); 
    }
  if(currentMove+1<pointCount){ // if the current movement isn't the final movement the cycle will continue.
    currentX=positionsX[currentMove]; // sets the current position of x
    targetX=positionsX[currentMove+1]; // sets the target position of x
    currentY=positionsY[currentMove]; // sets the current position of y
    targetY=positionsY[currentMove+1]; // set the target position of y
      
    if(difference(currentX, targetX)!=0){ // checks the difference between target x and current x
      delayX=timePerMovement / difference(currentX, targetX); //if there is a difference it sets the delay for the movement to move at the same time to y
    }else{
      delayX=timePerMovement; //if there is no difference the wait time is equal to the time to carry out each movement.
    }
       
    if(difference(currentY, targetY)!=0){ // checks the difference between target y and current y
      delayY=timePerMovement / difference(currentY, targetY); //if there is a difference it sets the delay for the movement to move at the same time to x
          }else{
      delayY=timePerMovement; //if there is no difference the wait time is equal to the time to carry out each movement.
    }
       
          while(currentX!=targetX || currentY!=targetY){ // the code within the statement will keep running until the current X and Y points equal their targets.
      pauseVal = digitalRead(pause);
        /*
      if(pauseVal == HIGH){  //if the remove button is being pressed, remove the last point   
        paused=!paused;
        setIdle();
        delay(1000);
          
        if(!paused)
          setActive();
        else             
          setIdle();
       }
        */
      if(!paused){
        time = millis(); // records the current time
        move1 = false; // true or false statement if the x servo should move on this loop, currently false until told otherwise
        move2 = false; // true or false statement if the y servo should move on this loop, currently false until told otherwise
        
        if (currentX!=targetX && time - currentDelayX >= delayX){ //if the x servo hasn't yet reached the right position and the program has waited long enough to make sure the servos remain in sync.
          currentDelayX = time; //resets the time for the next loop
            
          if(positionsX[currentMove] <positionsX[currentMove+1]){ // if the original x is less than the target x
            currentX+=1; // the current x increases
                  move1 = true; // a movement is therefore required
                }else if(positionsX[currentMove] >positionsX[currentMove+1]){ // if the original x is greater than the target x
                  currentX-=1; // the current x decreases
            move1 = true; // a movement is signalled
                }   
        }
      
    
            if (currentY!=targetY && time - currentDelayY >= delayY){  //if the y servo hasn't yet reached the right position and the program has waited long enough to make sure the servos remain in sync. 
          currentDelayY = time;  //resets the time for the next loop
                if(positionsY[currentMove] <positionsY[currentMove+1]){// if the original x is less than the target x
                      currentY+=1; // the current x increases                 
                  move2 = true; // a movement is therefore required
                  }else if(positionsY[currentMove] > positionsY[currentMove+1]){ // if the original x is greater than the target x
                      currentY-=1; // the current x decreases
            move2 = true; // a movement is signalled
          }          
              }
            if(move1){      
          servoX.write(currentX); //writes the movement to servo x
              }
          if(move2){      
          servoY.write(currentY); // write the movement to servo y
            }  
              delay(10);
       }
          }     
      }
  }
  setIdle();
  delay(1000);
  setNotActive();  
}

void addPoint(){ // when the add point button has been pressed
  
  if(pointCount<maxArraySize){ // checks their is space for the point
    positionsX[pointCount]=servoX.read(); // sets the x position in the array to the current resistor position 
    positionsY[pointCount]=servoY.read(); // sets the y position in the array to the current resistor position 
    pointCount+=1; // increases the current point count
    setIdle(); // the Blue Led is activated to show the point has been logged
    delay(1000);
    setNotActive();
  }else{ // if no space is available an error is signalled
    setError();
    delay(1000);
    setNotActive();
  }
}

void removePoint(){ //  when the remove point button has been pressed
  
  if(pointCount!=0){ // checks that their are points to delete
    positionsX[pointCount-1]=0; // last x position is removed
    positionsY[pointCount-1]=0; // last y position is removed
    pointCount-=1; // the point count is decreased
    setIdle(); // the Blue Led is activated to show the removal has been logged
    delay(1000);
    setNotActive();
  }else{ //if all points have already been deleted an error is signalled
    setError();
    delay(1000);
    setNotActive();
  }
}

void clearPoints(){  // clears the entire list of points
 
  if(pointCount!=0){
    
    for(int clearLoop=0;clearLoop<maxArraySize;clearLoop++){
      positionsX[clearLoop]=0;
      positionsY[clearLoop]=0;
    }
    pointCount=0; // sets the current point cout to 0    
    setIdle(); // the blue LED is activated to show the clearance has been completed.
    delay(1000);
    setNotActive();
  }else{ //if all points have already been deleted an error is signalled
    setError();
    delay(1000);
    setNotActive();
  }
}

int difference(int val1, int val2){ // a method for compared to values and getting the difference
  if(val1>val2){
    return val1-val2;
  }
  if(val1<val2){
    return val2-val1;
  }
  return 0;
}

// methods for the setting of LED colours

void setNotActive(){ 
  currentState=0;
  setLED();
}

void setActive(){  
  currentState=1;
  setLED();
}

void setError(){  
  currentState=2;
  setLED();
}

void setIdle(){  
  currentState=3;
  setLED();
}

//0=NOT ACTIVE, 1=ACTIVE, 2=ERROR, 3=IDLE
void setLED(){
  //NOT ACTIVE
  if(currentState==0){    
    digitalWrite(13,LOW);
    digitalWrite(12,LOW);
    digitalWrite(11,LOW);  
    return;
  }
  //ACTIVE
  if(currentState==1){    
    digitalWrite(13,LOW);
    digitalWrite(12,LOW);
    digitalWrite(11, HIGH);
    return;
  }
  //ERROR
  if(currentState==2){    
    digitalWrite(13,HIGH);
    digitalWrite(12,LOW);
    digitalWrite(11,LOW);
    return;
  }
  //IDLE
  if(currentState==3){    
    digitalWrite(13,LOW);
    digitalWrite(12,HIGH);
    digitalWrite(11,LOW);
    return;
  }
}
