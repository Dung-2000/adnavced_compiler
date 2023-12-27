//mytest1.c
int main(){
    int i;
    int A[40],B[30], C[40], D[40];
    for (i = 2; i < 20; i++) {
      A[i] = C[i];
      D[i*3-2] = A[i*3-4];
      D[i - 1] = C[2 * i];
      B[4*i-5] = D[i*2-3]; 
    }
    return 0;
}
