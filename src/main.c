#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"
#include "util.h"
#include "weather_layer.h"
#include "time_layer.h"
#include "link_monitor.h"
#include "config.h"

	
#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }

PBL_APP_INFO(MY_UUID,
             "Node Weather", "Roost", // Modification of "Roboto Weather" by Martin Rosinski
             1, 06, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);


// POST variables
#define WEATHER_KEY_LATITUDE 1
#define WEATHER_KEY_LONGITUDE 2
#define WEATHER_KEY_UNIT_SYSTEM 3
	
// Received variables
#define WEATHER_KEY_ICON 1
#define WEATHER_KEY_TEMPERATURE 2
#define WEATHER_KEY_HIGH 3
#define WEATHER_KEY_LOW 4
#define WEATHER_KEY_LTC 5	
#define WEATHER_KEY_BTC 6
	
//Cookie variables
#define WEATHER_HTTP_COOKIE 1949327671
	
//#define TIME_HTTP_COOKIE 1131038282
	
/*GRect works like this - LEFT TOP WIDE HIGH*/
#define TIME_FRAME      (GRect(0, 52, 144, 55))
#define DATE_FRAME      (GRect(0, 108, 144, 30))
	
Window window;          /* main window */
BmpContainer background_image;

TextLayer date_layer;   /* layer for the date */
TimeLayer time_layer;   /* layer for the time */

GFont font_date;        /* font for date */
GFont font_hour;        /* font for hour */



//Weather Stuff
static int our_latitude, our_longitude;
static bool located = false;

WeatherLayer weather_layer;

void request_weather();
void handle_timer(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie);

/*FAILED REQUEST OF WEATHER EITHER NO CONNECTION OR HTTP ERROR
***************************************************************/
void failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0 || cookie == WEATHER_HTTP_COOKIE) {		
//		weather_layer_set_icon(&weather_layer, WEATHER_ICON_NO_WEATHER);
//		text_layer_set_text(&weather_layer.temp_layer, "---°   ");
		layer_mark_dirty(&weather_layer.layer);
	}
	
	link_monitor_handle_failure(http_status);
	
	//Re-request the location and subsequently weather on next minute tick
	located = false;
}

/*IF SUCCESSFUL CONNECTION THEN DO THE FOLLOWING
***************************************************/
void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if(cookie != WEATHER_HTTP_COOKIE) return;
/*
GET THE WEATHER ICON
***************************/
	Tuple* icon_tuple = dict_find(received, WEATHER_KEY_ICON);
	if(icon_tuple) {
		int icon = icon_tuple->value->int8;
		if(icon >= 0 && icon < 10) {
			weather_layer_set_icon(&weather_layer, icon);
		} else {
			//weather_layer_set_icon(&weather_layer, WEATHER_ICON_NO_WEATHER);
			text_layer_set_text(&weather_layer.temp_layer, "???");
		}
	}
/*
GET THE TEMPERATURE
***************************/	
	Tuple* temperature_tuple = dict_find(received, WEATHER_KEY_TEMPERATURE);
	if(temperature_tuple) {
		
		text_layer_set_text(&weather_layer.temp_layer, "0");
		weather_layer_set_temperature(&weather_layer, 1);
		weather_layer_set_temperature(&weather_layer, temperature_tuple->value->int16);
		
	}
/*
GET THE HIGH TEMP
***************************/
	Tuple* high_tuple = dict_find(received, WEATHER_KEY_HIGH);
	if(high_tuple) {
		weather_layer_set_high(&weather_layer, high_tuple->value->int16);
	}
/*
GET THE LOW TEMP
***************************/	
	Tuple* low_tuple = dict_find(received, WEATHER_KEY_LOW);
	if(low_tuple) {
		weather_layer_set_low(&weather_layer, low_tuple->value->int16);
	}
/*
GET THE BTC PRICE
***************************/
	Tuple* btc_tuple = dict_find(received, WEATHER_KEY_BTC);
	if(btc_tuple) {
		weather_layer_set_btc(&weather_layer, btc_tuple->value->int16);
	}
/*
GET THE LTC PRICE
***************************/
	Tuple* ltc_tuple = dict_find(received, WEATHER_KEY_LTC);
	if(ltc_tuple) {
		weather_layer_set_ltc(&weather_layer, ltc_tuple->value->int16);
	}
	
	
	link_monitor_handle_success();
}

void set_timer(AppContextRef ctx) {
	app_timer_send_event(ctx, 1740000, 1);
}

/*GET OUR LOCATION AND * BY 10000 TO GET RID OF THE DECIMAL
*************************************************************************/
void location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 10000;
	our_longitude = longitude * 10000;
	located = true;
	request_weather();
}
/*RECONNECT IF CONNECTION LOST
************************************/
void reconnect(void* context) {
	request_weather();
}

void request_weather();

