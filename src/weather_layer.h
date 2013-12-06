#ifndef WEATHER_LAYER_H
#define WEATHER_LAYER_H

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer temp_layer;
	TextLayer highvalue_layer;
	TextLayer highlabel_layer;
	
	TextLayer btc_layer;
	TextLayer ltc_layer;
	
	TextLayer lowvalue_layer;
	TextLayer lowlabel_layer;
	TextLayer temp_layer_background;
	bool has_weather_icon;
	char temp_str[5];
	char high_str[10];
	char highlbl_str[10];
	char lowlbl_str[10];
	char low_str[10];
	
	char btc_str[10];
	char ltc_str[10];
	
} WeatherLayer;

typedef enum {
	WEATHER_ICON_CLEAR_DAY = 0,
	WEATHER_ICON_CLEAR_NIGHT,
	WEATHER_ICON_RAIN,
	WEATHER_ICON_SNOW,
	WEATHER_ICON_SLEET,
	WEATHER_ICON_WIND,
	WEATHER_ICON_FOG,
	WEATHER_ICON_CLOUDY,
	WEATHER_ICON_PARTLY_CLOUDY_DAY,
	WEATHER_ICON_PARTLY_CLOUDY_NIGHT,
	WEATHER_ICON_HTTP_ERROR,
	WEATHER_ICON_NO_WEATHER,
	WEATHER_ICON_COUNT
} WeatherIcon;

void weather_layer_init(WeatherLayer* weather_layer, GPoint pos);
void weather_layer_deinit(WeatherLayer* weather_layer);
void weather_layer_set_icon(WeatherLayer* weather_layer, WeatherIcon icon);
void weather_layer_set_temperature(WeatherLayer* weather_layer, int16_t temperature);
void weather_layer_set_high(WeatherLayer* weather_layer, int16_t high);
void weather_layer_set_low(WeatherLayer* weather_layer, int16_t low);

void weather_layer_set_ltc(WeatherLayer* weather_layer, int16_t ltc);
void weather_layer_set_btc(WeatherLayer* weather_layer, int16_t btc);

#endif
