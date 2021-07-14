#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "Westaby-Private";
const char* password = "nightnightwisconsin";

#define SLOW_TIMING 400
#define FAST_TIMING 100

#define BUTTON_HOLD_DELAY_IN_MILLIS_UNTIL_HELD_ACTION     2000 // 2 seconds

#define BUTTON_PIN D3
#define BRAKE_PIN  D6
 
#define NEOPIN D5
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16
 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIN);

/* ***************** */
//mode info

enum animations_e
{
  /* order of animations list */
  FLASHING_SLOW_RED = 0,
  FLASHING_FAST_RED,
  SOLID_RED,
  
  FLASHING_SLOW_GREEN,
  FLASHING_FAST_GREEN,
  SOLID_GREEN,
  
  OFF,
  
  /* keep at end of list */
  NUMBER_OF_ANIMATIONS,
  
  GRUNT_BDAY_PARTY, //secret
};

byte animation_mode = 0;

bool brake_flag = false;

bool wifi_connected = false;

/* ***************** */
//button routines
typedef enum switch_status { PROCESSED, NOT_PROCESSED, };
typedef enum switch_state  { IS_NOT_PRESSED, IS_PRESSED, WAS_RELEASED, IS_HELD, };
typedef enum input_type    { ACTIVE_HIGH, ACTIVE_LOW, };

enum button_link
{
  /* order of button_info list */
  PUSHBUTTON = 0,
  //BRAKE_SIGNAL, // process in real time
  /* keep at end of list */
  NUMBER_OF_BUTTONS,
};

typedef struct
{
  byte          pin;
  input_type    in_type;
  switch_state  state;
  switch_status status;
  long          last_interaction_timestamp;
  byte          last_button_read;
  byte          current_button_read;
} button_struct;

button_struct button_info[NUMBER_OF_BUTTONS] =
{
  { /* button_info[PUSHBUTTON] */
    /* pin       = */ BUTTON_PIN,
    /* in_type   = */ ACTIVE_LOW,
    /* state     = */ IS_NOT_PRESSED,
    /* status    = */ NOT_PROCESSED,
    /* timestamp = */ 0,
    /* last      = */ false,
    /* current   = */ false,
  },
};

/* ***************** */

void setup() 
{
  //setup output to serial monitor
  Serial.begin(115200);
  
  // initialize the button pins as an input:
  for (byte i = 0; i < NUMBER_OF_BUTTONS; i++)
  {
    if (button_info[i].in_type == ACTIVE_HIGH)
    {
      pinMode(button_info[i].pin, INPUT); //special pin, Active = 3.3V
    }
    else
    {
      pinMode(button_info[i].pin, INPUT_PULLUP);
    }
  }

  pinMode(BRAKE_PIN, INPUT); //no pullup, is active high

  pixels.begin();
  set_idle_brightness();
  
  Serial.println(F("Bike Light v1.0"));

}

void loop() 
{
  poll_brake_signal_for_brightness();
  poll_input_signals();
  process_button_presses();
  process_animation();
  //TBD, brake should override the animation, not directly change the mode
}

void poll_brake_signal_for_brightness()
{
  static bool last_flag = false;
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (3.2 / 1023.0);

  //Serial.print("Brake: ");
  //Serial.println(voltage);

  if (voltage < 0.1f) //check for active low signal (not strong enough for digital input)
  {
    brake_flag = true;
  }
  else
  {
    brake_flag = false;
  }

  if(brake_flag != last_flag) //change brightness only on state changes
  {
    last_flag = brake_flag;
    if(brake_flag) set_high_brightness();
    else set_idle_brightness();
  }
}

