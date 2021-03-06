/**************************************************************************
 *  Copyright 2008,2009 KISS Institute for Practical Robotics             *
 *                                                                        *
 *  This file is part of CBC Firmware.                                    *
 *                                                                        *
 *  CBC Firmware is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  CBC Firmware is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this copy of CBC Firmware.  Check the LICENSE file         *
 *  in the project root.  If not, see <http://www.gnu.org/licenses/>.     *
 **************************************************************************/

//CBC user library functions
//--DPM 12/27/08

//Note that button functions and display function have not been updated yet (see end of file)
//Note that voltage and sonar scaling probably need adjustment

////////////////////////////////////////////////////////////////////////////////////////////
// Function prototypes
/* Includes the Standard IO Library */

#include "cbc.h"
#include "cbc_data.h"
#include <assert.h>
#include "shared_mem.h"

shared_mem *g_cbc_data_sm = 0;
int __pid_defaults[6]={30,0,-30,70,1,51};
int __position_threshold=2000;

void kissSim_init(int world, int rx, int ry, int rt){}
void kissSim_end(){}
void kissSimEnablePause(){}
void kissSimPause(){}
int kissSimActive(){return 1;}

/////////////////////////////////////////////////////////////
// Shared memory functions
cbc_data *cbc_data_ptr()
{
  if(!g_cbc_data_sm) {
    g_cbc_data_sm = shared_mem_create("/tmp/cbc_data", sizeof(cbc_data));
    assert(g_cbc_data_sm);
  }
  return (cbc_data *)shared_mem_ptr(g_cbc_data_sm);
}

/////////////////////////////////////////////////////////////
// Tone Functions
void tone(int frequency, int duration)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(frequency <5 || frequency > 6000)frequency=0;
	cbc->tone_freq=frequency;
	msleep(duration);
	cbc->tone_freq=0;
}

void beep()
{
	tone(440, 100);
}


/////////////////////////////////////////////////////////////
// Sensor Functions

// retruns 1 or 0 unless port is out of bounds, then -1
int digital(int port)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(port < 8 && port >= 0) {
		// first clear the output enable bit, if it was set
		cbc->enable_digital_outputs = (255 ^ (1 << port)) & cbc->enable_digital_outputs;
		return (1 & (cbc->digitals >> port));
	}
	else{
		printf("Digital must be between 8 and 15\n");
		return -1;
	}
}


int set_digital_output_value(int port, int value)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(port < 8 && port >= 0) {
		// first set the output enable bit
		cbc->enable_digital_outputs = (1 << port) | cbc->enable_digital_outputs;
		// now set the output value
		cbc->digital_output_values = ((!(!value)) << port) | cbc->digital_output_values;
		return (0);
	}
	printf("Digital must be between 8 and 15\n");
	return -1;
}

int analog10(int port)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(port > 7 && port < 16) return (cbc->analogs[port-8]);
	printf("Analog sensors must be between 8 and 16\n");
	return -1;
}

// 8-bit analog for HB compatibility
int analog(int port)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(port > 7 && port < 16) return (cbc->analogs[port-8]/4);
	printf("Analog sensors must be between 8 and 16\n");
	return -1;
}

/////////////////////////////////////////////////////////////
// Accelerometer Functions

int accel_x() 
{
  cbc_data *cbc = cbc_data_ptr();
  
  return (cbc->acc_x-2048);
}

int accel_y() 
{
  cbc_data *cbc = cbc_data_ptr();
  
  return (cbc->acc_y-2048);
}

int accel_z() 
{
  cbc_data *cbc = cbc_data_ptr();
  
  return (cbc->acc_z-2048);
}


//////////////////////////
// Sonar function: returns distance in mm

int sonar(int port)
{
	if(port > 12 && port < 16) {
	  return ((int)((float)analog10(port)*8.79));
	}        
	printf("Sonar port must be between 13 and 15\n");
	return -1;
}

// returns distance in inches
int sonar_inches(int port)
{
	if(port > 12 && port < 16) {
	  return ((int)(0.346*(float)analog10(port)));
	}        
	printf("Sonar port must be between 13 and 15\n");
	return -1;
}

//////////////////////////
// Battery power functions

float power_level()
{
  cbc_data *cbc = cbc_data_ptr();
  
	float p = cbc->volts/4095.0;
	float scale = 8.4; //$$$$$$$$ This needs to be corrected $$$$$$$
	return (p*scale);
}



