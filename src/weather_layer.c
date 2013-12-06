#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "weather_layer.h"


	
static uint8_t WEATHER_ICONS[] = {
	RESOURCE_ID_ICON_CLEAR_DAY,
	RESOURCE_ID_ICON_CLEAR_NIGHT,
	RESOURCE_ID_ICON_RAIN,
	RESOURCE_ID_ICON_SNOW,
	RESOURCE_ID_ICON_SLEET,
	RESOURCE_ID_ICON_WIND,
	RESOURCE_ID_ICON_FOG,
	RESOURCE_ID_ICON_CLOUDY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_DAY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_NIGHT,
	RESOURCE_ID_ICON_HTTP_ERROR,
	RESOURCE_ID_ICON_ERROR,
	
};
//LEFT TOP WIDE HIGH
void weather_layer_init(WeatherLayer* weather_layer, GPoint pos) {
	layer_init(&weather_layer->layer, GRect(0,0,144,168));
	
	// Add background layer
	text_layer_init(&weather_layer->temp_layer_background, GRect(0, 0, 144, 95));
	text_layer_set_background_color(&weather_layer->temp_layer_background, GColorClear);
	layer_add_child(&weather_layer->layer, &weather_layer->temp_layer_background.layer);
	
    // Add temperature layer
	text_layer_init(&weather_layer->temp_layer, GRect(10, -7, 72, 95));
	text_layer_set_background_color(&weather_layer->temp_layer, GColorClear);
	text_layer_set_text_color(&weather_layer->lowvalue_layer, GColorWhite);
	text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentCenter);
	text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TEMP_45)));
	layer_add_child(&weather_layer->layer, &weather_layer->temp_layer.layer);

	//ADD HIGH LABEL VALUE LAYER
	text_layer_init(&weather_layer->highvalue_layer, GRect(116, 2, 32, 18));
	text_layer_set_background_color(&weather_layer->highvalue_layer, GColorClear);
	text_layer_set_text_color(&weather_layer->highvalue_layer, GColorWhite);
	text_layer_set_text_alignment(&weather_layer->highvalue_layer, GTextAlignmentLeft);
	text_layer_set_font(&weather_layer->highvalue_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HIGH_LOW_18)));
	layer_add_child(&weather_layer->layer, &weather_layer->highvalue_layer.layer);
	
	//ADD LOW LABEL VALUE LAYER
	text_layer_init(&weather_layer->lowvalue_layer, GRect(116, 20, 32, 18));
	text_layer_set_background_color(&weather_layer->lowvalue_layer, GColorClear);
	text_layer_set_text_color(&weather_layer->lowvalue_layer, GColorWhite);
	text_layer_set_text_alignment(&weather_layer->lowvalue_layer, GTextAlignmentLeft);
	text_layer_set_font(&weather_layer->lowvalue_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HIGH_LOW_18)));
	layer_add_child(&weather_layer->layer, &weather_layer->lowvalue_layer.layer);	

	
	//LTC_layer	
	text_layer_init(&weather_layer->ltc_layer, GRect(0, 141, 55, 35));
	text_layer_set_background_color(&weather_layer->ltc_layer, GColorClear);
	text_layer_set_text_color(&weather_layer->ltc_layer, GColorWhite);
	text_layer_set_text_alignment(&weather_layer->ltc_layer, GTextAlignmentRight);
	text_layer_set_font(&weather_layer->ltc_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PRICES_22)));
	layer_add_child(&weather_layer->layer, &weather_layer->ltc_layer.layer);
	
	//BTC_layer	
	text_layer_init(&weather_layer->btc_layer, GRect(85, 141, 48, 35));
	text_layer_set_background_color(&weather_layer->btc_layer, GColorClear);
	text_layer_set_text_color(&weather_layer->btc_layer, GColorBlack);
	text_layer_set_text_alignment(&weather_layer->btc_layer, GTextAlignmentRight);
	text_layer_set_font(&weather_layer->btc_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PRICES_22)));
	layer_add_child(&weather_layer->layer, &weather_layer->btc_layer.layer);
	

	
	
	// Note absence of icon layer
	weather_layer->has_weather_icon = false;
}

