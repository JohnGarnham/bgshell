/**
 *
 * bgshell.c
 *
 * A user shell
 *
 * @version: $Id: bgshell.c,v 1.6 2007/10/03 03:38:37 jeg3600 Exp jeg3600 $
 *
 * @author: John Garnham
 *
 * Revisions:
 *       $Log: bgshell.c,v $
 *       Revision 1.6  2007/10/03 03:38:37  jeg3600
 *       Added perrors
 *
 *       Revision 1.5  2007/10/03 02:16:59  jeg3600
 *       Corrected initial error
 *
 *       Revision 1.4  2007/10/01 19:56:47  jeg3600
 *       Added background process support
 *
 *       Revision 1.3  2007/09/29 17:36:13  jeg3600
 *       Runs child processes
 *
 *       Revision 1.2  2007/09/28 06:34:31  jeg3600
 *       Added makeshit parsing
 *
 *       Revision 1.1  2007/09/28 04:56:10  jeg3600
 *       Initial revision
 *
 *
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

// Buffer size for command line input
#define BUFFER_SIZE 1024

// The signal handler installation routine
void install_signal_handler();

// Signal handler
void catch_signal(int sig_num);

// Function for getting a line of input
char* get_line(void);

// Function for executing a command in a child process
void  execute(char **argv, int background);

// Function for parasing
void  parse(char *line, char **argv);

// The signal number
int signal_num = 0;

// Signal names
char* signal_name[] = {
  "", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT",
  "SIGEMT", "SIGFPE", "SIGKILL", "SIGBUS", "SIGSEGV", "SIGSYS",
  "SIGPIPE", "SIGALRM", "SIGTERM", "SIGUSR1", "SIGUSR2", "SIGCHLD",
  "SIGPWR", "SIGWINCH", "SIGURG", "SIGPOLL", "SIGSTOP", "SIGTSTP",
  "SIGCONT", "SIGTTIN", "SIGTTOU", "SIGVTALRM", "SIGPROF", 
  "SIGXCPU", "SIFWAITING"};

// Child process names
char* child[4];

// Index of the last child run
int lastchild = 0;

// Bool of whether full or not
int full = 0;

// Index of the child first executed
int firstexec = 0;

int i;

int main() {

  // Input from the user
  char* input;

  // Allocate space for input buffer
  if((input = malloc(BUFFER_SIZE)) == NULL) {
    perror("Could not allocate enough memory.");
  }
  input = "";

  // Allocate space for the child process name table
  for(i = 0; i < 4; ++i) {
    
    if((child[i] = malloc(215)) == NULL) {
      perror("Could not allocate enough memory.");
    }

  }

  // Install the signal handler
  install_signal_handler();

  // Main program loop

  while(*input != EOF) {
        
    // Reset input
    input = "";

    printf("\nCOMMAND (ctl-d or enter quit to quit) -> ");  
   
    // Grab user input
    input = get_line();

    // The argument array
    char *argv[64];

    // Whether or not the process is background or not
    int bg = 0;

    // Parse the user input. Store the process arguments into argv
    parse(input, argv);

    
    // Scan through arguments
    for(i = 0; *argv[i] != '\0' ; ++i) {

      // Check if it is a background process
      if (*argv[i] == '&') {
	//printf("Background process\n");
	bg = 1;
	// Set the argument to null so that it is not interpereted 
	// as a parameter
	*argv[i] = '\0';
      }
      //printf("%d:%s\n", i, argv[i]);
    }


    // Execute the argument. Ignore blank lines
    // Supply a back-up quit statement in case signal handler fails.
    if (*argv[0] != '\0') {
      if (strcmp(argv[0], "quit") == 0) {
	free(input);
	exit(0);
      }
      execute(argv, bg);
    }

  }

  // Deallocate space for input buffer
  free(input);  

}

/**
 * Install the signal handler
 *
 */
void install_signal_handler() {
     
  // Install signal handlers
  struct sigaction signal_action;

  // No flags
  signal_action.sa_flags = 0;

  // Set up the signal handler
  signal_action.sa_handler = catch_signal;

  // No masked interrupts
  sigemptyset(&signal_action.sa_mask);

  // Set up signals to be associated with signal_action structure
  int i;
  for(i = 0; i < 32; ++i) {
    sigaction(i, &signal_action, NULL); 
  }

}


/**
 * Handle signals. Captures the signal number
 *
 */
