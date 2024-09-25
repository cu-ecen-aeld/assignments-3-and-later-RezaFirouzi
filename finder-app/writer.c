// Author: Reza Firouzi

#include <stdio.h>
#include <fcntl.h>  // For open() system call and file access mode options i.e. O_RDWR
//#include <sys/stat.h>
//#include <sys/types.h>

int main(int argc, char *argv [])
{
  if (argc != 3)
  {
    printf("Error: Two required arguments must be passed to the program!\n");
    printf("Usage: %s <file_path> <string_to_write>\n", argv[0]);
    return 1;
  }

  const char * filePath = argv[1];
  const char * stringToWrite = argv[2];

  //int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  //int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0664);
  int fileDescriptor = creat(filePath, 0664);
  if (fileDescriptor == -1)
  {
    //perror("open() system call failed");
    perror("creat() system call failed");
    return 1;
  }



  close(fileDescriptor);
  return 0;
}