/* Called by the OS once per minute. Update the time and date.
*/
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
    /* Need to be static because pointers to them are stored in the text
    * layers.
    */

    static char hour_text[] = "00";
    static char minute_text[] = ":00";

    static char date_day[] = "XXX";
    static char date_monthday[] = "00";
    static char date_month[] = "XXX";
    
    static char full_date_text[20] = "";

    (void)ctx;  /* prevent "unused parameter" warning */

    if (t->units_changed & DAY_UNIT) {		

	    string_format_time(date_day,
                           sizeof(date_day),
                           "%a",
                           t->tick_time);

	    string_format_time(date_monthday,
                           sizeof(date_monthday),
                           "%d",
                           t->tick_time);

	    string_format_time(date_month,
                           sizeof(date_month),
                           "%b",
                           t->tick_time);

      snprintf(full_date_text, sizeof(full_date_text), "%s %s %s", date_day, date_monthday, date_month); 
      text_layer_set_text(&date_layer, full_date_text);
    }

    if (clock_is_24h_style()) {
      string_format_time(hour_text, sizeof(hour_text), "%H", t->tick_time);
		  if (hour_text[0] == '0') {
      }
    }
    else {
        string_format_time(hour_text, sizeof(hour_text), "%I", t->tick_time);
        if (hour_text[0] == '0') {
        }
    }

    string_format_time(minute_text, sizeof(minute_text), ":%M", t->tick_time);
    time_layer_set_text(&time_layer, hour_text, minute_text);
	
	if(!located || !(t->tick_time->tm_min % 15))
	{
		//Every 15 minutes, request updated weather
		http_location_request();
	}
	else
	{
		//Every minute, ping the phone
		link_monitor_ping();
	}
}
/* Initialize the application.
*/
void handle_init(AppContextRef ctx) {
  PblTm tm;
  PebbleTickEvent t;
  ResHandle res_d;
  ResHandle res_h;
  located=false;
  request_weather();

  window_init(&window, "Node");
  window_stack_push(&window, true /* Animated */);
  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
  layer_add_child(&window.layer, &background_image.layer.layer);
/* TIME AND DATE FONTS*/
  res_d = resource_get_handle(RESOURCE_ID_DATE_19);
  res_h = resource_get_handle(RESOURCE_ID_TIME_52);

  font_date = fonts_load_custom_font(res_d);
  font_hour = fonts_load_custom_font(res_h);

	
	layer_mark_dirty(&weather_layer.layer);
  time_layer_init(&time_layer, window.layer.frame);
  time_layer_set_text_color(&time_layer, GColorBlack);
    time_layer_set_text_color(&time_layer, GColorWhite);
    time_layer_set_background_color(&time_layer, GColorClear);
  time_layer_set_fonts(&time_layer, font_hour, font_hour);
  layer_set_frame(&time_layer.layer, TIME_FRAME);
  layer_add_child(&window.layer, &time_layer.layer);

  text_layer_init(&date_layer, window.layer.frame);
  text_layer_set_text_color(&date_layer, GColorBlack);
    text_layer_set_text_color(&date_layer, GColorWhite);
    text_layer_set_background_color(&date_layer, GColorClear);
  text_layer_set_font(&date_layer, font_date);
  text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
  layer_set_frame(&date_layer.layer, DATE_FRAME);
  layer_add_child(&window.layer, &date_layer.layer);

	// Add weather layer
	weather_layer_init(&weather_layer, GPoint(0, 90)); //0, 100
	layer_add_child(&window.layer, &weather_layer.layer);
	
	http_register_callbacks((HTTPCallbacks){
		.failure=failed,
		.success=success,
		.reconnect=reconnect,
		.location=location
	}, (void*)ctx);

	// Request weather
	located = false;
	request_weather();
	
	// Refresh time
	get_time(&tm);
    t.tick_time = &tm;
    t.units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;
	handle_minute_tick(ctx, &t);
}

/* Shut down the application
*/
void handle_deinit(AppContextRef ctx)
{
    fonts_unload_custom_font(font_date);
    fonts_unload_custom_font(font_hour);
    bmp_deinit_container(&background_image);
	  weather_layer_deinit(&weather_layer);
}


/********************* Main Program *******************/

void pbl_main(void *params)
{
    PebbleAppHandlers handlers =
    {
      .init_handler = &handle_init,
      .deinit_handler = &handle_deinit,
      .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
      },
.timer_handler = handle_timer,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 124,
				.outbound = 256,
			}
		}
    };

    app_event_loop(params, &handlers);
}

void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
	request_weather();
	// Update again in fifteen minutes.
	if(cookie)
		set_timer(ctx);
}

void request_weather() {
	if(!located) {
		http_location_request();
		return;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	//ENTER YOUR PHP LOCATION BELOW
	// Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get("PUT YOUR LINK TO PHP HERE IE HTTP://SOMEWHERE.COM/INDEX.PHP", WEATHER_HTTP_COOKIE, &body);
	if(result != HTTP_OK) {
		weather_layer_set_icon(&weather_layer, WEATHER_ICON_HTTP_ERROR);
		return;
	}
	dict_write_int32(body, WEATHER_KEY_LATITUDE, our_latitude);
	dict_write_int32(body, WEATHER_KEY_LONGITUDE, our_longitude);
	dict_write_cstring(body, WEATHER_KEY_UNIT_SYSTEM, UNIT_SYSTEM);
	// Send it.
	if(http_out_send() != HTTP_OK) {
		weather_layer_set_icon(&weather_layer, WEATHER_ICON_HTTP_ERROR);
		return;
	}
}