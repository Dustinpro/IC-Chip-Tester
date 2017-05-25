// Arduino Project on TTL IC Tester
// The project by Dustin, Bob, Hanson, and Gary
// Codes by Dustin


int output = 0;
int pin[15], L[10], i, c0, c1, c2, d, type, Type, boo;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //These pins are especially for the pairing of TTL chip and the Arduino
  //pin[TTL chip's pin] = Arduino's pin
  pin[1] = 3;
  pin[2] = 5;
  pin[3] = 10;
  pin[4] = 6;
  pin[5] = 9;
  pin[6] = 7;
  pin[13] = 11;
  pin[12] = 8;
  //L[1:3] are inputs for the output logic
  L[1] = pin[1];
  L[2] = pin[2];
  L[3] = pin[4];
  //L[7:8] control the GREEN and RED LEDs
  L[7] = pin[5];
  L[8] = pin[13];
  
  //The initial setup of TTL chip pins' functions
  //(INPUT or OUTPUT) for the Arduino
  pinMode(pin[1], OUTPUT);
  pinMode(pin[2], OUTPUT);
  //The third pin may be an INPUT or OUTPUT, so it is left out.
  pinMode(pin[4], OUTPUT);
  pinMode(pin[5], OUTPUT);
  pinMode(pin[6], INPUT);
  pinMode(pin[13], OUTPUT);
  pinMode(pin[12], INPUT); //It is an input for a 3-input chip but an output for 2-input chip.
  
  
  Serial.println("A new TTL chip test begins!");
  reset();
  // Test1 is aimed at figuring out whether it is a 3-input TTL chip
  // If it is a 3-input TTL chip, then determine NAND3 or NOR3
  Test1();
  reset();
  // Test2 is aimed at figuring out a 2-input chip's type
  // as well as whether it is broken (both for a 2-input chip or 3-input chip)
  Test2();
  reset();
  Serial.println("");
}

// Test1 is aimed at figuring out whether it is a 3-input TTL chip
// If it is a 3-input TTL chip, then determine NAND3 or NOR3
void Test1(){
  type = 1; // Assume it is a 2-input TTL chip.
  
  //Try to find out whether it is a 3-input TTL chip.
  c0 = readOutput3(pin[1], true, pin[2], true, pin[13], true, pin[12]);
  if(c0 == 1){
  	return;
  }
  else{
    for(i = 1; i <= 8; i++){
      delay(5);
      d = readOutput3(pin[1], true, pin[2], true, pin[13], true, pin[12]);
      if(c0 != d){
        return;
      }
    }
  }
  c1 = readOutput3(pin[1], false, pin[2], false, pin[13], false, pin[12]);
  if(c1 == 0){
  	return;
  }
  else{
    for(i = 1; i <= 8; i++){
      delay(5);
      d = readOutput3(pin[1], false, pin[2], false, pin[13], false, pin[12]);
      if(c1 != d){
        return;
      }
    }
  }
  c2 = readOutput3(pin[1], true, pin[2], false, pin[13], false, pin[12]);
  if(c0 == 0 && c1 == 1 && c2 == 0){
    type = 3; // 3-input NOR
    return;
  }
  type = 4; // 3-input NAND
}

//Read outputs for 2-input gates
int readOutput(int inPin1, int in1, int inPin2, int in2, int outPin){
  digitalWrite(inPin1, in1);
  digitalWrite(inPin2, in2);
  //Delay is used to ensure that the gate has produced the output
  delay(5);
  return digitalRead(outPin);
}

//Read outputs for 3-input gates
int readOutput3(int inPin1, int in1, int inPin2, int in2, int inPin3, int in3, int outPin){
  digitalWrite(inPin1, in1);
  digitalWrite(inPin2, in2);
  digitalWrite(inPin3, in3);
  //Delay is used to ensure that the gate has produced the output
  delay(5);
  return digitalRead(outPin);
}

//Determine the hash value for the truth table of a 2-input TTL chip
int hashValue(int IN1,int IN2,int OUT){
	return readOutput(pin[IN1], false, pin[IN2], false, pin[OUT]) + readOutput(pin[IN1], false, pin[IN2], true, pin[OUT])*2 + \
	readOutput(pin[IN1], true, pin[IN2], false, pin[OUT])*4 + readOutput(pin[IN1], true, pin[IN2], true, pin[OUT])*8;
}

