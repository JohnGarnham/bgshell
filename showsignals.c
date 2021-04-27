/**
 * showsignals.c
 * 
 * Command input and signal handling.
 *
 * @version: $Id: showsignals.c,v 1.5 2007/09/19 01:02:31 jeg3600 Exp jeg3600 $
 *
 * @author: John Garnham
 *
 * Revisions:
 *       $Log: showsignals.c,v $
 *       Revision 1.5  2007/09/19 01:02:31  jeg3600
 *       Added signal messages
 *
 *       Revision 1.4  2007/09/17 14:56:24  jeg3600
 *       Correctly set up signal handler
 *
 *       Revision 1.3  2007/09/16 17:34:13  jeg3600
 *       Added error checking.
 *
 *       Revision 1.2  2007/09/16 16:07:07  jeg3600
 *       *** empty log message ***
 *
 *
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>

#define BUFFER_SIZE 256


void catch_signal(int sig_num);

char* get_line(void);


// The signal number
int signal_num = 0;

char* signal_name[] = {
  "", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT",
  "SIGEMT", "SIGFPE", "SIGKILL", "SIGBUS", "SIGSEGV", "SIGSYS",
  "SIGPIPE", "SIGALRM", "SIGTERM", "SIGUSR1", "SIGUSR2", "SIGCHLD",
  "SIGPWR", "SIGWINCH", "SIGURG", "SIGPOLL", "SIGSTOP", "SIGTSTP",
  "SIGCONT", "SIGTTIN", "SIGTTOU", "SIGVTALRM", "SIGPROF", 
  "SIGXCPU", "SIFWAITING"};


int main() {

  char* input;

  if((input = malloc(BUFFER_SIZE)) == NULL) {
    perror("Could not allocate enough memory.");
  }
     
  // Install signal handlers
     
  struct sigaction signal_action;

  // No flags
  signal_action.sa_flags = 0;

  signal_action.sa_handler = catch_signal;

  // No masked interrupts
  sigemptyset(&signal_action.sa_mask);

  // Set up signals to be associated with signal_action structure
  int i;
  for(i = 0; i < 32; ++i) {
    sigaction(i, &signal_action, NULL);
  }

  // Main program loop

  while(1) {
    
    printf("Waiting for input (ctl-d to quit) ->");
    
    input = get_line();
    
    printf("\nReceived %d bytes: %s\n", strlen(input), input);
    
  }

  free(input);  

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

  // Create a temporary char array
  char* char_buffer;

  // The current character read
  char current;

  // The current index in the buffer
  int index = 0;

  // Loop while there is still buffer space left 
  // and the character is not a carriage return.
  do {

    current = getchar();

    // Check for errors
    if (errno != 0) {
      
      // Print the error
      fprintf(stderr, "\nEncountered error %d : %s", errno, strerror(errno));

      // Reset errno
      errno = 0;

    } 

    // Check for signals
    if (signal_num != 0) {

      // Print the signal number and signal name to stderr
      fprintf(stderr, "\nSignal #%d caught: %s", signal_num, signal_name[signal_num] );

      // Reset the signal flag
      signal_num = 0;

    } else {

      // Add the character to the string as long as it isn't a character return
      if (current != '\n') {
	*(char_buffer + index++) = current;
      }

    }

    // Hack to control for ^D
   
   
  } while (index < BUFFER_SIZE && current != '\n');

  printf("**********\n");

  // Return the string
  return char_buffer;

}
