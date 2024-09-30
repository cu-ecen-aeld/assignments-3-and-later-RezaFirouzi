
// Author: Reza Firouzi

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> // for macros to check status
#include <string.h>
#include <fcntl.h>  // For open() system call and file access mode options i.e. O_RDWR
#include <libgen.h>

#include "systemcalls.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    // Check if the cmd is null
    if (cmd == NULL) {
        return false;
    }

    // Execute the command
    int status = system(cmd);

    // Check if the system() call is failed
    if (status == -1) {
        perror("Failed to invoke system()");
        return false;
    }

    // Check if the child process exited normally
    if (WIFEXITED(status)) {
        // Check if the command executed successfuly
        if (WEXITSTATUS(status) == 0) {
            return true;
        }
    // Check if the command in the child process terminated by a signal
    } else if (WIFSIGNALED(status)) {
        return false;
    }

    return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    // command[count] = command[count];

    va_end(args);

    pid_t childPid = fork();
    if (childPid == -1) {
        perror("Failed to call fork()");
        return false;
    }

    // Executing the command in the child process
    if (childPid == 0) {
        execv(command[0], command);
        exit(-1);
    }

    // Execution in the parent process
    pid_t retPid;
    int status;

    retPid = waitpid(childPid, &status, 0);
    if (retPid == -1) {
        perror("Failed in calling waitpid()");
        return  false;
    }

    // Check if the child process exited normally
    if (WIFEXITED(status)) {
        // Check if the command executed successfuly
        if (WEXITSTATUS(status) == 0) {
            return true;
        }
        // Check if the command in the child process terminated by a signal
    } else if (WIFSIGNALED(status)) {
        return false;
    }

    return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];
    va_end(args);

    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to call open()");
        return false;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to call fork()");
        return false;
    }

    // Executing the command in the child process
    if (pid == 0) {
        int fdDuplicateRet = dup2(fd, 1);
        if (fdDuplicateRet == -1) {
            perror("Failed to invoke dup2()");
            return false;
        }
        close(fd);
        execv(command[0], command);
        exit(-1);
    }
    close(fd);

    // Execution in the parent process
    pid_t retPid;
    int status;

    retPid = waitpid(pid, &status, 0);
    if (retPid == -1) {
        perror("Failed in calling waitpid()");
        return  false;
    }

    // Check if the child process exited normally
    if (WIFEXITED(status)) {
        // Check if the command executed successfuly
        if (WEXITSTATUS(status) == 0) {
            return true;
        }
        // Check if the command in the child process terminated by a signal
    } else if (WIFSIGNALED(status)) {
        return false;
    }

    return false;
}
