/*
*  Jaikrishna
*  t.s.jaikrishna<at>gmail.com
*  Initial date: June 21, 2013
*  Based on Matthew Richardson's example on testing BrickPi drivers and Xander Soldaat's Example on NXT for RobotC
*  You may use this code as you wish, provided you give credit where it's due.
*  
*  This is a program for testing the RPi BrickPi drivers and I2C communication on the BrickPi with a dGPS
*/

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <math.h>
#include "tick.h"

#include <wiringPi.h>

#include "BrickPi.h"

//#include <unistd.h>  
//#include <errno.h>  
//#include <stdio.h>  
//#include <stdlib.h>  
#include <linux/i2c-dev.h>  
//#include <sys/ioctl.h>  
#include <fcntl.h>

// gcc -o program "Test BrickPi lib.c" -lrt -lm -L/usr/local/lib -lwiringPi
// gcc -o program "Test BrickPi lib.c" -lrt
// ./program

int result;


long UTC, lat, lon;
unsigned short head,velo,status;

#define I2C_PORT  PORT_1                             // I2C port for the dGPS
#define I2C_SPEED 0                                  // delay for as little time as possible. Usually about 100k baud

#define I2C_DEVICE_DGPS 0                        // dGPS is device 0 on this I2C bus

#define DGPS_I2C_ADDR   0x06      /*!< Barometric sensor device address */
#define DGPS_CMD_UTC    0x00      /*!< Fetch UTC */
#define DGPS_CMD_STATUS 0x01      /*!< Status of satellite link: 0 no link, 1 link */
#define DGPS_CMD_LAT    0x02      /*!< Fetch Latitude */
#define DGPS_CMD_LONG   0x04      /*!< Fetch Longitude */
#define DGPS_CMD_VELO   0x06      /*!< Fetch velocity in cm/s */
#define DGPS_CMD_HEAD   0x07      /*!< Fetch heading in degrees */

int main() {
  ClearTick();

  result = BrickPiSetup();
  printf("BrickPiSetup: %d\n", result);
  if(result)
    return 0;
  

  BrickPi.Address[0] = 1;
  BrickPi.Address[1] = 2;

  BrickPi.SensorType       [I2C_PORT]    = TYPE_SENSOR_I2C;
  BrickPi.SensorI2CSpeed   [I2C_PORT]    = I2C_SPEED;

  BrickPi.SensorI2CDevices [I2C_PORT]    = 1;
  
  BrickPi.SensorSettings   [I2C_PORT][I2C_DEVICE_DGPS] = 0;  
  BrickPi.SensorI2CAddr    [I2C_PORT][I2C_DEVICE_DGPS] = DGPS_I2C_ADDR;	//address for writing
  
  if(BrickPiSetupSensors())
    return 0;
	

  
  BrickPi.SensorSettings [I2C_PORT][I2C_DEVICE_DGPS]    = BIT_I2C_MID; // the dGPS device requires a clock change between reading and writing
  BrickPi.SensorI2CWrite [I2C_PORT][I2C_DEVICE_DGPS]    = 1;	//number of bytes to write
  BrickPi.SensorI2CRead  [I2C_PORT][I2C_DEVICE_DGPS]    = 4;	//number of bytes to read
 

  result = BrickPiSetupSensors();
  printf("BrickPiSetupSensors: %d\n", result); 
  if(!result){
    
    while(1){
	//read UTC
	  BrickPi.SensorI2CRead  [I2C_PORT][I2C_DEVICE_DGPS]    = 4;
	  BrickPi.SensorI2COut   [I2C_PORT][I2C_DEVICE_DGPS][0] = DGPS_CMD_UTC;	//byte to write
	  BrickPiSetupSensors();
      result = BrickPiUpdateValues(); //write and read
      if(!result){
        if(BrickPi.Sensor[I2C_PORT] & (0x01 << I2C_DEVICE_DGPS)){
		  UTC = ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][0]<<24) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][1]<<16) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][2]<<8) + (long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][3];        
        }
      }
     
	//read Latitude
	  BrickPi.SensorI2COut   [I2C_PORT][I2C_DEVICE_DGPS][0] = DGPS_CMD_LAT;	//byte to write
	  BrickPiSetupSensors();     
	  result = BrickPiUpdateValues(); //write and read
      if(!result){
        if(BrickPi.Sensor[I2C_PORT] & (0x01 << I2C_DEVICE_DGPS)){
		  lat = ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][0]<<24) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][1]<<16) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][2]<<8) + (long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][3];        
        }
      }
	  
    //read Longitude
	  BrickPi.SensorI2COut   [I2C_PORT][I2C_DEVICE_DGPS][0] = DGPS_CMD_LONG;	//byte to write
	  BrickPiSetupSensors();    
	  result = BrickPiUpdateValues(); //write and read
      if(!result){
        if(BrickPi.Sensor[I2C_PORT] & (0x01 << I2C_DEVICE_DGPS)){
		  lon = ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][0]<<24) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][1]<<16) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][2]<<8) + (long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][3];        
        }
      }
	  
    //read heading
	  BrickPi.SensorI2CRead  [I2C_PORT][I2C_DEVICE_DGPS]    = 2;
	  BrickPi.SensorI2COut   [I2C_PORT][I2C_DEVICE_DGPS][0] = DGPS_CMD_HEAD;	//byte to write
	  BrickPiSetupSensors();   
	  result = BrickPiUpdateValues(); //write and read
      if(!result){
        if(BrickPi.Sensor[I2C_PORT] & (0x01 << I2C_DEVICE_DGPS)){
		  head = ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][0]<<8) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][1]);        
        }
      }
	  
	//read status
	  BrickPi.SensorI2CRead  [I2C_PORT][I2C_DEVICE_DGPS]    = 1;
	  BrickPi.SensorI2COut   [I2C_PORT][I2C_DEVICE_DGPS][0] = DGPS_CMD_STATUS;	//byte to write
	  BrickPiSetupSensors();   
	  result = BrickPiUpdateValues(); //write and read
      if(!result){
        if(BrickPi.Sensor[I2C_PORT] & (0x01 << I2C_DEVICE_DGPS)){
		  status = ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][0]);
        }
      }
	
	//read velocity
	  BrickPi.SensorI2CRead  [I2C_PORT][I2C_DEVICE_DGPS]    = 3;
	  BrickPi.SensorI2COut   [I2C_PORT][I2C_DEVICE_DGPS][0] = DGPS_CMD_VELO;	//byte to write
	  BrickPiSetupSensors();   
	  result = BrickPiUpdateValues(); //write and read
      if(!result){
        if(BrickPi.Sensor[I2C_PORT] & (0x01 << I2C_DEVICE_DGPS)){
		  velo = ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][0]<<16) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][1]<<8) + ((long)BrickPi.SensorI2CIn[I2C_PORT][I2C_DEVICE_DGPS][2]);        
        }
      }
	  printf("Status:%d UTC:%ld Latitude:%ld Longitude:%ld Heading:%d Velocity:%d\n",status,UTC,lat,lon,head,velo);
      usleep(500000);
    }
  }
  return 0;
}
