#include <stdio.h>
#include <stdlib.h>
int max = 15, min = 15;

int get_sign(int edge, int b){
    signed int sum = edge - b;
    sum = (sum >> 31) & 1;
    
    return sum;
}
void doMax(int a){
    max = a;
    printf("Max changed to %d\n", max);
}
void doMin(int a){
    min = a;
    printf("Min changed to %d\n", min);
}
void non(int a){
    printf("No changes recored for %d        ", a);
}

void swaparoo(int a){
    void (*compCall[2])(int) = {non, doMax};
    compCall[get_sign(max, a)](a);


    compCall[1] = doMin;
    get_sign(min, a);
    compCall[get_sign(a, min)](a);
}



int main(int argc, char *argv[]){
    int a = 20, b = 12;
    int x;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }
    
    printf("\nSTARTING RESULTS\n max: %d\nmin: %d\n",max, min);
    while (fscanf(fp, "%d", &x) == 1) {
        printf("\nmax:%d min:%d current:%d  ------->", max, min, x);
        swaparoo(x);
    }

    printf("\nFINAL RESULTS\n max: %d\nmin: %d\n",max, min);

}