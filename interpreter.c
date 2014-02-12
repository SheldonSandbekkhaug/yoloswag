/* Author(s): Sheldon Sandbekkhaug
              Ryan Lorey
   Written 2013

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "interpreter.h"


#define starting_buffer_size 32
char delimeters[16];

FILE *input;
unsigned long long line_number = 0;
bool debug = FALSE;


Variable *memory; // "Virtual" memory. address 0 is NULL
unsigned long long memory_size = 1024; // Default memory size
unsigned long long active_address; // Location in memory of the variable to modify
unsigned long long stack_pointer = 1;
char adjustment;


int main(int argc, STRING *argv)
{
    int i;
    bool filenamegiven = FALSE;

    delimeters[0] = ' ';
    delimeters[1] = '\t';
    delimeters[2] = '\n';
    delimeters[3] = '\0';

    /* main driver program.  Define the input file
       from either standard input or a name on the
       command line.  Process all arguments. */
    for (i = 1; i < argc; i++)
    {
      argv++;
      // Scan any command line arguments
      if (**argv == '-')
        scan_args(*argv);
      else
      {
        // If we found a real file, open it
        filenamegiven = TRUE;
        input = fopen(*argv,"r");

        if (input == NULL)
        {
          filenamegiven = FALSE;
        }
      }
    }

    memory = malloc(memory_size * sizeof(Variable));
    check_malloc(memory);

    // Use stdin if no valid filename was given
    if (!filenamegiven)
    {
      input = stdin;
      read_input(stdin);
    }
    else
    {
      read_input(input);
      fclose(input);
    }


    // TODO: Write a function to free everything in memory
/*    for(stack_pointer = stack_pointer - 1; stack_pointer >= 0; stack_pointer--)
    {
      printf("freeing %x\n", &memory[stack_pointer]);
      if (&memory[stack_pointer] != NULL)
      {
        printf("name: %s\n", memory[stack_pointer].name);
        free(memory[stack_pointer].name);
      }
    }
*/
    free(memory);

    exit(0);
}


void scan_args(STRING s)
{
    /* check each character of the option list for
       its meaning. */

    while (*++s != '\0')
        switch (*s)
            {
            case 'M': // Maximum memory usage for client program data
                s = s + 1;
                memory_size = atoi(s);

                // Stop loop from reading the rest of this argument
                s[1] = '\0'; 
                break;
            case 'D': /* debug option */
                debug = TRUE;
                break;

            default:
                fprintf (stderr,"PROGR: Bad option %c\n", *s);
                fprintf (stderr,"usage: PROGR [-D] file\n");
                exit(-1);
            }
}



void read_input(FILE *source)
{
  /* Read a line from source and process it. */
  STRING buffer;
  STRING temp_buffer;
  unsigned long long int buffer_size; // Current size of the malloc'd space
  unsigned long long int line_length; // Number of characters on a line
  int c = '\0';

  buffer = malloc(sizeof(char) * starting_buffer_size);
  check_malloc(buffer);
  line_length = 0;
  buffer_size = starting_buffer_size;

  while(c != EOF)
  {
    c = getc(source);

    // Increment line_number every line
    if (c == '\n')
    {
      line_number++;
    }

    // Process the line once we hit '#'
    if (c == '#')
    {
      buffer[line_length] = '\0';

      // Don't process the line unless something is in it
      if (line_length > 0) process_line(buffer);
      free(buffer);

      // Create a new buffer for the next line
      buffer = NULL;
      buffer = malloc(sizeof(char) * starting_buffer_size);
      check_malloc(buffer);
      buffer_size = starting_buffer_size;
      buffer[0] = '\0'; // Just in case there is nothing on the line
      line_length = 0;
    }
    else if (c == EOF)
    {
      buffer[line_length] = '\0';
      process_line(buffer);
      free(buffer);
    }
    else
    {
      if (line_length >= buffer_size-1)
      {
        // Increase the size of the buffer
        buffer = realloc(buffer, 2 * (line_length+1) * sizeof(char));
        check_malloc(buffer);
        buffer_size = 2 * (line_length + 1);
      }

      // Add c to the buffer
      buffer[line_length] = c;
      line_length++;
    }
  }
}


