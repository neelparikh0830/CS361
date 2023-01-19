#include <stdio.h>
#include <string.h>

char str1[100];
const char* str2 = "Hello 361\0";
const char* str3 = "3";

int main(){

  printf("%s is %zu chars long\n", str2, strlen(str2));
  int i = 0;
  for (i=0; i<5;i++){
    strncat(str1, str2, 9);
  }
  printf("%s is %zu chars long\n", str1, strlen(str1));
  printf("%s\n", strpbrk(str2, str3));
  
 
  
}