void start_wifi()
{
  uint32_t timeout = millis();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while ( ( (timeout + 5000) > millis() ) && 
          ( WiFi.waitForConnectResult() != WL_CONNECTED) );
  
  if(WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    wifi_connected = false;
    Serial.println("Connection Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
  else
  {
    wifi_connected = true;
  }

  if(wifi_connected)
  {
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Wifi Not Connected");
  }
}

//--------------------------------------
//          Mode Subroutines           |
//--------------------------------------

void next_animation()
{
  if ((animation_mode + 1) < NUMBER_OF_ANIMATIONS)
  {
    animation_mode++;
  }
  else
  {
    animation_mode = 0;
  }
  
  Serial.print("Mode: ");
  Serial.println(animation_mode);
}

void secret_animation()
{
  //hold to enter / exit
  if(animation_mode != GRUNT_BDAY_PARTY)
  {
    start_wifi();
    animation_mode = GRUNT_BDAY_PARTY;
  }
  else
  {
    animation_mode = 0; 
  }
  
  Serial.print("Mode: ");
  Serial.println(animation_mode);
}

//--------------------------------------
//        Neopixel Subroutines         |
//--------------------------------------

void process_animation()
{
  if (brake_flag) //BRAKE !!
  {
    switch (animation_mode)
    {
      case FLASHING_SLOW_RED:
      case FLASHING_FAST_RED:
      case SOLID_RED:
      case OFF:
        fill_ring(pixels.Color(255,0,0)); //red
        break;
        
      case FLASHING_SLOW_GREEN:
      case FLASHING_FAST_GREEN:
      case SOLID_GREEN:
        fill_ring(pixels.Color(0,255,0)); //red
        break;
    
      case GRUNT_BDAY_PARTY:
        rainbow(1);
        if(wifi_connected)
        {
          ArduinoOTA.handle();
        }
        break;
      default:
        break;
    }
  }
  else //NO BRAKE, NORMAL ANIMATION
  {
    switch (animation_mode)
    {
      case FLASHING_SLOW_RED:
        flash_animation(SLOW_TIMING, pixels.Color(255,0,0));
        break;
      case FLASHING_FAST_RED:
        flash_animation(FAST_TIMING, pixels.Color(255,0,0));
        break;
      case SOLID_RED:
        fill_ring(pixels.Color(255,0,0)); //red
        break;
        
      case FLASHING_SLOW_GREEN:
        flash_animation(SLOW_TIMING, pixels.Color(0,255,0));
        break;
      case FLASHING_FAST_GREEN:
        flash_animation(FAST_TIMING, pixels.Color(0,255,0));
        break;
      case SOLID_GREEN:
        fill_ring(pixels.Color(0,255,0)); //red
        break;

      case OFF:
        fill_ring(pixels.Color(0,0,0)); //black / off
        break;
      case GRUNT_BDAY_PARTY:
        rainbow(1);
        if(wifi_connected)
        {
          ArduinoOTA.handle();
        }
        break;
      default:
        Serial.print("Unknown Mode: ");
        Serial.println(animation_mode);
        break;
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) 
{
  static long firstPixelHue = 0;
  static int i = 0;
  static uint32_t last_update = 0;

  if (last_update + wait < millis())
  {
    last_update = millis();
    
    //for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) 
    {
      //for(int i=0; i<pixels.numPixels(); i++) 
      {
        int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
        pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
        
        if (i<pixels.numPixels())
        {
          i++;
        }
        else
        {
          i = 0;
        }
      }
      pixels.show();
      
      if (firstPixelHue < 3*65536)
      {
        firstPixelHue += 256;
      }
      else
      {
        firstPixelHue = 0;
      }
    }
  }
}

void flash_animation(uint32_t time_delay, uint32_t color)
{
  static uint32_t last_update = 0;
  static bool flash_state = LOW;

  if (last_update + time_delay < millis())
  {
    last_update = millis();

    flash_state = !flash_state;

    if (flash_state == LOW)
    {
      fill_ring(pixels.Color(0,0,0)); //black / off
    }
    else
    {
      fill_ring(color);
    }
  }
}

void set_high_brightness()
{
 pixels.setBrightness(255);
}

void set_idle_brightness()
{
 pixels.setBrightness(64);
}

void fill_ring(uint32_t color)
{
 for(int i=0;i<NUMPIXELS;i++)
 {
   pixels.setPixelColor(i, color);
 }
   pixels.show();
}

//--------------------------------------
//          Button Subroutines         |
//--------------------------------------

void process_button_presses()
{
  for (byte i = 0; i < NUMBER_OF_BUTTONS; i++)
  {
    if (button_info[i].state == WAS_RELEASED)
    {
      //CLICKED ACTION

      //Note: action taken on WAS_RELEASED instead of IS_PRESSED to prevent extra clicks during button holds

      button_info[i].last_interaction_timestamp = millis();

      if (button_info[i].status == NOT_PROCESSED)
      {
        //one action per press / hold for all buttons
        for (byte j = 0; j < NUMBER_OF_BUTTONS; j++)
        {
          button_info[j].status = PROCESSED;
        }

        //button was clicked
        switch (i)
        {
          case PUSHBUTTON:
            next_animation();
            break;
          default:
            break;
        }
      }
    }
    else if ( ( button_info[i].state == IS_HELD ) &&
              ( button_info[i].last_interaction_timestamp + BUTTON_HOLD_DELAY_IN_MILLIS_UNTIL_HELD_ACTION < millis() ) )
    {
      //HELD ACTION

      button_info[i].last_interaction_timestamp = millis();

      if (button_info[i].status == NOT_PROCESSED)
      {
        //one action per press / hold for all buttons
        for (byte j = 0; j < NUMBER_OF_BUTTONS; j++)
        {
          button_info[j].status = PROCESSED;
        } //TBD, move this to a routine

        //button was held
        switch (i)
        {
          case PUSHBUTTON:
            secret_animation();
            break;
          default:
            break;
        }
      }
    }
    else if (button_info[i].state == IS_NOT_PRESSED)
    {
      //reset timers
      button_info[i].last_interaction_timestamp = millis();
      button_info[i].status = NOT_PROCESSED;
    }

  }// end for loop
}

void poll_input_signals()
{
  for (byte i = 0; i < NUMBER_OF_BUTTONS; i++)
  {
    if ( button_info[i].in_type == ACTIVE_HIGH )
    {
      /* read digital active high beam signal */
      button_info[i].current_button_read = digitalRead(button_info[i].pin);
    }
    else // button_info[i].in_type == ACTIVE_LOW
    {
      /* read digital active low button signal */
      button_info[i].current_button_read = !digitalRead(button_info[i].pin);
    }

    if (!button_info[i].last_button_read && button_info[i].current_button_read)
    {
      //button was just pressed
      button_info[i].state = IS_PRESSED;
    }
    else if  (button_info[i].last_button_read && button_info[i].current_button_read)
    {
      button_info[i].state = IS_HELD;
    }
    else if (button_info[i].last_button_read && !button_info[i].current_button_read)
    {
      //button was just released
      button_info[i].state = WAS_RELEASED;
    }
    else
    {
      button_info[i].state = IS_NOT_PRESSED;
    }

    button_info[i].last_button_read = button_info[i].current_button_read;
  }
}
