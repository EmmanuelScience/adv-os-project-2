#include <stdio.h>
#include "client_library.h"

int main(int argc, char *argv[]) {
  printf("this is the client\n");
  print_stuff();



  // read in file and put into buffer
  //Create and open file for send
  unsigned char *buffer;
  unsigned long file_len;
  FILE *file, *file1;

  //open the file
  file = fopen("client_input.txt", "r+");
  file1 = fopen("compressed", "rw");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file %s\n", argv[1]);
    return 1;
  }

  //Get file length
  fseek(file, 0, SEEK_END);
  file_len=ftell(file);
  fseek(file, 0, SEEK_SET);

  //Allocate memory
  buffer = (char *) malloc(file_len);
  if (!buffer)
    {
      fprintf(stderr, "Memory error!");
      fclose(file);
      return 1;
    }

  fread(buffer,file_len,sizeof(unsigned char),file);
  fclose(file);


  char * compressed_file_buffer = sync_compress(buffer, file_len);
  fprintf(file1, "The text: %s\n", compressed_file_buffer);


}