void process_line(STRING line)
{
  /* Parse a line and do commands.  */
  unsigned long long line_size = strlen(line);
  STRING token;
  STRING uppercase_token;
  STRING next_token;
  STRING variable_name;
  unsigned long long temp;
  unsigned char printable; // For HOLLA

  token = malloc(line_size);
  check_malloc(token);

  uppercase_token = malloc(line_size);
  check_malloc(uppercase_token);

  next_token = malloc(line_size);
  check_malloc(next_token);

//  printf("Processing the following line: %s\n", line);
  
  // Get the first token on the line
  token = strtok(line, delimeters);

  if (token == NULL)
  {
    // Don't do anything if no valid token was found
    return;
  }
  else
  {
    convert_to_upper(token, uppercase_token);
  }

  // Do something with the token
  if (are_equivalent(uppercase_token, "YOLO"))
  {
    // YOLO means to declare an 8-bit variable

    // Get the next token. It should be a variable name.
    next_token = strtok(NULL, delimeters);

    // Check if the Variable with name token exists
    temp = variable_address(token);
    if (temp == 0)
    {
      // Create new Variable
      //printf("creating new variable named %s\n", next_token);
      Variable *v = malloc(sizeof(Variable));
      check_malloc(v);

      variable_name = malloc(sizeof(char) * strlen(next_token));
      check_malloc(variable_name);
      strcpy(variable_name, next_token);
      v->name = variable_name;

      v->data = 0;

      // Add a new value to memory and increment the stack pointer
      active_address = stack_pointer;
      memory[active_address] = *v;

      stack_pointer++;
    }
    else
    {
      fprintf(stderr, "Multiple declaration of %s at line %llu\n",
              next_token, line_number);
    }
  }
  else if (are_equivalent(uppercase_token, "SWAG"))
  {
    // Increment variable
    adjustment++;
    do_adjustment(line_size);

//    memory[active_address].data = memory[active_address].data + 1;
//    printf("DATA: %c\n", memory[active_address].data);

  }
  else if (are_equivalent(uppercase_token, "RATCHET"))
  {
//    printf("doing ratchet\n");
    // Decrement variable
    adjustment--;
    do_adjustment(line_size);
  }
  else if (are_equivalent(uppercase_token, "HOLLA"))
  {
    // Print variable as ASCII character

    next_token = strtok(NULL, delimeters);

    // Check if the Variable with name token exists
    temp = variable_address(next_token);
    active_address = temp;
    if (temp != 0)
    {
      // The variable exists, so we can print it
      printable = (unsigned char)memory[active_address].data;
      printable = printable & 0x000000FF;
      printf("%c", printable);
    }
    else
    {
      // The variable does not exist
      fprintf(stderr, "ERROR: undefined variable \"%s\" at line %llu\n",
              next_token, line_number);
      exit(-1);
    }
  }
  else if (are_equivalent(uppercase_token, "HOLLANUMBER"))
  {
    // Print variable as a number
    next_token = strtok(NULL, delimeters);

    // Check if the Variable with name token exists
    temp = variable_address(next_token);
    active_address = temp;
    if (temp != 0)
    {
      // The variable exists, so we can print it
      printable = (unsigned char)memory[active_address].data;
      printable = printable & 0x000000FF;
      printf("%d", printable);
    }
    else
    {
      // The variable does not exist
      fprintf(stderr, "ERROR: undefined variable \"%s\" at line %llu\n",
              next_token, line_number);
      exit(-1);
    }
  }
  else
  {

/*    // It's a variable name
    // Check if the Variable with name token exists
  //  printf("about to check if variable exists\n");
    temp = variable_address(token);
    if (temp == 0)
    {
      // Create new Variable
      Variable *v = malloc(sizeof(Variable));
      check_malloc(v);
      v->name = token;
      v->data = 0;
      memory[active_address] = *v;
    }
    else
    {
      active_address = temp;
    }*/
  }

  adjustment = 0;

//  free(token);
//  free(uppercase_token);

  return;
}


void do_adjustment(unsigned long long line_size)
{
  /* Read a statement and set adjustment to the amount a variable will be
     adjusted. Stop reading when we hit a variable. */
  STRING token = malloc(line_size);
  STRING uppercase_token = malloc(line_size);

  check_malloc(token);
  check_malloc(uppercase_token);

  token = strtok(NULL, delimeters);
  convert_to_upper(token, uppercase_token);

  while (uppercase_token != NULL)
  {
    convert_to_upper(token, uppercase_token);

    if (are_equivalent(uppercase_token, "SWAG"))
    {
      adjustment++;
    }
    else if (are_equivalent(uppercase_token, "RATCHET"))
    {
      adjustment--;
    }
    else
    {
      // This token might be a variable
      active_address = variable_address(token);

      if (active_address != 0)
      {
        // Variable exists
        memory[active_address].data += adjustment;
      }
      else
      {
        // Variable did not exist
        fprintf(stderr, "ERROR: %s undeclared at line %llu\n", token, line_number);
        exit(-1);
      }
      adjustment = 0;
      return;
    }

    token = strtok(NULL, delimeters);
  }

  fprintf(stderr, "ERROR: No variable for line %llu found\n", line_number);
  adjustment = 0;
  exit(-1);
}


unsigned long long variable_address(STRING element)
{
  /* Return the address in virtual memory of the variable with name element.
     Return 0 if that element could not be found.
  
  */
  unsigned long long i = stack_pointer;
  for(; i > 0; i--)
  {
    if(memory[i].name != NULL && are_equivalent(element, memory[i].name))
    {
      return i;
    }
  }
  return 0;
}


bool are_equivalent(STRING s1, STRING s2)
{
  /* Returns TRUE if s1 and s2 are the same ASCII characters, case-sensitive.
     Returns FALSE otherwise.
  */
  int result;

  // Check if their lengths are the same
  if (strlen(s1) != strlen(s2))
  {
    return FALSE;
  }

  // Check if each byte is the same
  result = memcmp(s1, s2, strlen(s1));
  if (result == 0) // 0 means no characters differ
  {
    return TRUE;
  }
  return FALSE;
}


void convert_to_upper(STRING src, STRING dest)
{
  /* Convert every character in src to uppercase and store them in dest.

  */
  int i;
  for (i = 0; src[i] != '\0'; i++)
  {
    dest[i] = toupper(src[i]);
  }
}


bool check_malloc(void* pointer)
{
  /* Checks if pointer is NULL.
     If pointer is null, prints and error message, and quits.
     Else returns TRUE
  */
  if (pointer == NULL)
  {
    fprintf(stderr, "No more memory. Exiting...\n");
    exit(2);
  }
  return TRUE;
}
