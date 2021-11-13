
/*******************************************************************************
 *******************************  Includes  ***********************************
 ******************************************************************************/
#include "mbed.h"
#include "MMA8451Q.h"
#include "TCS3472_I2C/TCS3472_I2C.h"
#include "MBed_Adafruit_GPS.h"

using namespace std::chrono;
BufferedSerial * gps_Serial;




/*******************************************************************************
 *******************************  Global Variables  ****************************
 ******************************************************************************/
 
extern uint32_t _rhData;
float rhData;
float auxrhData;
extern int32_t  _tData;
float tData;
float auxtData;
extern float light_value;
extern float moisture_value;
float result[3] = {0,0,0};
float x_value, y_value, z_value;
int rgb_readings[4]; // Declare a 4 element array to store RGB sensor readings

int counter = 0;

float sum_light = 0.0;
float min_light = 0.0;
bool light_0 = true;
float max_light = 0.0;
float mean_light = 0.0;

float sum_moisture = 0.0;
float min_moisture = 0.0;
bool moisture_0 = true;
float max_moisture = 0.0;
float mean_moisture = 0.0;

float sum_t = 0.0;
float min_t = 0.0;
bool t_0 = true;
float max_t = 0.0;
float mean_t = 0.0;

float sum_rh = 0.0;
float min_rh = 0.0;
bool rh_0 = true;
float max_rh = 0.0;
float mean_rh = 0.0;

float min_x = 0.0;
bool x_0 = true;
float max_x = 0.0;

float min_y = 0.0;
bool y_0 = true;
float max_y = 0.0;

float min_z = 0.0;
bool z_0 = true;
float max_z = 0.0;

int red_counter = 0;
int blue_counter = 0;
int green_counter = 0;



int wait_threads;
InterruptIn button(PB_2);
bool test = true;
bool change = false;
bool first_half = false;
bool second_half = false;
bool start_ticker = false;


//Show the sensed colour

DigitalOut Red(PH_0);
DigitalOut Green(PH_1);
DigitalOut Blue(PB_13);

DigitalOut test_mode(PB_5);
DigitalOut normal_mode(PA_5);

Thread AnalogThread(osPriorityNormal, 1024);
Thread I2CThread(osPriorityNormal, 1024);

Ticker half;

bool AnalogFinish;
bool I2CFinish;
bool GPSFinish;


bool RTHerror;
bool AccError;
bool RGBerror;
/*******************************************************************************
 *******************************     Global FUNCTIONS   ************************
 ******************************************************************************/

extern void readSensor(void);
extern bool RTHpresent();
extern void ReadADC(void);

void AnalogRead(void){
	while(true){
		ReadADC();
		AnalogFinish = true;
		wait_us(wait_threads);
	}
}

void I2CRead(void){
	TCS3472_I2C rgb_sensor (PB_9, PB_8);
	MMA8451Q acc(PB_9,PB_8,0x1c<<1);

	int present;
	while(true){
		//read of temperature and humidity first
			if(RTHpresent()){
					readSensor();
					RTHerror = false;
			}
			else
				RTHerror = true;
		//Now the accelerometer
			present = acc.getWhoAmI();
			if(present == 0x1A){
				acc.getAccAllAxis(result);
				AccError = false;
			}
			else
				AccError = true;
			//rgb sensor at last
				rgb_sensor.enablePowerAndRGBC();
				rgb_sensor.getAllColors(rgb_readings);
				RGBerror = false;
			
			I2CFinish = true;
			wait_us(wait_threads);
		  
	}
}

void change_mode(void){test = !test; change = false;}
void tickhalf_isr(void) {first_half = true;}



