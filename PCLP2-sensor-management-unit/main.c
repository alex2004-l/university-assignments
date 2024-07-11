/* Lache Alexandra Florentina Georgiana, 311 CD, Anul I, UPB ACS 2022-2023 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#define TIRE_DATA_LEN (2 * sizeof(float) + 2 * sizeof(int))
#define PMU_DATA_LEN (3 * sizeof(float) + 2 * sizeof(int))

#define flag_assert(cmd, val1, val2) \
do { \
	if (cmd < val1 || cmd > val2) \
		flag = 0; \
} while (0)

void get_operations(void **operations);

void free_sensor(sensor *sensor);
void free_list_of_sensors(int nr_sensors, sensor *sensor);

/* functie care citeste datele si operatiile pentru un
senzor de tipul Tire Sensor primit ca parametru*/
void get_tire_sensor(FILE *in, sensor *sensor)
{
	sensor->sensor_data = malloc(TIRE_DATA_LEN);
	if (sensor->sensor_data != NULL)
		fread(sensor->sensor_data, TIRE_DATA_LEN, 1, in);
	else
		fprintf(stderr, "ERROR: Cannot allocate memory\n");

	fread(&(sensor->nr_operations), sizeof(int), 1, in);
	sensor->operations_idxs = malloc(sizeof(int) * sensor->nr_operations);
	if (sensor->operations_idxs != NULL)
		fread(sensor->operations_idxs, sizeof(int), sensor->nr_operations, in);
	else
		fprintf(stderr, "ERROR: Cannot allocate memory\n");
}

/* functie care citeste datele si operatiile pentru un
senzor de tipul PMU Sensor primit ca parametru*/
void get_power_management_unit(FILE *in, sensor *sensor)
{
	sensor->sensor_data = malloc(PMU_DATA_LEN);
	if (sensor->sensor_data != NULL)
		fread(sensor->sensor_data, PMU_DATA_LEN, 1, in);
	else
		fprintf(stderr, "ERROR: Cannot allocate memory\n");

	fread(&(sensor->nr_operations), sizeof(int), 1, in);
	sensor->operations_idxs = malloc(sizeof(int) * sensor->nr_operations);
	if (sensor->operations_idxs != NULL)
		fread(sensor->operations_idxs, sizeof(int), sensor->nr_operations, in);
	else
		fprintf(stderr, "ERROR: Cannot allocate memory\n");
}

/* functie care copiaza la adresa primului senzor primit ca parametru
informatiile celui de-al doilea senzor */
void copy_sensor(sensor *sensor1, sensor *sensor2)
{
	sensor1->sensor_type = sensor2->sensor_type;
	sensor1->sensor_data = sensor2->sensor_data;
	sensor1->nr_operations = sensor2->nr_operations;
	sensor1->operations_idxs = sensor2->operations_idxs;
}

/* functie pentru citirea unui vector de senzori */
void get_list_of_sensors(FILE *in, int numberOfSensors, sensor *sensors)
{
	for (int i = 0; i < numberOfSensors; ++i) {
		fread(&(sensors[i].sensor_type), sizeof(enum sensor_type), 1, in);
		// se verifica tipul senzorului pentru a sti cum se efectueaza citirea
		if (sensors[i].sensor_type == 0) {
			get_tire_sensor(in, &(sensors[i]));
		} else {
			int j;
			for (j = i-1; j >= 0 && sensors[j].sensor_type != 1; --j) {
				copy_sensor(&(sensors[j+1]), &(sensors[j]));
			}
			j++;
			sensors[j].sensor_type = 1;
			get_power_management_unit(in, &(sensors[j]));
		}
	}
}

// functie pentru printarea informatiilor unui Tire Sensor
void print_tire_sensors(void *data)
{
	tire_sensor *t = (tire_sensor *) data;
	if (data != NULL) {
		printf("Tire Sensor\n");
		printf("Pressure: %.2f\n", t->pressure);
		printf("Temperature: %.2f\n", t->temperature);
		printf("Wear Level: %d%%\n", t->wear_level);
		if (t->performace_score == 0)
			printf("Performance Score: Not Calculated\n");
		else
			printf("Performance Score: %d\n", t->performace_score);
	}
}

// functie pentru printarea informatiilor unui PMU Sensor
void print_power_management_unit(void *data)
{
	power_management_unit *pmu = (power_management_unit *) data;
	if (data != NULL) {
		printf("Power Management Unit\n");
		printf("Voltage: %.2f\n", pmu->voltage);
		printf("Current: %.2f\n", pmu->current);
		printf("Power Consumption: %.2f\n", pmu->power_consumption);
		printf("Energy Regen: %d%%\n", pmu->energy_regen);
		printf("Energy Storage: %d%%\n", pmu->energy_storage);
	}
}

