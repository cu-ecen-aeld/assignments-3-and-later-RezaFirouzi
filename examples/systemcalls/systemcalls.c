#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h> // for macros to check status
#include "systemcalls.h"

#include <string.h>

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

    // Child process failed to exit/terminate normally
    if (!WIFEXITED(status)) {
        return false;
    }

    // The command was terminated by a signal
    if (WIFSIGNALED(status)) {
        return false;
    }

    // Failure in having a successful execution
    if (WEXITSTATUS(status) != 0) {
        return false;
    }

    return true;
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
    command[count] = command[count];
    char * arguments[count];

    memcpy(arguments, &command[1], count * sizeof(*command));

    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to call fork()");
        return false;
    }

    // Executing the command in the child process
    if (pid == 0) {
        // int ret = execv(command[0], arguments);
        // if (ret == -1) {
        //     perror("Failed to invoke execv()");
        //     return false;
        // }
        execv(command[0], arguments);
        exit(-1);
    }

    // Execution in the parent process
    pid_t retPid;
    int status;

    retPid = waitpid(pid, &status, 0);
    if (retPid == -1) {
        perror("Failed in calling waitpid()");
        return  false;
    }

    // The child process failed to exit/terminate normally
    if (!WIFEXITED(status)) {
        return false;
    }

    // The command in the child process terminated by a signal
    if (WIFSIGNALED(status)) {
        return false;
    }

    // The executed command in the child process not returned success
    if (WEXITSTATUS(status) != 0) {
        return false;
    }

    va_end(args);

    return true;
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


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    va_end(args);

    return true;
}
