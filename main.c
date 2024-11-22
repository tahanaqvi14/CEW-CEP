#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "weather.h"

int main() {
    CURL *curl;
    CURLcode res;

    struct Memory chunk;
    chunk.data = malloc(1);
    chunk.size = 0;

    FILE *file = fopen("output.txt", "a");
    if (!file) {
        fprintf(stderr, "Could not open file for writing\n");
        return 1;
    }

    time_t rawtime;
    struct tm *timeinfo;
    char timeString[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Fetch weather data from API
    char *url = "https://open-weather13.p.rapidapi.com/city/karachi/EN";
    if (fetch_weather_data(url, &chunk) == 0) {
        // Process and store weather data
        FILE *rawFile = fopen("rawdata.txt", "a");
        if (!rawFile) {
            fprintf(stderr, "Could not open raw data file for writing\n");
            return 1;
        }

        // Write timestamp to the file
        fprintf(rawFile, "Date and Time: %s\n", timeString);

        // Write a newline before appending the JSON data
        fwrite(chunk.data, sizeof(char), chunk.size, rawFile);

        // Add a newline after the JSON data for better readability
        fprintf(rawFile, "\n");

        fclose(rawFile);


        

        struct json_object *parsed_json;
        struct json_object *city_name, *main, *weather_array, *weather_item, *description, *wind;
        struct json_object *temp, *humidity, *windspeed;
        const char *city, *weather_description;
        double temperature, wind_speed_value;
        int humidity_value;

        parsed_json = json_tokener_parse(chunk.data);

        // Extract weather data from the parsed JSON
        json_object_object_get_ex(parsed_json, "name", &city_name);
        json_object_object_get_ex(parsed_json, "main", &main);
        json_object_object_get_ex(parsed_json, "wind", &wind);
        json_object_object_get_ex(parsed_json, "weather", &weather_array);
        json_object_object_get_ex(main, "temp", &temp);
        json_object_object_get_ex(main, "humidity", &humidity);
        json_object_object_get_ex(wind, "speed", &windspeed);

        weather_item = json_object_array_get_idx(weather_array, 0);
        json_object_object_get_ex(weather_item, "description", &description);

        city = json_object_get_string(city_name);
        temperature = json_object_get_double(temp);
        humidity_value = json_object_get_int(humidity);
        wind_speed_value = json_object_get_double(windspeed);
        weather_description = json_object_get_string(description);

        // Write data to output file and print to console
        fprintf(file, "Date and Time: %s\n", timeString);
        fprintf(file, "Weather Information:\n");
        fprintf(file, "City: %s\n", city);
        fprintf(file, "Weather Description: %s\n", weather_description);
        fprintf(file, "Temperature: %.2f°F\n", temperature);
        fprintf(file, "Humidity: %d%%\n", humidity_value);
        fprintf(file, "Wind Speed: %.2f m/s\n", wind_speed_value);
        fprintf(file, "\n");

        printf("Date and Time: %s\n", timeString);
        printf("Weather Information:\n");
        printf("City: %s\n", city);
        printf("Weather Description: %s\n", weather_description);
        printf("Temperature: %.2f°F\n", temperature);
        printf("Humidity: %d%%\n", humidity_value);
        printf("Wind Speed: %.2f m/s\n", wind_speed_value);

        // Check if temperature exceeds alert threshold and send alert
        if (temperature > 85) {
            char alertMessage[256];
            snprintf(alertMessage, sizeof(alertMessage), "ALERT: High temperature of %.2f°F detected in %s!", temperature, city);
            send_alert(alertMessage);  // Send alert if condition is met
        }

        free(chunk.data);
    } else {
        printf("Failed to fetch weather data.\n");
    }

    fclose(file);

    return 0;
}
