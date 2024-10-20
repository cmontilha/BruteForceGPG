#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// this is to test using system command and gpg to decrypt file

int main() {
  int rc;
  char pass[50] = "asdfsa";
//  char pass[50] = "changeme";
  char command[150];
  char opt[] = ">nul 2>nul";
  sprintf(command, "gpg -d --batch --passphrase %s test.gpg", pass); // ##### change filename to file you are testing against
  
  strcat(command, opt);
  rc = system(command);
  printf("%d", rc);
}
