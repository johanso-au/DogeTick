<?php
 
function get_data($forecast_url)
{
    $ch = curl_init();
    $timeout = 0;
    curl_setopt($ch,CURLOPT_URL,$forecast_url);
    curl_setopt($ch,CURLOPT_RETURNTRANSFER,1);
    curl_setopt($ch,CURLOPT_CONNECTTIMEOUT,$timeout);
    $data = curl_exec($ch);
    curl_close($ch);
    return $data;
}
function get_data2($litecoin_url)
{
    $ch2 = curl_init();
    $timeout2 = 0;
    curl_setopt($ch2,CURLOPT_URL,$litecoin_url);
    curl_setopt($ch2,CURLOPT_RETURNTRANSFER,1);
    curl_setopt($ch2,CURLOPT_CONNECTTIMEOUT,$timeout2);
    $data2 = curl_exec($ch2);
    curl_close($ch2);
    return $data2;
}
function get_data3($coinbase_url)
{
    $ch3 = curl_init();
    $timeout3 = 0;
    curl_setopt($ch3,CURLOPT_URL,$coinbase_url);
    curl_setopt($ch3,CURLOPT_RETURNTRANSFER,1);
    curl_setopt($ch3,CURLOPT_CONNECTTIMEOUT,$timeout3);
    $data3 = curl_exec($ch3);
    curl_close($ch3);
    return $data3;
}

//SET YOUR API KEY HERE
$api_key='YOUR API HERE';


$payload = json_decode(file_get_contents('php://input'), true);
/*
if(!$payload) 
{
 $payload = json_decode('{"1": 411157,"2": -960238,"3": "us"}', true);
}
*/
$payload[1] = $payload[1] / 10000;
$payload[2] = $payload[2] / 10000;

$forecast_url='https://api.forecast.io/forecast/'.$api_key.'/'.$payload[1].','.$payload[2].'?units='.$payload[3].'&exclude=hourly,minutely,alerts';
$litecoin_url='https://www.litecoinpool.org/api?api_key=YOUR API HERE';//ENTER YOUR API FROM LITECOINPOOL
$coinbase_url='https://coinbase.com/api/v1/prices/buy';

$forecast = json_decode(get_data($forecast_url)); 
$coin = json_decode(get_data2($litecoin_url)); 
$coinbase = json_decode(get_data2($coinbase_url)); 

if(!$forecast) {
    die();
}      
$response = array();
$icons = array(
    'clear-day' => 0,
    'clear-night' => 1,
    'rain' => 2,
    'snow' => 3,
    'sleet' => 4,
    'wind' => 5,
    'fog' => 6,
    'cloudy' => 7,
    'partly-cloudy-day' => 8,
    'partly-cloudy-night' => 9
);
$ltc_price = $coin->market->ltc_usd;
$btc_price = $coin->market->btc_usd;
$coinbase_price = $coinbase->total->amount;
$sunset = $forecast->daily->data[0]->sunsetTime;
$sunset_h = date('H', $sunset);
$sunset_m = date('i', $sunset);

$round_m = intval($sunset_m);

while ($round_m < 10)
{
  if ($round_m < 5)
  {
    $round_m = 59;
    $sunset_h = $sunset_h -1;
  }
  elseif ($round_m >=5)
  {
    $round_m = 10;
    }
}

$icon_id = $icons[$forecast->currently->icon];
$response[1] = array('b', $icon_id);
$response[2] = array('s', round($forecast->currently->temperature));
$response[3] = array('s', round($forecast->daily->data[0]->temperatureMax));
$response[4] = array('s', round($forecast->daily->data[0]->temperatureMin));
$response[5] = array('s', intval($ltc_price));
$response[6] = array('s', intval($coinbase_price));
print json_encode($response);