//Determine the hash value for the truth table of a 3-input TTL chip
int hashValue3(int IN1,int IN2,int IN3,int OUT){
	return readOutput3(pin[IN1], false, pin[IN2], false, pin[IN3], false, pin[OUT]) + readOutput3(pin[IN1], false, pin[IN2], false, pin[IN3], true, pin[OUT])*2 + \
	readOutput3(pin[IN1], false, pin[IN2], true, pin[IN3], false, pin[OUT])*4 + readOutput3(pin[IN1], false, pin[IN2], true, pin[IN3], true, pin[OUT])*8 + \
	readOutput3(pin[IN1], true, pin[IN2], false, pin[IN3], false, pin[OUT])*16 + readOutput3(pin[IN1], true, pin[IN2], false, pin[IN3], true, pin[OUT])*32 + \
	readOutput3(pin[IN1], true, pin[IN2], true, pin[IN3], false, pin[OUT])*64 + readOutput3(pin[IN1], true, pin[IN2], true, pin[IN3], true, pin[OUT])*128;
}

//It is used for initialization as well as
// avoiding as much as unnecessary light during the test process as possible
void reset(){
	for(i = 1; i <= 3; i++){
		digitalWrite(L[i], LOW);
	}
	digitalWrite(L[7], LOW);
	digitalWrite(L[8], LOW);
}

// Test2 is aimed at figuring out a 2-input chip's type
// as well as whether it is broken (both for a 2-input chip or 3-input chip)
void Test2() {
  boo = 1;
  switch(type){
    case 1:
      pinMode(pin[3], INPUT);
      pinMode(pin[12], OUTPUT);
      
      // figure out a 2-input chip's type through the first three pins
      Type = hashValue(1, 2, 3);
	  
      reset();
      //Control the serial monitor to give the chip's info (type and integrity)
      switch(Type){
        case 6:
          Serial.println("2-input XOR");
          break;
        case 7:
          Serial.println("2-input NAND");
          break;
        case 8:
          Serial.println("2-input AND");
          break;
        case 14:
          Serial.println("2-input OR");
          break;
        default:
          Serial.println("Please insert the TTL chip correctly.");
      	  boo = 0;
      	  return;
      }
      if(hashValue(4, 5, 6) != Type){
	  	Serial.println("This TTL chip does not work normally!");
	  	boo = 0;
	  }
      return;
    
    case 3:
      pinMode(pin[3], OUTPUT);
      //Control the serial monitor to give the chip's info (type and integrity)
      Serial.println("3-input NOR");
      if(hashValue3(1, 2, 13, 12) != hashValue3(3, 4, 5, 6)){
      	Serial.println("This TTL chip does not work normally!");
      	boo = 0;
      	return;
	  }
      return;
      
    case 4:
      pinMode(pin[3], OUTPUT);
      //Control the serial monitor to give the chip's info (type and integrity)
      Serial.println("3-input NAND");
      if(hashValue3(1, 2, 13, 12) != hashValue3(3, 4, 5, 6)){
      	Serial.println("This TTL chip does not work normally!");
      	boo = 0;
      	return;
	  }
      return;
  }
}

//Determine inputs for output logic to light those LEDs
void loop(){
  //Light the GREEN/RED (unbroken/broken) LEDs
  //boo == 0 means it is broken
  if(boo){
  	//unbroken-->GREEN LED ON
	digitalWrite(L[7], HIGH);
  }
  else
  {
  	//broken-->RED LED ON
	digitalWrite(L[8], HIGH);
  }
  
  //Determine inputs for output logic of chip types to light those LEDs
  switch(type){
    case 1:
      switch(Type){
        case 6:
	      //XOR2 74LS86
	      digitalWrite(L[1], HIGH);
	      digitalWrite(L[2], HIGH);
	      digitalWrite(L[3], HIGH);
          break;
        case 7:
	      //NAND2 74LS00
	      digitalWrite(L[1], LOW);
	      digitalWrite(L[2], HIGH);
	      digitalWrite(L[3], HIGH);
          break;
        case 8:
	      //AND2 74LS08
	      digitalWrite(L[1], HIGH);
	      digitalWrite(L[2], HIGH);
	      digitalWrite(L[3], LOW);
          break;
        case 14:
	      //OR2 74LS32
	      digitalWrite(L[1], LOW);
	      digitalWrite(L[2], HIGH);
	      digitalWrite(L[3], LOW);
          break;
      }
      break;
    
	case 3:
	  //NOR3 74LS27
	  digitalWrite(L[1], HIGH);
	  digitalWrite(L[2], LOW);
	  digitalWrite(L[3], HIGH);
      break;
      
    case 4:
	  //NAND3 74LS10
	  digitalWrite(L[1], LOW);
	  digitalWrite(L[2], LOW);
	  digitalWrite(L[3], HIGH);
	  break;
  }
}
