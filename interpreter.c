/* Author: Sheldon Sandbekkhaug
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
Variable *memory;
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
    bool filenamenotgiven = TRUE;

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
        filenamenotgiven = FALSE;
        input = fopen(*argv,"r");

        if (input == NULL)
        {
          filenamenotgiven = TRUE;
        }
      }
    }

    // Use stdin if no valid filename was given
    if (filenamenotgiven)
    {
      input = stdin;
      read_input(stdin);
    }
    else
    {
      read_input(input);
      fclose(input);
    }


    // TODO: put in own function?
    for(;stack_pointer >= 0; stack_pointer--)
    {
      free(memory[stack_pointer].name);
      //free(&memory[stack_pointer]);
    }

    free(memory);

    exit(0);
}


void scan_args(STRING s)
{
    /* check each character of the option list for
       its meaning. */
    int size = 4096;

    while (*++s != '\0')
        switch (*s)
            {
            case 'M': // Maximum memory usage for client program data
                *s++;
                printf("%s\n", s); // TODO: remove
                size = atoi(s);
                memory = malloc(size * sizeof(Variable));
                check_malloc(memory);

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
        // Increase the size of the buffer
        buffer[line_length] = '\0';
        buffer = realloc(buffer, 2 * line_length * sizeof(char));
        check_malloc(buffer);
        buffer_size = 2 * line_length;
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
  STRING element = malloc(sizeof(line));
  STRING next_element = malloc(sizeof(line));
  check_malloc(element);
  printf("Processed the following line: %s\n", line);
  
  element = strtok(line, delimeters);

  while(element != NULL)
  {
    printf("element: %s\n", element);

    process_element(element);

    element = strtok(NULL, delimeters);
    if (next_element == NULL)
    {
      printf("next element is NULL\n");
      //printf("line: %s\n", line);
      element = NULL;
    }
  }

  return;
}


void process_element(STRING element)
{
  /* Do things with the element given.

  */
    if (are_equivalent(element, "YOLO"))
    {
      // Declare variable
      printf("doing yolo\n");
      // Add a new value to memory and increment the stack pointer
      active_address = stack_pointer;
      stack_pointer++;      
    }
    else if (are_equivalent(element, "SWAG"))
    {
      // Increment variable
      printf("doing swag\n");
    }
    else if (are_equivalent(element, "RATCHET"))
    {
      // Decrement variable
      printf("doing ratchet\n");
    }
    else if (are_equivalent(element, "HOLLA"))
    {
      // Print variable
      printf("doing holla\n");
      printf("active address: %c\n", memory[active_address].data);
    }
    else
    {
      // It's a variable name
      // Check if the variable exists
      active_address = variable_address(element);
      if (active_address == 0)
      {
        // Create new Variable
        // TODO: I think I have to alloc the struct first
        memory[active_address].name = element;
        memory[active_address].data = 0;

        // TODO: remove
        printf("var at active address: %s\n", memory[active_address].name);
        printf("var at active address: %d\n", memory[active_address].data);
      }

    }

}


//TODO
long long variable_address(STRING element)
{
  return 0;
}


bool are_equivalent(STRING s1, STRING s2)
{
  /* Returns TRUE if s1 and s2 are the same ASCII characters, case-insensitive.
     Returns FALSE otherwise.
  */
  int result;
  STRING s1_upper;
  STRING s2_upper;

  // Check if their lengths are the same
  if (strlen(s1) != strlen(s2))
  {
    return FALSE;
  }

  // Convert s1 to uppercase
  s1_upper = malloc(sizeof(s1));
  check_malloc(s1_upper);
  convert_to_upper(s1, s1_upper);

  // Don't convert s2 to uppercase, it should already be uppercase
  s2_upper = s2;

  // Check if each byte is the same
  result = memcmp(s1_upper, s2_upper, strlen(s1));
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
