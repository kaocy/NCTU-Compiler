      
int main() {      
    int n, a = {2};      
    int f[200]={0,1};      
    for(int i=2;i<90;i++) {      
        f[i]=f[i-1]+f[i-2];      
        //printf("%d\n", f[i]);      
    }  
    return 0;
}
int x;// test