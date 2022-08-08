//-. elapsedMillis.h: library for measuring the time elapsed 

#include <Arduino.h>
#include <elapsedMillis.h>

//void stopwatch(): function for converting the elapsed time in milliseconds to HH:MM:SS format to be readable in SmartWorks Panopticon

elapsedMillis tmr; //initialize the timer

void stopwatch(char* time_array){
  unsigned long seconds = tmr/1000; 
  unsigned long minutes = seconds/60; 
  unsigned long hours = minutes/60; 

  minutes %= 60; 
  seconds %= 60; 
  //storing time in HH:MM:SS string
  String str_time = String() + (hours < 10 ? "0" : "") + String(hours) + ":" 
  + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "") + seconds; 

  //converting string to char array
  strcpy(time_array, str_time.c_str()); 
}