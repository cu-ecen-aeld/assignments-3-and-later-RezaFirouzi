// Author: Reza Firouzi

#include <stdio.h>
#include <fcntl.h>  // For open() system call and file access mode options i.e. O_RDWR
//#include <sys/stat.h>
//#include <sys/types.h>
#include <unistd.h>  // For write() system call
#include <string.h>  // For len() function
#include <syslog.h>

int main(int argc, char *argv [])
{
  openlog("writeApplication", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);

  if (argc != 3)
  {
    printf("Error: Two required arguments must be passed to the program!\n");
    printf("Usage: %s <file_path> <string_to_write>\n", argv[0]);
    syslog(LOG_ERR, "Two required arguments not passed to the program!");
    return 1;
  }

  const char * filePath = argv[1];
  const char * stringToWrite = argv[2];
  const size_t stringLength = strlen(stringToWrite);

  //int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  //int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0664);
  int fileDescriptor = creat(filePath, 0664);
  if (fileDescriptor == -1)
  {
    //perror("open() system call failed");
    perror("creat() system call failed");
    syslog(LOG_ERR, "Failed to create the file!");
    return 1;
  }

  syslog(LOG_DEBUG, "Writing %s to %s", filePath, stringToWrite);
  ssize_t writtenByteCount = write(fileDescriptor, stringToWrite, stringLength);

  if (writtenByteCount == -1)
  {
    perror("write() system call failed");
    syslog(LOG_ERR, "Failed to write to the file!");
    return 1;
  }

  close(fileDescriptor);
  closelog();
  return 0;
}