/////////////////////////////////////////////////////////////
// Servo Functions
void enable_servos()
{
  cbc_data *cbc = cbc_data_ptr();
  
	cbc->enable_servos=1;
}

void disable_servos()
{
  cbc_data *cbc = cbc_data_ptr();
  
	cbc->enable_servos=0;
}

int set_servo_position(int servo, int pos)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(servo < 1 || servo > 4)
	{
		printf("Servo index must be between 1 and 4\n");
		return -1;
	}
	if (pos < 0 || pos > 2047)
	{
		printf("Servo position must be between 0 and 2047\n");
		return -1;
	}
	cbc->servo_targets[servo-1]=pos;
	return 0;
}

int get_servo_position(int servo)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(servo < 1 || servo > 4)
	{
		printf("Servo index must be between 1 and 4\n");
		return -1;
	}
	return(cbc->servo_targets[servo-1]);
}

/////////////////////////////////////////////////////////////
// Motor Functions

//////////////////////////
// CBC-specific motor functions


#define MAX_VEL   1000
#define MAX_POS_POS 1073741824
#define MAX_NEG_POS -1073741824


int get_motor_position_counter(int motor)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return 0;
	}
	return (cbc->motor_counter[motor]/160);// for actual CBC, divide by 160
}


int clear_motor_position_counter(int motor)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return -1;
	}
	off(motor);//turn off anything running on this channel
        cbc->clear_motor_counters = (1 << motor); // set the bit to clear that counter
        cbc->motor_tps[motor]=0;
        msleep(20);

        cbc->motor_counter_targets[motor]=0;//if the motor is moving to a position, zero that position & stop motor
        cbc->clear_motor_counters = 0;
	msleep(20);// make sure this gets done before next user command is issued
	off(motor);//again, insure that nothing is running until after motor counter is reset.

        return 0;
}


int move_at_velocity(int motor, int velocity)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return -1;
	}
	if(cbc->pid_gains[motor][0]==0){//pid gains have not been set
	  int i;
	  for(i=0;i<6;i++)cbc->pid_gains[motor][i]=__pid_defaults[i];
          cbc->motor_thresholds[0]=__position_threshold;

	}
	cbc->motor_pwm[motor]=0;

        if(velocity < 0)cbc->motor_counter_targets[motor]=MAX_NEG_POS;
	else cbc->motor_counter_targets[motor]=MAX_POS_POS;
        cbc->motor_tps[motor] = (velocity*16)/10;// for actual CBC
	return 0;
}

int mav(int motor, int velocity)
{ return move_at_velocity(motor, velocity); }

// Move motor to goal_pos at given velocity.  The amount actually
// moved will be goal_pos - get_motor_position().
int move_to_position(int motor, int speed, int goal_pos)
{ 
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return -1;
	}
	if(cbc->pid_gains[motor][0]==0){//pid gains have not been set
	  int i;
	  for(i=0;i<6;i++)cbc->pid_gains[motor][i]=__pid_defaults[i];
          cbc->motor_thresholds[0]=__position_threshold;
	}
	if(speed<0)speed=-speed;//speed is always assumed to be positive
	cbc->motor_pwm[motor]=0;

	cbc->motor_counter_targets[motor]=160*goal_pos;// for actual CBC, multiply by 160
        if((160*goal_pos) < cbc->motor_counter[motor]) speed=-speed;//change speed to velocity
        cbc->motor_tps[motor]=(speed*16)/10;// for actual CBC, multiply by 160

        //cbc->motor_in_motion=cbc->motor_in_motion | (1 << motor);//solves bmd sync issue where this is not set by BoB b4 bmd command is executed
	return 0;
}

int mtp(int motor, int velocity, int goal_pos)
{ return(move_to_position(motor, velocity, goal_pos)); }


// Move delta_pos relative to the current position of the motor.  The
// final motor tick will be get_motor_position() at the time of the
// call + delta_pos.
int move_relative_position(int motor, int speed, int delta_pos)
{ 
  return move_to_position(motor, speed, get_motor_position_counter(motor)+delta_pos);
}


int mrp(int motor, int velocity, int delta_pos)
{
	return(move_relative_position(motor, velocity, delta_pos));
}

//Turns off or actively holds the motor in position depending  on the situation -- but it may drift
int freeze(int motor)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return -1;
	}
	cbc->motor_pwm[motor]=0;
	cbc->motor_counter_targets[motor]=cbc->motor_counter[motor];
	cbc->motor_tps[motor]=160*MAX_VEL/2;// for actual CBC, multiply by 160
	return 0;
}