void weather_layer_set_icon(WeatherLayer* weather_layer, WeatherIcon icon) {
	
	// Remove any possible existing weather icon
	if(weather_layer->has_weather_icon) {
		layer_remove_from_parent(&weather_layer->icon_layer.layer.layer);
		bmp_deinit_container(&weather_layer->icon_layer);
		weather_layer->has_weather_icon = false;
	}
	
	// Add weather icon
	bmp_init_container(WEATHER_ICONS[icon], &weather_layer->icon_layer);
	layer_add_child(&weather_layer->layer, &weather_layer->icon_layer.layer.layer);
	layer_set_frame(&weather_layer->icon_layer.layer.layer, GRect(73,2, 40, 40));
	weather_layer->has_weather_icon = true;
}

//BTC
void weather_layer_set_btc(WeatherLayer* weather_layer, int16_t btc) {
	memcpy(weather_layer->btc_str, itoa(btc), 4);
	int btc_pos = strlen(weather_layer->btc_str);
	memcpy(&weather_layer->btc_str[btc_pos], "", 3);
	text_layer_set_text(&weather_layer->btc_layer, weather_layer->btc_str);
}

//LTC
void weather_layer_set_ltc(WeatherLayer* weather_layer, int16_t ltc) {
	memcpy(weather_layer->ltc_str, itoa(ltc), 4);
	int ltc_pos = strlen(weather_layer->ltc_str);
	memcpy(&weather_layer->ltc_str[ltc_pos], " ", 3);
	text_layer_set_text(&weather_layer->ltc_layer, weather_layer->ltc_str);
	layer_mark_dirty(&weather_layer->layer);
	
}



void weather_layer_set_high(WeatherLayer* weather_layer, int16_t h) {
	memcpy(weather_layer->high_str, itoa(h), 4);
	int degree_pos = strlen(weather_layer->high_str);
	memcpy(&weather_layer->high_str[degree_pos], " ", 3);
	text_layer_set_text(&weather_layer->highvalue_layer, weather_layer->high_str);
	layer_mark_dirty(&weather_layer->layer);
	/*
	memcpy(&weather_layer->highlbl_str[0], "HIGH :", 6);
	text_layer_set_text(&weather_layer->highlabel_layer, weather_layer->highlbl_str);*/
}

void weather_layer_set_low(WeatherLayer* weather_layer, int16_t l) {
	memcpy(weather_layer->low_str, itoa(l), 4);
	int degree_poss = strlen(weather_layer->low_str);
	memcpy(&weather_layer->low_str[degree_poss], " ", 3);
	text_layer_set_text(&weather_layer->lowvalue_layer, weather_layer->low_str);
	layer_mark_dirty(&weather_layer->layer);
	/*
	memcpy(&weather_layer->lowlbl_str[0], "LOW :", 5);
	text_layer_set_text(&weather_layer->lowlabel_layer, weather_layer->lowlbl_str);*/
}


void weather_layer_set_temperature(WeatherLayer* weather_layer, int16_t t) {
	memcpy(weather_layer->temp_str, itoa(t), 4);
	int degree_pos = strlen(weather_layer->temp_str);
	
	if (strlen(weather_layer->temp_str) == 1 || 
		(strlen(weather_layer->temp_str) == 2 && weather_layer->temp_str[0] != '1')) {
	  // Don't move temperature if between 0-9° or 20°-99°
	  text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TEMP_45)));
	  text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentCenter);
	  memcpy(&weather_layer->temp_str[degree_pos], " ", 3);
	} else if (strlen(weather_layer->temp_str) == 2 && weather_layer->temp_str[0] == '1') {
	  // Move temperature slightly to the left if between 10°-19°
	  text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TEMP_45)));
	  text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentLeft);
	  memcpy(&weather_layer->temp_str[degree_pos], " ", 3); 
	} else if (strlen(weather_layer->temp_str) > 2) { 
	  // Shrink font size if above 99° or below -9°
	  text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TEMP_45)));
	  text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentCenter);
	  memcpy(&weather_layer->temp_str[degree_pos], " ", 3);
	}
	text_layer_set_text(&weather_layer->temp_layer, weather_layer->temp_str);
	layer_mark_dirty(&weather_layer->layer);
}



void weather_layer_deinit(WeatherLayer* weather_layer) {
	if(weather_layer->has_weather_icon)
		bmp_deinit_container(&weather_layer->icon_layer);
}