//mytest2.c
int main(){
    int i;
    int A[40], C[40], D[40];
    for (i = 2; i < 25; i++) {
        A[i] = C[i];
        D[i*9-5] = A[i*3-7];
        D[i - 1] = C[2 * i];
    }
    return 0;
}
