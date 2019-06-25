/**CTIME**********************************************************************
  File        ctime.c
  Resume      Converts Unix timestamp to human readable time
  Autor       Raúl San Martín Aniceto
  Copyright (c) 2019 Raúl San Martín Aniceto
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define NUM_ERROR_MSGS 9

#define SUCCESS 0
#define GENERAL_ERROR -1
#define NO_VALID_TIMEZONE -2
#define NO_TIMEZONE_SPECIFIED -3
#define UNKNOWN_OPTION -4
#define TOO_MANY_ARGS -5
#define TOO_LESS_ARGS -6
#define NO_DIGITS_FOUND -7
#define UNKNOWN -8

static const char *error_map[NUM_ERROR_MSGS] = {
  "Success",
  "General error",
  "is not a valid timezone",
  "Timezone was not specified",
  "is not a recognized option",
  "Too many arguments",
  "Too less arguments",
  "No digits were found",
  "Unknown error"
};

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

typedef struct{
  int help;
  int version;
  int ms;
  int tz;
  char* tz_str;
  int no_valid_optn;
}args_t;

static struct option long_options[] = {
  {"help",    no_argument,       0, 'h'},
  {"version", no_argument,       0, 'v'},
  {"ms",      no_argument,       0,  0 },
  {"tz",      required_argument, 0,  0 },
  {0, 0, 0, 0}
};

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

int n_args = 0;
char **args_str = NULL;

int error = 0;
char *str_error_opt = NULL;

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

void print_help(char *program_name);

void print_version();

args_t process_args(int num, char **arguments);

const char *get_error_msg(void);

int print_error(void);

/*---------------------------------------------------------------------------*/
/* Main                                                                      */
/*---------------------------------------------------------------------------*/

int main(int argc, char **argv){
  n_args = argc;
  args_str = argv;
  
  int min_num_args = 1;
  
  args_t arguments = process_args(argc, argv);
  
  if(opterr || error){
    if(errno){
      error = errno;
    }
    return print_error();
  }

  if(arguments.help){
    print_help(argv[0]);
    return error;
  }

  if(arguments.version){
    print_version();
    return error;
  }

  if(arguments.ms){
    min_num_args++;
  }if(arguments.tz){
    min_num_args += 2;
  }
    
  if(argc > min_num_args + 1){
    error = TOO_MANY_ARGS;
  }else if(argc < min_num_args){
    error = TOO_LESS_ARGS;
  }
  
  if(error){
    return print_error();
  }
  
  time_t now;
  struct tm ts;
  char buf[80];
  
  char *str, *endptr;
  
  if(argv[optind] == NULL){
    time(&now);
  }else{
    str = argv[optind];
    now = strtol(str, &endptr, 10);
    
    if(errno){
      error = errno;
    }else if(endptr == str){
      error = NO_DIGITS_FOUND;
    }
    
    if(error){
      return print_error();
    }
    
    if(arguments.ms){
      now/=1000;
    }
  }
  
  ts = *localtime(&now);
  strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
  printf("%s\n", buf);
  return error;
}

/*---------------------------------------------------------------------------*/
/* Static function definitions                                               */
/*---------------------------------------------------------------------------*/

void print_help(char *program_name){
    printf("Usage: %s [OPTION]... [TIMESTAMP]\n", program_name);
    printf("Converts Unix timestamp to human readable time.\n\n");
    printf("With no TIMESTAMP prints the actual time.\n\n");
    printf("Options:\n");
    printf("\t-h, --help           Displays this help and exit\n");
    printf("\t-v, --version        Outputs version information and exit\n");
    printf("\t    --ms             When TIMESTAMP is in miliseconds\n");
    printf("\t    --tz TIMEZONE    Outputs the date in the corresponding TIMEZONE according to tz database https://en.wikipedia.org/wiki/List_of_tz_database_time_zones\n");
    printf("\t                     If TIMEZONE is not specified, the default system timezone will be used.\n\n");
}

void print_version(){
  printf("ctime v0.6 ");
  printf("Copyright (c) 2019 Raúl San Martín Aniceto.\n\n");
  printf("MIT License\n");
  printf("<https://opensource.org/licenses/MIT>.\n");
  printf("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND\n");
}

args_t process_args(int num, char **arguments){
  args_t result;
  int option_index;
  int c;
  opterr = 0;

  result.help = 0;
  result.version = 0;
  result.ms = 0;
  result.tz = 0;
  result.no_valid_optn = 0;

  while((c = getopt_long(num, arguments,"hv",long_options,
            &option_index)) != -1){
    switch(c){
      case 'h':
        result.help = 1;
      break;

      case 'v':
        result.version = 1;
      break;
      
      case 0:
        if(!strcmp(long_options[option_index].name, "ms")){
          result.ms = 1;
        }else if(!strcmp(long_options[option_index].name, "tz")){
          result.tz = 1;
          result.tz_str = optarg;
          setenv( "TZ", result.tz_str, 1 );
          tzset();
          if(errno){
            error = NO_VALID_TIMEZONE;
            errno = 0;
            str_error_opt = optarg;
          }
        }
      break;

      case '?':
        error = UNKNOWN_OPTION;
        if(!strcmp(args_str[optind - 1], "--tz")){
          error = NO_TIMEZONE_SPECIFIED;
        }else{
          str_error_opt = args_str[optind - 1];
        }
        result.no_valid_optn = optind - 1;
      break;

      default:
        abort();
    }
  }
  return result;
}

const char *get_error_msg(void){
  if(error <= 0){
    if(-error < NUM_ERROR_MSGS){
      return error_map[-error];
    }
    return error_map[NUM_ERROR_MSGS - 1];
  }else{
    return strerror(error);
  }
}

int print_error(void){
  if(str_error_opt != NULL){
    fprintf(stderr, "Error: %s %s.\n", str_error_opt, get_error_msg());
  }else{
    fprintf(stderr, "Error: %s.\n", get_error_msg()); 
  }
  fprintf(stderr, "Try '%s --help' for more information.\n", args_str[0]);
  return error;
}