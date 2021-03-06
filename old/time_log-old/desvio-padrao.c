#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


float calculateSD(float data[], int n);
void measureAll(char filename[50]);
void get_conn_time();

int main()
{
    get_conn_time("operation_1.txt");
    get_conn_time("operation_2.txt");
    get_conn_time("operation_3.txt");
    get_conn_time("operation_4.txt");
    get_conn_time("operation_5.txt");
    get_conn_time("operation_6.txt");

    measureAll("client_operation_1.txt");
    measureAll("client_operation_2.txt");
    measureAll("client_operation_3.txt");
    measureAll("client_operation_4.txt");
    measureAll("client_operation_5.txt");
    measureAll("client_operation_6.txt");

    measureAll("comm_operation_1.txt");
    measureAll("comm_operation_2.txt");
    measureAll("comm_operation_3.txt");
    measureAll("comm_operation_4.txt");
    measureAll("comm_operation_5.txt");
    measureAll("comm_operation_6.txt");

    return 0;
}

float calculateSD(float data[], int n)
{
    float sum = 0.0, mean, standardDeviation = 0.0;
    int i;

    for(i=0; i<n; i++)
    {
        sum += data[i];
    }

    mean = sum/n;

    for(i=0; i<n; i++)
        standardDeviation += pow(data[i] - mean, 2);

    return sqrt(standardDeviation/n);
}

void measureAll(char filename[50])
{
    FILE *read_file, *write_file;
    float *data, aux, mean=0, confianca, desvio;
    int i, n;

    read_file = fopen(filename, "r");
    if (read_file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    n=0;
    while (fscanf(read_file, "%f", &aux) != EOF)
    {
        mean += aux;
        n++;
    }
    data = malloc(n * sizeof(float));

    rewind(read_file);

    i=0;
    while (fscanf(read_file, "%f", &data[i]) != EOF) { i++; }

    fclose(read_file);

    write_file = fopen("measures.txt", "a");
    if (write_file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    mean = mean/n;
    desvio = calculateSD(data, n);
    confianca = 1.96 * (desvio / sqrt(n));

    fprintf(write_file, "File: %s\n", filename);
    fprintf(write_file, "n = %d\n", n);
    fprintf(write_file, "Mean = %f\n", mean);
    fprintf(write_file, "Standard Deviation = %.6f\n", calculateSD(data, n));
    fprintf(write_file, "Intervalo de confianca: (%.6f - %.6f , %.6f + %.6f)\n\n",mean, confianca, mean, confianca);

    free(data);
    fclose(write_file);
}

void get_conn_time(char operation[15])
{
    FILE *server, *client, *result;
    float server_time, client_time;
    char server_name[50] = "server_";
    char client_name[50] = "client_";
    char result_name[50] = "comm_";

    strcat(server_name, operation);
    server = fopen(server_name, "r");

    if (server == NULL)
    {
        printf("Error opening server file!\n");
        return;
    }

    strcat(client_name, operation);
    client = fopen(client_name, "r");
    if (client == NULL)
    {
        printf("Error opening client file!\n");
        return;
    }

    strcat(result_name, operation);
    result = fopen(result_name, "a");
    if (result == NULL)
    {
        printf("Error opening result file!\n");
        return;
    }

    while (fscanf(client, "%f", &client_time) != EOF)
    {
        if(fscanf(server, "%f", &server_time) != EOF)
        {
            fprintf(result, "%f\n", (client_time - server_time)/2);
        }
    }

    fclose(server);
    fclose(client);
    fclose(result);
}
