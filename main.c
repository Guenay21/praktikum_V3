#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struktur, die die Daten des Sensors repräsentiert
struct SensorData 
{
    int sensor_id;
    int value;
    int warn_low;
    int warn_high;
    int alarm_low;
    int alarm_high;
};

// Funktion zur Berechnung des Mittelwerts eines Arrays von Werten
int calculate_mean(int* values, int size) 
{
    int sum = 0;
    for (int i = 0; i < size; ++i) 
    {
        sum += values[i];
    }
    return sum / size;
}

// Vergleichsfunktion für qsort, um Werte aufsteigend zu sortieren
int compare_int(const void* a, const void* b) 
{
    return (*(int*)a - *(int*)b);
}

// Funktion zur Berechnung des Medians eines Arrays von Werten
int calculate_median(int* values, int size) 
{
    // Kopieren der Werte in ein temporäres Array, um die Reihenfolge nicht zu ändern
    int* temp_values = (int*)malloc(size * sizeof(int));
    memcpy(temp_values, values, size * sizeof(int));
    
    // Sortieren des temporären Arrays
    qsort(temp_values, size, sizeof(int), compare_int);

    int median;
    if (size % 2 == 0) 
    {
        median = (temp_values[size / 2 - 1] + temp_values[size / 2]) / 2;
    } 
    else 
    {
        median = temp_values[size / 2];
    }

    free(temp_values);
    return median;
}

// Funktion zur Filterung von Sensorwerten basierend auf einer Option (original, mean, median)
int* filter_value(struct SensorData* sensor_data_list, int size, char* filter_option) 
{
    int* filtered_values = (int*)malloc(size * sizeof(int));

    if (strcmp(filter_option, "original") == 0) 
    {
        // Wenn "original" ausgewählt ist, kopiere die originalen Messwerte
        for (int i = 0; i < size; ++i) 
        {
            filtered_values[i] = sensor_data_list[i].value;
        }
    } 
    else if (strcmp(filter_option, "mean") == 0) 
    {
        // Wenn "mean" ausgewählt ist, berechne den Mittelwert über die letzten drei Messwerte
        for (int i = 0; i < size; ++i) 
        {
            int mean_values[3];
            for (int j = 0; j < 3; ++j) 
            {
                int index = i - j;
                mean_values[j] = (index >= 0) ? sensor_data_list[index].value : 0;
            }
            filtered_values[i] = calculate_mean(mean_values, 3);
        }
    } 
    else if (strcmp(filter_option, "median") == 0) 
    {
        // Wenn "median" ausgewählt ist, berechne den Median über die letzten drei Messwerte
        for (int i = 0; i < size; ++i) 
        {
            int median_values[3];
            for (int j = 0; j < 3; ++j) 
            {
                int index = i - j;
                median_values[j] = (index >= 0) ? sensor_data_list[index].value : 0;
            }
            filtered_values[i] = calculate_median(median_values, 3);
        }
    }

    return filtered_values;
}

// Funktion zur Auswertung der gefilterten Werte basierend auf den Anforderungen R1
void analyze_filtered_values(struct SensorData* sensor_data_list, int* filtered_values, int size) 
{
    int count_warning_low = 0;
    int count_warning_high = 0;
    int count_alarm_low = 0;
    int count_alarm_high = 0;

    for (int i = 0; i < size; ++i) 
    {
      
        //Prüfen, ob welche Bedingung erfüllt ist, und Zähler erhöhen
        if (filtered_values[i] >= sensor_data_list[i].alarm_high) 
        {
            count_alarm_high++;
        }

        if (filtered_values[i] <= sensor_data_list[i].alarm_low) 
        {
            count_alarm_low++;
        }

        if (filtered_values[i] >= sensor_data_list[i].warn_high) 
        {
            count_warning_high++;
        }

        if (filtered_values[i] <= sensor_data_list[i].warn_low) 
        {
            count_warning_low++;
        }
    }

    // Ausgabe der Ergebnisse
    printf("Anzahl der Werte über der oberen Alarmgrenze: %d\n", count_alarm_high);
    printf("Anzahl der Werte über der oberen Warnungsgrenze: %d\n", count_warning_high);
    printf("Anzahl der Werte unter der unteren Alarmgrenze: %d\n", count_alarm_low);
    printf("Anzahl der Werte unter der unteren Warnungsgrenze: %d\n", count_warning_low);
}

// Funktion zur Verarbeitung von Daten aus einer Textdatei
void process_data(char* file_path, char* filter_option) 
{
    FILE* file = fopen(file_path, "r");
    if (!file) 
    {
        perror("Fehler beim Öffnen der Datei");
        exit(EXIT_FAILURE);
    }

    // Angenommen , dass maximale Anzahl von Sensoren 1000 ist
    struct SensorData sensor_data_list[1000];
    int sensor_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) 
    {
        // Zeile mit Semikolon aufspalten
        char *token = strtok(line, ";");
       
        if (token != NULL) 
        {
            // Sensor-ID einlesen und speichern
            sensor_data_list[sensor_count].sensor_id = atoi(token);
            token = strtok(NULL, ";");
           
            if (token != NULL) 
            {
                // Messwert einlesen und speichern
                sensor_data_list[sensor_count].value = atoi(token);
                token = strtok(NULL, ";");
                // Warn_low, Warn_high, Alarm_low, Alarm_high einlesen und speichern
                sensor_data_list[sensor_count].warn_low = atoi(token);
                token = strtok(NULL, ";");
                sensor_data_list[sensor_count].warn_high = atoi(token);
                token = strtok(NULL, ";");
                sensor_data_list[sensor_count].alarm_low = atoi(token);
                token = strtok(NULL, ";");
                sensor_data_list[sensor_count].alarm_high = atoi(token);
            }
        }
        sensor_count++;
    }

    fclose(file);

    // Gefilterte Werte basierend auf der ausgewählten Option erhalten
    int* filtered_values = filter_value(sensor_data_list, sensor_count, filter_option);

    // Auswertung der gefilterten Werte
    analyze_filtered_values(sensor_data_list, filtered_values, sensor_count);

    // Speicher freigeben
    free(filtered_values);
}

int main() 
{
    //Verarbeitung von Daten aus "processData.txt" mit Filteroption "mean""median" und "original"
    process_data("D:\\VisualStudioCode\\testpr3\\src\\processData.txt", "mean");
    process_data("D:\\VisualStudioCode\\testpr3\\src\\processData.txt", "median");
    process_data("D:\\VisualStudioCode\\testpr3\\src\\processData.txt", "original");
    return 0;
}