int main(void) {
	
	AnalogThread.start(AnalogRead);
	I2CThread.start(I2CRead);
	AnalogFinish = false;
	I2CFinish = false;
	
	button.mode(PullUp);
  button.fall(change_mode);
	
	  gps_Serial = new BufferedSerial(PA_9, PA_10,9600); //serial object for use w/ GPS
    Adafruit_GPS myGPS(gps_Serial); //object of Adafruit's GPS class
    char c; //when read via Adafruit_GPS::read(), the class returns single character stored here
    Timer refresh_Timer; //sets up a timer for use in loop; how often do we print GPS info?
    const int refresh_Time = 2000; //refresh time in ms
	  wait_threads = 2000000;

    myGPS.begin(9600);  //sets baud rate for GPS communication; note this may be changed via Adafruit_GPS::sendCommand(char *)
                        //a list of GPS commands is available at http://www.adafruit.com/datasheets/PMTK_A08.pdf

    myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGA); //these commands are defined in MBed_Adafruit_GPS.h; a link is provided there for command creation
    myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    myGPS.sendCommand(PGCMD_ANTENNA);

    printf("Connection established at 9600 baud...\r\n");
		


    

    refresh_Timer.start();  //starts the clock on the timer

	
	
  while (1){
		
		
		
		if(test && !change){	
			test_mode = 1;
			normal_mode = 0;
			wait_threads = 2000000;
			change = true;
			first_half = false;
			second_half = false;
			start_ticker = false;
			
			Red = 0;
			Green = 0;
			Blue = 0;
		}
		if(!test && !change){
			test_mode = 0;
			normal_mode = 1;
			wait_threads = 5000000;
			change = true;
			first_half = false;
			second_half = false;
			start_ticker = true;
			
			Red = 0;
			Green = 0;
			Blue = 0;

			sum_light = 0.0;
			min_light = 0.0;
			light_0 = true;
			max_light = 0.0;
			mean_light = 0.0;
			
			sum_moisture = 0.0;
			min_moisture = 0.0;
			moisture_0 = true;
			max_moisture = 0.0;
			mean_moisture = 0.0;
			
			sum_t = 0.0;
			min_t = 0.0;
			t_0 = true;
			max_t = 0.0;
			mean_t = 0.0;

			sum_rh = 0.0;
			min_rh = 0.0;
			rh_0 = true;
			max_rh = 0.0;
			mean_rh = 0.0;

			min_x = 0.0;
			x_0 = true;
			max_x = 0.0;

			min_y = 0.0;
			y_0 = true;
			max_y = 0.0;

			min_z = 0.0;
			z_0 = true;
			max_z = 0.0;	
			
			red_counter = 0;
			blue_counter = 0;
			green_counter = 0;
			
			counter = 0;
		}
		
		if(test && (first_half || second_half)){first_half = false; second_half = false;}
		
		if(!test && first_half && !second_half){
			first_half = false;
			second_half = true;
			half.attach_us(tickhalf_isr, 10000000);
			printf("TERMINEEEEEEEEEEEEEEEEEEE 1 \n");
		}
		
				
		if(!test && first_half && second_half){
			first_half = false;
			second_half = false;
			half.attach_us(tickhalf_isr, 10000000);
			
			mean_light = sum_light / counter;
			mean_moisture = sum_moisture / counter;
			mean_t = sum_t /counter;
			mean_rh = sum_rh / counter;
			
			printf("RESULTS AFTER AN HOUR WITH %i VALUES TAKEN: \n", counter);
			printf("SOIL MOISTURE: MEAN: %.2f, MAX_VALUE: %.2f, MIN_VALUE: %.2f\n", mean_moisture, max_moisture, min_moisture);
			printf("LIGHT: MEAN: %.2f, MAX_VALUE: %.2f, MIN_VALUE: %.2f\n", mean_light, max_light, min_light);
			printf("TEMP/HUM -");
			printf("Temperature: MEAN: %.3f, MAX_VALUE: %.3f, MIN_VALUE: %.3f\n", mean_t, max_t, min_t);
			printf("Relative Humidity: MEAN: %.3f, MAX_VALUE: %.3f, MIN_VALUE: %.3f\n", mean_rh, max_rh, min_rh);
			printf("ROTATION: X_max= %.2f\t X_min= %.2f\t Y_max= %.2f\t Y_min= %.2f\t Z_max= %.2f\t Z_min= %.2f \n",
					max_x, min_x, max_y, min_y, max_z, min_z);
			if(red_counter>blue_counter && red_counter>green_counter){
				printf("RED IS THE COLOR MOST SAW, %i TIMES", red_counter);
			}
			if(blue_counter>red_counter && blue_counter>green_counter){
				printf("BLUE IS THE COLOR MOST SAW, %i TIMES", blue_counter);
			}
			if(green_counter>red_counter && green_counter>blue_counter){
				printf("GREEN IS THE COLOR MOST SAW, %i TIMES", green_counter);
			}
			
			printf("\n \n");
			
			
			
			sum_light = 0.0;
			min_light = 0.0;
			light_0 = true;
			max_light = 0.0;
			mean_light = 0.0;
			
			sum_moisture = 0.0;
			min_moisture = 0.0;
			moisture_0 = true;
			max_moisture = 0.0;
			mean_moisture = 0.0;
			
			sum_t = 0.0;
			min_t = 0.0;
			t_0 = true;
			max_t = 0.0;
			mean_t = 0.0;

			sum_rh = 0.0;
			min_rh = 0.0;
			rh_0 = true;
			max_rh = 0.0;
			mean_rh = 0.0;

			min_x = 0.0;
			x_0 = true;
			max_x = 0.0;

			min_y = 0.0;
			y_0 = true;
			max_y = 0.0;

			min_z = 0.0;
			z_0 = true;
			max_z = 0.0;
			
			red_counter = 0;
			blue_counter = 0;
			green_counter = 0;
			
			
			counter = 0;
		}	
		
	
			if(AnalogFinish && I2CFinish){
				AnalogFinish = false;
				I2CFinish = false;
				
		
				
				printf("SOIL MOISTURE: %.2f\n", moisture_value);
				printf("LIGHT: %.2f\n", light_value);
				
				
				c = myGPS.read();   //queries the GPS

//        if (c) { printf("%c", c); } //this line will echo the GPS data if not paused

        //check if we recieved a new message from GPS, if so, attempt to parse it,
        if ( myGPS.newNMEAreceived() ) {
            if ( !myGPS.parse(myGPS.lastNMEA()) ) {
                continue;
            }
        }

        //check if enough time has passed to warrant printing GPS info to screen
        //note if refresh_Time is too low or pc.baud is too low, GPS data may be lost during printing
        if (duration_cast<milliseconds>(refresh_Timer.elapsed_time()).count() >= refresh_Time) {
        //if (refresh_Timer.read_ms() >= refresh_Time) {
            refresh_Timer.reset();
					
					printf("GPS - #Sats: %d\r", myGPS.satellites);
            if ((int)myGPS.fixquality > 0) {
                printf("Lat(VTC): %5.2f %c\r", myGPS.latitude, myGPS.lat);
							  printf("Long(VTC): %5.2f %c\r", myGPS.longitude, myGPS.lon);
                printf("Altitude: %5.2f m\r", myGPS.altitude);
            }else
						printf("NOT ENOUGH QUALITY TO GET LOCATION ");
						
						printf("GPS Time: %d:%d:%d.%u\r\n", myGPS.hour + 1, myGPS.minute, myGPS.seconds, myGPS.milliseconds);
        }
				
					if(!RTHerror){		
						printf("TEMP/HUM -");
						tData = _tData/1000;
						auxtData = _tData % 1000;
						if(auxtData > 100){
							tData = tData + (auxtData/1000);
						}
						else{
							if(auxtData > 10){
								tData = tData + (auxtData/100);
							}
							else{
								
								tData = tData + (auxtData/10);						
													 
							}
						}
						printf("Temperature: %.3f oC ", tData);
						
						rhData = _rhData/1000;
						auxrhData = _rhData % 1000;
						if(auxrhData > 100){
							rhData = rhData + (auxrhData/1000);
						}
						else{
							if(auxrhData > 10){
								rhData = rhData + (auxrhData/100);
							}
							else{
								
								rhData = rhData + (auxrhData/10);						
													 
							}
						}
						printf("Relative Humidity: %.3f % \n", rhData);
				
					}
					else
				printf("**********PLEASE CONNECT THE RTH SENSOR***********\n");
					
				if(!AccError){
					x_value = result[0] * 10;
					y_value = result[1] * 10;
					z_value = result[2] * 10;
					printf("ROTATION: X_axis= %.2f\t Y_axis= %.2f\t Z_axis= %.2f \n",
					x_value, y_value, z_value);
				}else
				printf("**********PLEASE CONNECT ACCELEROMETER*********\n");
				
				if(test){
				
					if(!RGBerror){
						printf("COLOR SENSOR: Clear: %d, Red: %d, Green: %d, Blue: %d\n",rgb_readings[0],rgb_readings[1],rgb_readings[2],rgb_readings[3]);
								if(rgb_readings[0]>6000)
							{
									Red    = 0;
									Green  = 0;
									Blue   = 0;		
							}
							else
								{
									if(rgb_readings[1]<100 && rgb_readings[2]<100 && rgb_readings[3]<100) //If low: none
										{
											Red    = 1;
											Green  = 1;
											Blue   = 1;
										}
										else
											{
											if(rgb_readings[1]>rgb_readings[2] && rgb_readings[1]>=rgb_readings[3]) //If max=RED
												{
													Red    = 0;
													Green  = 1;
													Blue   = 1;
												}else if(rgb_readings[2]>rgb_readings[1] && rgb_readings[2]>rgb_readings[3]) //If max=Green
												{
													Red    = 1;
													Green  = 0;
													Blue   = 1;
												}
												else if(rgb_readings[3]>rgb_readings[1] && rgb_readings[3]>rgb_readings[2])   //If max=Blue
												{
													Red    = 1;
													Green  = 1;
													Blue   = 0;						
												}
											}
								}
					}else 
						printf("Please Connect RGB sensor");
				}
				
				
				
							
				
				if(start_ticker){
					start_ticker = false;
					half.attach_us(tickhalf_isr, 5000000);
				}
				
				if(!test && !start_ticker){
					counter = counter + 1;
					sum_light = sum_light + light_value;
					sum_moisture = sum_moisture + moisture_value;
					sum_t = sum_t + tData;
					sum_rh = sum_rh + rhData;
					if(light_value < min_light || light_0){min_light = light_value; light_0 = false;}	
					if(light_value > max_light){max_light = light_value;}	
					if(moisture_value < min_moisture || moisture_0){min_moisture = moisture_value; moisture_0 = false;}	
					if(moisture_value > max_moisture){max_moisture = moisture_value;}	
					if(tData < min_t || t_0){min_t = tData; t_0 = false;}	
					if(tData > max_t){max_t = tData;}
					if(rhData < min_rh || rh_0){min_rh = rhData; rh_0 = false;}	
					if(rhData > max_rh){max_rh = rhData;}
					
					if(x_value < min_x || x_0){min_x = x_value; x_0 = false;}	
					if(x_value > max_x){max_x = x_value;}
					if(y_value < min_y || y_0){min_y = y_value; y_0 = false;}	
					if(y_value > max_y){max_y = y_value;}
					if(z_value < min_z || z_0){min_z = z_value; z_0 = false;}	
					if(z_value > max_z){max_z = z_value;}
					
					if(!RGBerror){
						printf("COLOR SENSOR: Clear: %d, Red: %d, Green: %d, Blue: %d\n",rgb_readings[0],rgb_readings[1],rgb_readings[2],rgb_readings[3]);
								if(rgb_readings[0]>6000)
							{
								
							}
							else
								{
									if(rgb_readings[1]<100 && rgb_readings[2]<100 && rgb_readings[3]<100) //If low: none
										{
										
										}
										else
											{
											if(rgb_readings[1]>rgb_readings[2] && rgb_readings[1]>=rgb_readings[3]) //If max=RED
												{
													red_counter = red_counter + 1;
												}else if(rgb_readings[2]>rgb_readings[1] && rgb_readings[2]>rgb_readings[3]) //If max=Green
												{
													green_counter = green_counter + 1;
												}
												else if(rgb_readings[3]>rgb_readings[1] && rgb_readings[3]>rgb_readings[2])   //If max=Blue
												{
													blue_counter = blue_counter + 1;						
												}
											}
								}
					}else 
						printf("Please Connect RGB sensor");
					
				}
				
				printf("\n");

			}

	
	}
}
