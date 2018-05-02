#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float calculateSD(float data[], int n);

int main()
{

    FILE *f;
    char filename[50];
    float *data, aux, mean=0;
    int i, n;

    scanf("%s", filename);

    f = fopen(filename, "r");
    if (f == NULL)
	{
    	printf("Error opening file!\n");
    	return 1;
	}

    n=0;
    while (fscanf(f, "%f", &aux) != EOF)
    {
        mean += aux;
        n++;
    }
    data = malloc(n * sizeof(float));

    rewind(f);

    i=0;
    while (fscanf(f, "%f", &data[i]) != EOF) { i++; }

    printf("n = %d\n", n);
    printf("Mean = %f\n", mean/n);
    printf("Standard Deviation = %.6f\n", calculateSD(data, n));

    free(data);

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
