#include <stdio.h>

void main()
{
    for (char i = 0; i < 5; i++)
        printf("%d\t", i);

    float a = (7 / 2) * 7;

    if (1 == 1 && 1 || 2)
        printf("Compiler\n");
    else
        printf("Design\n");

    long long temp = 2 ^ ~((int)a);

    while (temp < 0 && temp++)
        printf("%lld\t", temp);
}