#include <stdio.h>
#include <stdlib.h>
void main(){
    int n=0;
    printf("enter n\n");
    scanf("%d",&n);
    // printf("%d",n);
    int *a=malloc(n*sizeof(int));
    for (int i=0;i<n;i++){
        scanf("%d",&a[i]);
    }
    printf("array \n");
    for (int i=0;i<n;i++){
        printf("%d",a[i]);
    }
}
