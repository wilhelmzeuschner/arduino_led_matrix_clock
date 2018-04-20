//Wilhelm Zeuschner - Project started: 14.03.2018
//I'd recommend changing line 217 in "MD_MAX72xx_font.cpp" to "  5, 0x00, 0x42, 0x7f, 0x40, 0x00,	// 49 - '1'". This ensures even character spacing.


//Libraries
#include <RtcDS3231.h>
#include <MD_MAX72xx.h>
#include <DHT.h>

#include <Wire.h>
#include <SPI.h>

//Defines
#define	MAX_DEVICES	5	//Number of chained Martices
#define	CLK_PIN		13  // SPI SCK
#define	DATA_PIN	11  // SPI MOSI
#define	CS_PIN		10  // SPI SS

#define DHTPIN 9
#define DHTTYPE DHT11
//Text Parameters
#define  CHAR_SPACING  1 // pixels between characters
//Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	10


MD_MAX72XX matrix = MD_MAX72XX(CS_PIN, MAX_DEVICES);	//Create Matrix Instance
RtcDS3231<TwoWire> rtc(Wire);							//Create RTC Instance
DHT dht(DHTPIN, DHTTYPE);								//Create DHT11 Instance

//Pins
const byte sensor = 3;

//Variables
RtcDateTime datetime;								//Stores Time
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int hour;
unsigned long last_time;							//For Timing

volatile bool touch_flag;
bool display_mode;
volatile unsigned long time_between;
volatile bool trigger_state;
bool working_on_it = 0;


char message[BUF_SIZE] = { " " };
bool newMessageAvailable = true;


void setup() {
	pinMode(sensor, INPUT);
	attachInterrupt(digitalPinToInterrupt(sensor), touch, RISING);

	matrix.begin();
	rtc.Begin();
	
	dht.begin();
	Serial.begin(115200);
}

void loop() {

	if ((millis() - last_time) >= 1000) {
		last_time = millis();
		datetime = rtc.GetDateTime();
		hour = datetime.Hour();
		setBrightness();

		int lh = ((hour / 10) % 10);
		int rh = (hour % 10);
		int lm = ((datetime.Minute() / 10) % 10);
		int rm = (datetime.Minute() % 10);
		int ls = ((datetime.Second() / 10) % 10);
		int rs = (datetime.Second() % 10);




		//Serial.println(hour);

		message[0] = char(lh) + 48;
		message[1] = char(rh) + 48;
		message[2] = ':';
		message[3] = char(lm) + 48;
		message[4] = char(rm) + 48;
		message[5] = ':';
		message[6] = char(ls) + 48;
		message[7] = char(rs) + 48;

		printText(0, MAX_DEVICES - 1, message);
		

		//Serial.println(daysOfTheWeek[datetime.DayOfWeek()]);
	}

	//Serial.println("trigger_state");
	//Serial.println(trigger_state);
	//Serial.println("time_between");
	//Serial.println(time_between);
	if (touch_flag) {

		unsigned long trigger_time = millis();
		working_on_it = 1;
		touch_flag = 0;
		bool delayed = 0;
		display_mode = !display_mode;

		//Display Temperature
		if (display_mode) {

			int temperature = dht.readTemperature();
			int humidity = dht.readHumidity();
			/*Serial.println(temperature);
			Serial.println(humidity);*/
			
			//RtcTemperature temp = rtc.GetTemperature();
			//int temperature = temp.AsFloat() * 100;


			int t1 = ((temperature / 10) % 10);
			int t2 = (temperature % 10);
			int h1 = ((humidity / 10) % 10);
			int h2 = (humidity % 10);

			message[0] = char(t1) + 48;
			message[1] = char(t2) + 48;
			message[2] = 'C';
			message[3] = ' ';			
			message[4] = char(h1) + 48;
			message[5] = char(h2) + 48;
			message[6] = '%';
			message[7] = ' ';
			printText(0, MAX_DEVICES - 1, message);
			/*Serial.print(temperature);
			Serial.println("C");
			Serial.print(humidity);
			Serial.println("%");*/
		}

		//Display current Date and Day
		else {
			//Print Day, then Date
			printText(0, MAX_DEVICES - 1, daysOfTheWeek[datetime.DayOfWeek()]);			
			delay(900);

			int ld = ((datetime.Day() / 10) % 10);
			int rd = (datetime.Day() % 10);
			int lm = ((datetime.Month() / 10) % 10);
			int rm = (datetime.Month() % 10);
			int ly = ((datetime.Year() / 10) % 10);
			int ry = (datetime.Year() % 10);

			//Serial.println(ly);
			//Serial.println(ry);

			message[0] = char(ld) + 48;
			message[1] = char(rd) + 48;
			message[2] = '.';
			message[3] = char(lm) + 48;
			message[4] = char(rm) + 48;
			message[5] = '.';
			message[6] = char(ly) + 48;
			message[7] = char(ry) + 48;

			printText(0, MAX_DEVICES - 1, message);
		}

		//Delay Logic
		while (true) {
			if (!delayed) {				
				delayed = 1;
				delay(1500);
			}

			if (digitalRead(sensor) == 0 || (millis() - trigger_time) > 10000) {		//Quit, if sensor is no longer pressed or if it has been pressed for over 10s
				working_on_it = 0;
				break;
			}
		}
	}
}

void touch() {
	if (!working_on_it) {
		touch_flag = 1;
	}	
}

//Determines the Brightness for Matrices
void setBrightness() {
	//Dim Light Intensity (0x1 to 0xf)
	if ((hour > 20) && (hour < 21)) {				//Medium Brightness
		matrix.control(MD_MAX72XX::INTENSITY, 0x3);
		//Serial.println("Case 1");
	}
	else if ((hour > 21) || (hour < 7)) {			//Low Brightness
		matrix.control(MD_MAX72XX::INTENSITY, 0x1);
		//Serial.println("Case 2");
	}
	else /*if ((hour < 20) || (hour > 7))*/ {		//Normal Brightness
		matrix.control(MD_MAX72XX::INTENSITY, 0xf);
		//Serial.println("Case 3");
	}
}

//Prints Text to Matrices
void printText(uint8_t modStart, uint8_t modEnd, char *pMsg) {
	uint8_t		state = 0;
	uint8_t		curLen;
	uint16_t	showLen;
	uint8_t		cBuf[8];
	int16_t		col = ((modEnd + 1) * COL_SIZE) - 1;

	matrix.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

	do     // finite state machine to print the characters in the space available
	{
		switch (state)
		{
		case 0: // Load the next character from the font table
				// if we reached end of message, reset the message pointer
			if (*pMsg == '\0')
			{
				showLen = col - (modEnd * COL_SIZE);  // padding characters
				state = 2;
				break;
			}

			// retrieve the next character form the font file
			showLen = matrix.getChar(*pMsg++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
			curLen = 0;
			state++;
			// !! deliberately fall through to next state to start displaying

		case 1: // display the next part of the character
			matrix.setColumn(col--, cBuf[curLen++]);

			// done with font character, now display the space between chars
			if (curLen == showLen)
			{
				showLen = CHAR_SPACING;
				state = 2;
			}
			break;

		case 2: // initialize state for displaying empty columns
			curLen = 0;
			state++;
			// fall through

		case 3: // display inter-character spacing or end of message padding (blank columns)
			matrix.setColumn(col--, 0);
			curLen++;
			if (curLen == showLen)
				state = 0;
			break;

		default:
			col = -1;   // this definitely ends the do loop
		}
	} while (col >= (modStart * COL_SIZE));

	matrix.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}
