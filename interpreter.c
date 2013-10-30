/* Author(s): Sheldon Sandbekkhaug
              Ryan Lorey
   Written 2013

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef char bool;
#define TRUE 1
#define FALSE 0

typedef char* STRING;
#define starting_buffer_size 32
char delimeters[16];

FILE *input;
bool debug = FALSE;

// An entry in memory
struct Variable
{
  char data;
  STRING name;
};

typedef struct Variable Variable;
Variable *memory; // "Virtual" memory
long long memory_size = 1024;
long long active_address; // Location in memory of the variable to modify
long long stack_pointer = 1;

void scan_args(STRING s);
void read_input(FILE *source);
void process_line(STRING line);
void process_element(STRING element);
long long variable_address(STRING element);
bool are_equivalent(STRING s1, STRING s2);
void convert_to_upper(STRING src, STRING dest);
bool check_malloc(void* pointer);


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
/*    for(stack_pointer = stack_pointer - 1;stack_pointer >= 0; stack_pointer--)
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
                *s++;
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
/* Read a line from source and process it.
*/
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

    // Process the line once we hit '#'
    if (c == '#')
    {
      printf("line length: %d\n", line_length); // TODO: remove
      buffer[line_length] = '\0';

      process_line(buffer);
      free(buffer);

      // Create a new buffer for the next line
      buffer = NULL;
      buffer = malloc(sizeof(char) * 8);
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
        printf("increasing buffer size from %d\n", buffer_size); //TODO: remove
        // Increase the size of the buffer
        buffer[line_length] = '\0';
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
  STRING element = malloc(strlen(line));
  STRING next_element = malloc(strlen(line));
  check_malloc(element);
  check_malloc(next_element);
  printf("Processed the following line: %s\n", line);
  
  element = strtok(line, delimeters);

  while(element != NULL)
  {
    printf("element: %s\n", element);
    printf("active_address: %x\n", active_address);

    process_element(element);

    element = strtok(NULL, delimeters);
    if (next_element == NULL)
    {
      element = NULL;
    }
  }

  return;
}


void process_element(STRING element)
{
  /* Do things with the element given.

  */
  STRING uppercase_element;
  long long temp;

  uppercase_element = malloc(sizeof(element));
  check_malloc(uppercase_element);
  convert_to_upper(element, uppercase_element);

    if (are_equivalent(uppercase_element, "YOLO"))
    {
      // Declare variable
      printf("doing yolo\n");
      // Add a new value to memory and increment the stack pointer
      active_address = stack_pointer;
      stack_pointer++;      
    }
    else if (are_equivalent(uppercase_element, "SWAG"))
    {
      // Increment variable
      printf("doing swag\n");
      memory[active_address].data = memory[active_address].data + 1;
      printf("DATA: %c\n", memory[active_address].data);
    }
    else if (are_equivalent(uppercase_element, "RATCHET"))
    {
      // Decrement variable
      printf("doing ratchet\n");
    }
    else if (are_equivalent(uppercase_element, "HOLLA"))
    {
      // Print variable
      printf("doing holla\n");
      printf("DATA: %c\n", memory[active_address].data);
    }
    else
    {
      // It's a variable name
      // Check if the Variable with name element exists
      printf("about to check if variable exists\n");
      temp = variable_address(element);
      if (temp == 0)
      {
        // Create new Variable
        Variable *v = malloc(sizeof(Variable));
        check_malloc(v);
        v->name = element;
        v->data = 0;
        memory[active_address] = *v;
        printf("data: %d\n", memory[active_address].data);
      }
      else
      {
        active_address = temp;
      }

    }

}


//TODO
long long variable_address(STRING element)
{
  /* Return the address in virtual memory of the variable with name element.
     Return 0 if that element could not be found.
  
  */
  long long i = stack_pointer;
  for(; i >= 0; i--)
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
