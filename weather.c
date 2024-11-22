#include "weather.h"
#include <unistd.h>


size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->data, mem->size + totalSize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Not enough memory for API response\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, totalSize);
    mem->size += totalSize;
    mem->data[mem->size] = '\0';

    return totalSize;
}

void send_alert(const char *message) {
    char command[512];
    snprintf(command, sizeof(command), "notify-send 'Weather Alert' '%s'", message);

    system(command);


    // write(STDOUT_FILENO, "\a", 1);

    // write(STDOUT_FILENO, message, strlen(message));
}

int fetch_weather_data(char *url, struct Memory *chunk) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) {
        return -1; 
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "X-Rapidapi-Key: c09fc65b8emshcad6b2b9327e78dp104eb7jsncde9466fb732");
    headers = curl_slist_append(headers, "X-Rapidapi-Host: open-weather13.p.rapidapi.com");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        return -1; 
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return 0; 
}