void set_pid_gains(int motor, int p, int i, int d, int pd, int id, int dd)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
	}
	cbc->pid_gains[motor][0]=p;
	cbc->pid_gains[motor][1]=i;
	cbc->pid_gains[motor][2]=d;
	cbc->pid_gains[motor][3]=pd;
	cbc->pid_gains[motor][4]=id;
	cbc->pid_gains[motor][5]=dd;
}


//returns 0 if motor is in motion and 1 if it has reached its target position
int get_motor_done(int motor)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return -1;
	}
        //return(!(1 & (cbc->motor_in_motion >> motor)));
        if(cbc->motor_counter[motor] > (cbc->motor_counter_targets[motor]-cbc->motor_thresholds[0]) &&
           cbc->motor_counter[motor] < (cbc->motor_counter_targets[motor]+cbc->motor_thresholds[0])) return 1;
        else return 0;
}

void bmd(int motor)
{
	//loop doing nothing while motor position move is in progress
	while(!get_motor_done(motor)){msleep(10);}
}

void block_motor_done(int motor)
{
	//loop doing nothing while motor position move is in progress
	while(!get_motor_done(motor)){msleep(10);}
}

int setpwm(int motor, int pwm)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return -1;
	}
	cbc->motor_tps[motor]=0;
	cbc->motor_pwm[motor]=pwm;
	return 0;
}

int getpwm(int motor)
{
  cbc_data *cbc = cbc_data_ptr();
  
	if(motor <0 || motor>3) {
		printf("Motors are 0 to 3\n");
		return -1;
	}
	return(cbc->motor_pwm[motor]);
}

//////////////////////////
// Classic pwm motor functions, scaled for XBC and Blk gear motors
/***************************************************************** */
/*                                                                */
/* MOTOR PRIMITIVES                                               */
/*                                                                */
/*   fd(n) sets motor n to full on in the green direction    */
/*   bk(n) sets motor n to full on in the red direction      */
/*   motor(n, s) sets motor n on at speed s;               */
/*     s= 100 is full on green,                  */
/*     s= -100 is full on red,                   */
/*     s= 0 is off                               */
/*   off(n)  turns off motor n      */
/*                                                                */
/*   alloff() turns off all motors                      */
/*   ao()  turns off all motors                      */
/*                                                                */
/*                                                                */
/*   motors are numbered 0 through 3.                          */
/***************************************************************** */

void fd(int motor)
{
	//    move_at_velocity(motor, MAX_VEL);
	setpwm(motor,255);
}

void bk(int motor)
{ 
	setpwm(motor,-255);
	//    move_at_velocity(motor, -MAX_VEL);
}

void motor(int port, int speed)
{
	setpwm(port,(255*speed)/100);
}



void off(int motor)
{
	setpwm(motor, 0);
}

void alloff()
{
	setpwm(0, 0);
	setpwm(1, 0);
	setpwm(2, 0);
	setpwm(3, 0);
}

void ao()
{
	setpwm(0, 0);
	setpwm(1, 0);
	setpwm(2, 0);
	setpwm(3, 0);
}




/////////////////////////////////////////////////////////////
// Button Functions
int black_button()
{
  cbc_data *cbc = cbc_data_ptr();
  
  return cbc->button;
}

int up_button()
{
  cbc_data *cbc = cbc_data_ptr();
  
  return cbc->up;
}

int down_button()
{
  cbc_data *cbc = cbc_data_ptr();
  
  return cbc->down;
}

int left_button()
{
  cbc_data *cbc = cbc_data_ptr();
  
  return cbc->left;
}

int right_button()
{
  cbc_data *cbc = cbc_data_ptr();
  
  return cbc->right;
}

int a_button()
{
  cbc_data *cbc = cbc_data_ptr();
  
  return cbc->a;
}

int b_button()
{
  cbc_data *cbc = cbc_data_ptr();
  
  return cbc->b;
}

void display_clear()
{
  printf("%c",12);
}

/////////////////////////////////////////////////////////////
// Display Functions
// Copyright(c) KIPR, 2009
// Full screen management functions for the display window provided with the CBC console screen
//   1. cbc_display_clear()
//   2. cbc_display_printf(int column, int row, <printf arguments>)
//
// Console screen display window size is 7 rows and limited to 28 columns, indexed from 0
// (variable pitch font used by CBC necessitates using a shortened width and prevents using
//  the slicker strategy of treating the display as one very long string)
//
// Initial version: 1/27/2009 - cnw
//
// stdarg.h provides macros for accessing a function's argument list ... see K&R