// functie pentru analizarea informatiilor unui senzor
void analyze_sensor(int nr_operations, int *operations_idxs, void *data)
{
	if (operations_idxs != NULL && data != NULL) {
		void **operations;
		operations = malloc(8 * sizeof(void *));
		if (operations != NULL) {
			get_operations(operations);
				for (int i = 0; i < nr_operations; ++i) {
					void (*fun_ptr)(void *) = operations[operations_idxs[i]];
					fun_ptr(data);
				}
			free(operations);
		} else
			fprintf(stderr, "ERROR: Cannot allocate memory for operations\n");
	} else
		fprintf(stderr, "ERROR: Cannot analyze sensor\n");
}

// functie pentru stergerea din vector a senzorilor cu valori eronate
void clear_list_of_sensors(int *numberOfSensors, sensor *sensors)
{
	int flag = 0, nrSen = (*numberOfSensors);
	for (int i = nrSen - 1; i >= 0; --i) {
		flag = 1;
		if (sensors[i].sensor_type == 1) {
			// verificari senzori de tipul PMU Sensor
			power_management_unit *pmu =
				(power_management_unit *)sensors[i].sensor_data;
			if (pmu == NULL)
				flag = 0;
			else {
				flag_assert(pmu->voltage, 10, 20);
				flag_assert(pmu->current, -100, 100);
				flag_assert(pmu->power_consumption, 0, 1000);
				flag_assert(pmu->energy_regen, 0, 100);
				flag_assert(pmu->energy_storage, 0, 100);
			}
		} else {
			// verificari senzori de tipul Tire Sensors
			tire_sensor *t = (tire_sensor *)sensors[i].sensor_data;
			if (t == NULL)
				flag = 0;
			else {
				flag_assert(t->pressure, 19, 28);
				flag_assert(t->temperature, 0, 120);
				flag_assert(t->wear_level, 0, 100);
			}
		}
		// cazul in care senzorul are valori eronate
		if (flag == 0) {
			// memoria este dealocata
			free_sensor(&(sensors[i]));
			// restul senzorilor sunt permutati cu o pozitie la stanga
			for (int j = i; j < nrSen-1; j++) {
				copy_sensor(&(sensors[j]), &(sensors[j+1]));
			}
			nrSen -= 1;
		}
	}
	(*numberOfSensors) = nrSen;
}

// functie pentru dealocarea memoriei unui senzor
void free_sensor(sensor *sensor)
{
	if (sensor->sensor_data != NULL)
		free(sensor->sensor_data);
	if (sensor->operations_idxs != NULL)
		free(sensor->operations_idxs);
}

// functie pentru dealocarea memoriei vectorului de senzori
void free_list_of_sensors(int nr_sensors, sensor *sensors)
{
	for (int i = 0; i < nr_sensors; ++i) {
		free_sensor(&(sensors[i]));
	}
	free(sensors);
}

int main(int argc, char const *argv[])
{
	FILE *in;
	in = fopen(argv[1], "rb");

	if (in == NULL) {
		fprintf(stderr, "ERROR: Cannot open %s file", argv[1]);
	}

	int numberOfSensors = 0, index = 0;
	fread(&numberOfSensors, sizeof(int), 1, in);

	sensor *sensors = malloc(numberOfSensors * sizeof(sensor));
	if (sensors == NULL) {
		fprintf(stderr, "ERROR: Cannot allocate memory\n");
		return -1;
	} else
		get_list_of_sensors(in, numberOfSensors, sensors);

	char command[20];
	scanf("%s", command);
	while (strcmp(command, "exit")) {
		if (strcmp(command, "print") == 0) {
			scanf("%d", &index);
			if (index < 0 || index >= numberOfSensors) {
				printf("Index not in range!\n");
			} else if (sensors[index].sensor_type == 0) {
				print_tire_sensors(sensors[index].sensor_data);
			} else
				print_power_management_unit(sensors[index].sensor_data);
		}
		if (strcmp(command, "analyze") == 0) {
			scanf("%d", &index);
			if (index < 0 || index >= numberOfSensors)
				printf("Index not in range!\n");
			else
				analyze_sensor(sensors[index].nr_operations,
				sensors[index].operations_idxs, sensors[index].sensor_data);
		}
		if (strcmp(command, "clear") == 0) {
			clear_list_of_sensors(&numberOfSensors, sensors);
			// vectorul de senzori este realocat dinamic
			sensor *aux = realloc(sensors, numberOfSensors * sizeof(sensor));
			if (aux != NULL)
				sensors = aux;
			else
				fprintf(stderr, "ERROR: Cannot reallocate array\n");
		}
		scanf("%s", command);
	}

	// dealocare memorie pentru vectorul de senzori
	free_list_of_sensors(numberOfSensors, sensors);

	fclose(in);
	return 0;
}