void catch_signal(int sig_num) {

  // Set the signal flag to the signal number caught
  signal_num = sig_num;

}



/**
 * Returns a string and handles signals and errors
 *
 * @return  The string
 */
char* get_line(void) {

  // Reset the error flag
  errno = 0;

  // Create a temporary char array
  char* char_buffer;

  // How many bytes have been read from stdin
  ssize_t bytes_read = 0;

  
  // Allocate space for the temporary character buffer
  if((char_buffer = malloc(BUFFER_SIZE)) == NULL) {
    perror("Could not allocate enough memory.");
    fprintf(stderr, "Could not allocate enough memory");
  }
  

  // Grab the input
  while(bytes_read <= 0) {

    // Read from STDIN
    bytes_read = read(STDIN_FILENO, char_buffer, BUFFER_SIZE);
  
    // Check for errors. Ignore "interrupted system calls."
    if (errno != 0 && errno != 4) {
      
      // Print the error
      fprintf(stderr, "\nEncountered error %d : %s\n", errno, strerror(errno));
      perror("Encountered error");
     
    } 

    
    // Check for signals. Ignore SIGCHLDs
    if (signal_num != 0 && signal_num != 18) {

      printf("\nSignal %d\n", &signal_num);

      if (signal_num == 2) {

	// Control-C

	printf("\nThe last few background processes:\n");

	// Print all of the background processes
	for(i = firstexec; i < 4; ++i) {
	  printf("child[%d] = %s\n", i, child[i]);
	}

	for(i = 0; i < firstexec; ++i) {
	  printf("child[%d] = %s\n", i, child[i]);
	}

      } else if (signal_num == 8) {

	// Control-D. Does not work.

	exit(0);

       } else {
     
	 // All other signals

	// Print the signal number and signal name to stderr
	fprintf(stderr, "\nSignal #%d caught: %s\n", signal_num, signal_name[signal_num] );
	perror("\nCaught signal.\n");
      
      }

      // Reset the signal flag
      signal_num = 0;

    } 

    // Maybe this catches control D????
    if (*char_buffer == EOF) {
      exit(0);
    }

    // Null-terminate the string
    *(char_buffer + bytes_read) = '\0';

  }

  // Free the space
  free(char_buffer);

  // Return the string
  return char_buffer;

}

/*
 * Parse user input
 *
 */
void  parse(char *input, char **argv) {

  // Iterate through the line
  while (*input != '\0') {    

    // Replace whitespace with \0s
    while (*input == ' ' || *input == '\n' || *input == '\n') {
      *input++ = '\0';    
    }

    // Found new argument. Save position
    *argv++ = input;         

    // Skip whitespace and nulls
    while (*input != ' ' && *input != '\n' && *input != '\t' && *input != '\0') {
      input++;          
    }

  }

  // Null terminate the arguments
  *argv = '\0';             

}


/*
 * Execute a command
 *
 */
void  execute(char **argv, int background) {

  // printf("*%s*\n", argv[0]);


  // If the process is a background process, store in the table
  if (background) {

    // Clear the current info
    strcpy(child[lastchild], "");

    // Store the process name and arguments
    for(i = 0; *argv[i] != '\0'; ++i) {
      strcat(child[lastchild], argv[i]);
      strcat(child[lastchild], " ");
    }

    // Increment the child index and clear info
    ++lastchild;

    if (full) {
      ++firstexec;
      if (firstexec == 4) {
	firstexec = 0;
      }
    }

    // If over 4, roll back to one
    if (lastchild == 4) {
      lastchild = 0;
      full = 1;
    }

  }

  // The process id
  pid_t  pid;

  // Fork a child process
  pid = fork();

  if (pid  < 0) {  
    // Could not fork new process
    fprintf(stderr, "\nCould not fork a new process!\n");
    perror("\nCould not fork a new process!\n");
    exit(1);
  } else if (pid == 0) {
    // The child process  
  
    /**
    for(i = 0; *argv[i] != '\0'; ++i) {
      printf("%d: %s\n", i, argv[i]);
    }
    */

    // Execute the command
    if (execvp(*argv, argv) < 0) {   
      // Could not fork new process
      fprintf(stderr, "\nCould not execute the command!");
      perror("\nCould not execute the command!\n");
    } 

    if (! background) {
      exit(0);
    }


  } else {
    // In parent

    // Wait for completion of child if foreground process
    if (! background) {

      // The process status
      int status;

      // Block until the child has completed
      while (wait(&status) != pid);

     }

  }

}