#define _MAPy 7
#define _MAPx 29  // last column is for \0
char _display_map[_MAPy][_MAPx]; 

#include <stdarg.h>
#include <string.h>

void cbc_display_clear() {
  int x,y;
  printf("\n\n\n\n\n\n\n");  // blow it away and reset map
  for(y=0;y<_MAPy;y++)       
    for(x=0;x<_MAPx; x++) _display_map[y][x]=' ';   // blank out the map and
  for(y=0;y<_MAPy;y++) _display_map[y][_MAPx-1]='\0'; // make each row a string
  msleep(100);   // needs a brief pause to empty display
}

void cbc_printf(int col, int row, char *t, ...) { // column, row, string with format phrases, args
  va_list argp;    // argp is typed for traversing the variable part of the arg list
  int i; char *c; double d;  // working variables to receive arg values
  char *cp, *fmte, sc;  // cp traverses format string t, fmte marks end of each format phrase, sc is switch control
  
  int y;           // row index
  char *dp;        // pointer into display
  int maxw;        // available room on line
  char fws[_MAPx]; // formatted phrase work area
  char fmt[_MAPx]; int fl; // fmt is a working string for each format extracted
  
  va_start (argp,t);  // t is last named argument in cbc_printf's function header;
  // this initializes argp to point to first variable arg
  
  dp = &_display_map[row][col]; // starting point for printf
  maxw=_MAPx-col;               // space remaining on line
  for (cp = t; *cp; cp++)       // process printf string; stop when *cp = '\0'
    {
      if (*cp != '%')           // if not a format phrase 
	{
	  if(strspn(cp,"\n\t\v\b\r\f")>0) {  // is it a spec char? if so treat as if \n
	    for (i=0;i<maxw;i++) { // clear balance of line
	      *dp=' '; dp++;
	    }
	    if (row==_MAPy) break; // out of rows so proceed to display refresh
	    row++;
	    dp = &_display_map[row][0]; maxw=_MAPx; // set up for new line
	  }
	  else { // nothing special about this one so insert it          
	    *dp=*cp; dp++;
	    maxw--; if (maxw==0) break; // no more room on line so proceed to display refresh
	  }
	  continue;             // return to top
	}
      // OK, if we're here we may have hit a format phrase
      fmte = strpbrk(cp+1, "dioxXucsfeEgG%"); // look for format end        
      // strpbrk returns the location of 1st character of its argument that is in the scan string
      if (fmte == NULL)        // what's left is not a format phrase so insert % and return to top       
	{                        
	  *dp='%'; dp++; 
	  maxw--; if (maxw==0) break; // no more room on line so proceed to display refresh
	  continue;            // return to top
	}
      // OK, there looks to be a format phrase
      sc = *fmte;              // set switch control for the case 
      fl = 1+fmte-cp;          // pick off phrase (pointed to by cp)
      strncpy(fmt,cp,fl);      // capture the format phrase
      fmt[fl] = '\0';          // make it a string
      switch (sc)              // process the kind of format specified
	{
	case 'd': case 'i': case 'o': case 'x': case 'X': case 'u': case 'c':
	  i = va_arg(argp, int);    // return next parm as type int and step argp
	  sprintf(fws,fmt,i);       // use sprintf to do the formatting to fws
	  break;
	case 's':
	  c = va_arg(argp, char *); // return next parm as type char * and step argp
	  sprintf(fws,fmt,c);       // use sprintf to do the formatting to fws
	  break;
	case 'f': case 'e': case 'E': case 'g': case 'G':
	  d = va_arg(argp, double); // return next parm as type double and step argp
	  sprintf(fws,fmt,d);       // use sprintf to do the formatting to fws
	  break;
	case '%':                 // no format specified between %'s
	  sprintf(fws,fmt);
	  break;
	}
      for(i=0; i<strlen(fws); i++) {// insert formatted phrase in display map
	if (maxw==0) break;       // if no more room get out of this
	*dp = fws[i];             // insert next character from formatted phrase
	dp++; maxw--;
      }
      if (maxw==0) break;           // if no more room proceed to display refresh
      cp = fmte;                    // set cp for next pass
    }
  va_end(argp);                     // clean up
  for(y=0;y<_MAPy; y++) _display_map[y][_MAPx-1]='\0';  // make each row a string
  for(y=0;y<_MAPy; y++) printf("\n%s",_display_map[y]); // refresh the display
}

