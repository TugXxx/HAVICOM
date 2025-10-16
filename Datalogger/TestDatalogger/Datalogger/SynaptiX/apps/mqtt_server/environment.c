#include "environment.h"

typedef struct { 
    float Clow, Chigh; 
    int Ilow, Ihigh; 
} AQIRange;

AQIRange pm25_ranges[] = {
    {0.0, 12.0, 0, 50},
    {12.1, 35.4, 51, 100},
    {35.5, 55.4, 101, 150},
    {55.5, 150.4, 151, 200},
    {150.5, 250.4, 201, 300},
    {250.5, 500.4, 301, 500}
};

AQIRange pm10_ranges[] = {
    {0, 54, 0, 50},
    {55, 154, 51, 100},
    {155, 254, 101, 150},
    {255, 354, 151, 200},
    {355, 424, 201, 300},
    {425, 604, 301, 500}
};

AQIRange co_ranges[] = {
    {0.0, 4.4, 0, 50},
    {4.5, 9.4, 51, 100},
    {9.5, 12.4, 101, 150},
    {12.5, 15.4, 151, 200},
    {15.5, 30.4, 201, 300},
    {30.5, 50.4, 301, 500}
};

AQIRange so2_ranges[] = {
    {0, 35, 0, 50},
    {36, 75, 51, 100},
    {76, 185, 101, 150},
    {186, 304, 151, 200},
    {305, 604, 201, 300},
    {605, 1004, 301, 500}
};

AQIRange o3_ranges[] = {
    {0, 54, 0, 50},
    {55, 70, 51, 100},
    {71, 85, 101, 150},
    {86, 105, 151, 200},
    {106, 200, 201, 300}
};

static int calculateAQI(float C, float Clow, float Chigh, int Ilow, int Ihigh) {
    return (int)(((float)(Ihigh - Ilow) / (Chigh - Clow)) * (C - Clow) + Ilow);
}

static int getAQI_(float value, AQIRange *ranges, int size) {
    for (int i = 0; i < size; i++) {
        if (value >= ranges[i].Clow && value <= ranges[i].Chigh) {
            return calculateAQI(value, ranges[i].Clow, ranges[i].Chigh, ranges[i].Ilow, ranges[i].Ihigh);
        }
    }
    return -1; 
}

int getAQI(SensorData *data) {
    int aqi_values[5];

    aqi_values[0] = getAQI_(data->pm25, pm25_ranges, sizeof(pm25_ranges)/sizeof(pm25_ranges[0]));
    aqi_values[1] = getAQI_(data->pm10, pm10_ranges, sizeof(pm10_ranges)/sizeof(pm10_ranges[0]));
    aqi_values[2] = getAQI_(data->co,   co_ranges,   sizeof(co_ranges)/sizeof(co_ranges[0]));
    aqi_values[3] = getAQI_(data->so2,  so2_ranges,  sizeof(so2_ranges)/sizeof(so2_ranges[0]));
    aqi_values[4] = getAQI_(data->o3,   o3_ranges,   sizeof(o3_ranges)/sizeof(o3_ranges[0]));

    int max_aqi = aqi_values[0];
    for (int i = 1; i < 5; i++) {
        if (aqi_values[i] > max_aqi) {
            max_aqi = aqi_values[i];
        }
    }

    return max_aqi;
}

