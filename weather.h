#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"
#include "json-c/json.h"

// Structure to hold API response data
struct Memory {
    char *data;
    size_t size;
};

// Function declarations
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
void send_alert(const char *message);
int fetch_weather_data(char *url, struct Memory *chunk);

#endif // WEATHER_H
