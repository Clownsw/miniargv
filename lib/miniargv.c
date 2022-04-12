#include "miniargv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#if defined(_MSC_VER) || (defined(__MINGW32__) && !defined(__MINGW64__))
#define strcasecmp _stricmp
#endif

#define MINIARG_PROCESS_FLAG_FLAGS      0x01
#define MINIARG_PROCESS_FLAG_VALUES     0x02
#define MINIARG_PROCESS_FLAG_BOTH       (MINIARG_PROCESS_FLAG_FLAGS | MINIARG_PROCESS_FLAG_VALUES)
#define MINIARG_PROCESS_FLAG_FIND_ONLY  0x08
#define MINIARG_PROCESS_FLAG_FIND_VALUE (MINIARG_PROCESS_FLAG_FIND_ONLY | MINIARG_PROCESS_FLAG_VALUES)

int miniargv_process_partial (unsigned int flags, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int i;
  size_t l;
  const miniargv_definition* current_argdef;
  int success;
  int standalonevaluedefset = 0;
  const miniargv_definition* standalonevaluedef = NULL;
  for (i = ((flags & MINIARG_PROCESS_FLAG_FIND_ONLY) == 0 ? 1 : *(int*)callbackdata + 1); argv[i]; i++) {
    success = 0;
    if (argv[i][0] == '-' && argv[i][1]) {
      if (argv[i][1] != '-') {
        //short argument
        current_argdef = argdef;
        while (!success && current_argdef->callbackfn) {
          if (current_argdef->shortarg) {
            if (argv[i][1] == current_argdef->shortarg) {
              if (!current_argdef->argparam) {
                //without value
                if (argv[i][2] == 0) {
                  if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                    success++;
                  else if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                    success++;
                }
              } else if (argv[i][2] != 0) {
                //with value and no space
                if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                  success++;
                else if ((current_argdef->callbackfn)(current_argdef, argv[i] + 2, callbackdata) == 0)
                  success++;
              } else if (argv[i + 1]) {
                //with value and space
                i++;
                if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                  success++;
                else if ((current_argdef->callbackfn)(current_argdef, argv[i], callbackdata) == 0)
                  success++;
              }
              break;
            }
          }
          current_argdef++;
        }
      } else {
        //long argument
        current_argdef = argdef;
        while (!success && current_argdef->callbackfn) {
          if (current_argdef->longarg) {
            l = strlen(current_argdef->longarg);
            if (strncmp(argv[i] + 2, current_argdef->longarg, l) == 0 && (argv[i][2 + l] == 0 || argv[i][2 + l] == '=')) {
              if (!current_argdef->argparam) {
                //without value
                if (argv[i][2 + l] == 0) {
                  if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                    success++;
                  else if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                    success++;
                }
              } else if (argv[i][2 + l] == '=') {
                //with value
                if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                  success++;
                else if ((current_argdef->callbackfn)(current_argdef, argv[i] + 3 + l, callbackdata) == 0)
                  success++;
              }
              break;
            }
          }
          current_argdef++;
        }
      }
    } else {
      //standalone value argument
      if (!standalonevaluedefset) {
        current_argdef = argdef;
        while (!success && current_argdef->callbackfn) {
          if (!current_argdef->shortarg && !current_argdef->longarg) {
            standalonevaluedef = current_argdef;
            break;
          }
          current_argdef++;
        }
        standalonevaluedefset = 1;
      }
      if (standalonevaluedef && standalonevaluedef->callbackfn) {
        if ((flags & MINIARG_PROCESS_FLAG_VALUES) == 0)
          success++;
        else if ((flags & MINIARG_PROCESS_FLAG_FIND_ONLY) != 0)
          return i;
        else if ((standalonevaluedef->callbackfn)(standalonevaluedef, argv[i], callbackdata) == 0)
          success++;
      }
    }
    if (!success && badfn) {
      //bad argument
      if (badfn && (badfn)(NULL, argv[i], callbackdata) == 0)
        success++;
      else if ((flags & MINIARG_PROCESS_FLAG_FIND_ONLY) != 0)
        return -1;
      else
        return i;
    }
    if (!success) {
      if ((flags & MINIARG_PROCESS_FLAG_FIND_ONLY) != 0)
        return -1;
      fprintf(stderr, "Invalid command line argument: %s\n", argv[i]);
      return i;
    }
  }
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_process (char* argv[], char* env[], const miniargv_definition argdef[], const miniargv_definition envdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int result = 0;
  if (env)
    result = miniargv_process_env(env, envdef, callbackdata);
  if (argv) {
    if (result == 0)
      result = miniargv_process_partial(MINIARG_PROCESS_FLAG_FLAGS, argv, argdef, badfn, callbackdata);
    if (result == 0)
      result = miniargv_process_partial(MINIARG_PROCESS_FLAG_VALUES, argv, argdef, badfn, callbackdata);
  }
  return result;
}

DLL_EXPORT_MINIARGV int miniargv_process_ltr (char* argv[], char* env[], const miniargv_definition argdef[], const miniargv_definition envdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int result = 0;
  if (env)
    result = miniargv_process_env(env, envdef, callbackdata);
  if (result == 0 && argv)
    result = miniargv_process_arg(argv, argdef, badfn, callbackdata);
  return result;
}

DLL_EXPORT_MINIARGV int miniargv_process_arg (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_FLAG_BOTH, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_process_arg_flags (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_FLAG_FLAGS, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_process_arg_params (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_FLAG_VALUES, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_get_next_arg_param (int argindex, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn)
{
  return miniargv_process_partial(MINIARG_PROCESS_FLAG_FIND_VALUE, argv, argdef, badfn, &argindex);
}

DLL_EXPORT_MINIARGV int miniargv_process_env (char* env[], const miniargv_definition envdef[], void* callbackdata)
{
  char* s;
  char** current_env;
  const miniargv_definition* current_envdef = envdef;
  int result;
  while (current_envdef->callbackfn) {
    if (current_envdef->longarg) {
      current_env = env;
      while (*current_env) {
        if ((s = strchr(*current_env, '=')) != NULL) {
          if (strncmp(*current_env, current_envdef->longarg, s - *current_env) == 0) {
            if ((result = (current_envdef->callbackfn)(current_envdef, s + 1, callbackdata)) != 0)
              return result;
          }
        }
        current_env++;
      }
    }
    current_envdef++;
  }
  return 0;
}

#define READLINE_BLOCK_SIZE 128

typedef int (*cfg_value_fn)(const char* varname, const char* value, void* callbackdata);

char* readline (FILE* src)
{
  int datalen;
  char data[READLINE_BLOCK_SIZE];
  int resultlen = 0;
  char* result = NULL;
  //read next data
  while (fgets(data, sizeof(data), src)) {
    datalen = strlen(data);
    //allocate memory and store the result
    result = (char*)realloc(result, resultlen + datalen + 1);
    memcpy(result + resultlen, data, datalen + 1);
    resultlen += datalen;
    //check for line end, if found remove it and return result
    if (resultlen > 0 && result[resultlen - 1] == '\n') {
      result[--resultlen] = 0;
      if (resultlen > 0 && result[resultlen - 1] == '\r')
        result[--resultlen] = 0;
      break;
    }
  }
  return result;
}

DLL_EXPORT_MINIARGV int miniargv_process_cfgfile (const char* cfgfile, const miniargv_definition cfgdef[], void* callbackdata)
{
  FILE* src;
  char* line;
  char* p;
  char* varname;
  size_t varnamelen;
  char* value;
  const miniargv_definition* current_cfgdef;
  int status = 0;
  //open file for reading
  if ((src = fopen(cfgfile, "rb")) != NULL) {
    //read lines
    while (status == 0 && (line = readline(src)) != NULL) {
      varname = line;
      //skip spaces preceding varname
      while (*varname && isspace(*varname))
        varname++;
      if (*varname) {
        //find starting position of value
        p = varname;
        while (*p && *p != '=' && *p != ':' && *p != '#' && *p != ';')
          p++;
        if (*p == '=' || *p == ':') {
          value = p + 1;
          //skip spaces following varname
          while (p != varname && isspace(*(p - 1)))
            p--;
          if (p != varname) {
            varnamelen = p - varname;
            //skip spaces preceding value
            while (*value && isspace(*value))
              value++;
            //skip spaces following value
            if ((p = strchr(value, 0)) != NULL) {
              while (p != value && isspace(*(p - 1)))
                p--;
              *p = 0;
            }
            //process variable
            current_cfgdef = cfgdef;
            while (current_cfgdef->callbackfn) {
              if (current_cfgdef->longarg) {
                if (strncmp(varname, current_cfgdef->longarg, varnamelen) == 0) {
                  status = (current_cfgdef->callbackfn)(current_cfgdef, value, callbackdata);
                  break;
                }
              }
              current_cfgdef++;
            }
          }
        }
      }
      free(line);
    }
    fclose(src);
  }
  return 0;
}

DLL_EXPORT_MINIARGV void miniargv_cfgfile_generate (FILE* cfgfile, const miniargv_definition cfgdef[])
{
  const miniargv_definition* current_cfgdef = cfgdef;
  while (current_cfgdef->callbackfn) {
    if (current_cfgdef->longarg) {
      fprintf(cfgfile, "; %s\n;   %s\n%s = %s\n", current_cfgdef->longarg, current_cfgdef->help, current_cfgdef->longarg, (current_cfgdef->argparam ? current_cfgdef->argparam : ""));
    }
    current_cfgdef++;
  }
}

DLL_EXPORT_MINIARGV const char* miniargv_getprogramname (const char* argv0, int* length)
{
  int pos;
  int len;
  if (!argv0 || !*argv0) {
    if (length)
      *length = 0;
    return NULL;
  }
  //find beginning of filename in path
  len = strlen(argv0);
  pos = len;
  while (pos > 1 && argv0[pos - 1] != '/'
#ifdef _WIN32
    && argv0[pos - 1] != '\\' && argv0[pos - 1] != ':'
#endif
  ) {
    pos--;
  }
  //skip extension
  if (length) {
#ifdef _WIN32
    if (len >= 4 && strcasecmp(argv0 + len - 4, ".exe") == 0)
      len -= 4;
#endif
    len = len - pos;
    *length = len;
  }
  return argv0 + pos;
}

DLL_EXPORT_MINIARGV void miniargv_arg_list (const miniargv_definition argdef[], int shortonly)
{
  const miniargv_definition* current_argdef = argdef;
  while (current_argdef->callbackfn) {
    if (current_argdef != argdef)
      printf(" ");
    if (current_argdef->shortarg  || current_argdef->longarg) {
      printf("[");
      if (current_argdef->shortarg) {
        printf("-%c", current_argdef->shortarg);
        if (current_argdef->argparam)
          printf(" %s", current_argdef->argparam);
      }
      if (current_argdef->longarg && !(shortonly && current_argdef->shortarg)) {
        if (current_argdef->shortarg)
          printf("|");
        printf("--%s", current_argdef->longarg);
        if (current_argdef->argparam)
          printf("=%s", current_argdef->argparam);
      }
      printf("]");
    } else {
      printf("%s", (current_argdef->argparam ? current_argdef->argparam : "param"));
    }
    current_argdef++;
  }
}

DLL_EXPORT_MINIARGV void miniargv_env_list (const miniargv_definition envdef[], int noparam)
{
  const miniargv_definition* current_envdef = envdef;
  while (current_envdef->callbackfn) {
    if (current_envdef != envdef)
      printf(" ");
    if (current_envdef->longarg) {
      printf("%s", current_envdef->longarg);
      if (!noparam && current_envdef->argparam)
        printf("=%s", current_envdef->argparam);
    }
    current_envdef++;
  }
}

DLL_EXPORT_MINIARGV void miniargv_arg_help (const miniargv_definition argdef[], int descindent, int wrapwidth)
{
  int pos;
  const miniargv_definition* current_argdef = argdef;
  if (!descindent)
    descindent = 25;
  while (current_argdef->callbackfn) {
    pos = printf("  ");
    if (!current_argdef->shortarg && !current_argdef->longarg) {
      pos += printf("%s", (current_argdef->argparam ? current_argdef->argparam : "param"));
    } else {
      if (current_argdef->shortarg) {
        pos += printf("-%c", current_argdef->shortarg);
        if (current_argdef->argparam)
          pos += printf(" %s", current_argdef->argparam);
      }
      if (current_argdef->longarg) {
        if (current_argdef->shortarg)
          pos += printf(", ");
        pos += printf("--%s", current_argdef->longarg);
        if (current_argdef->argparam)
          pos += printf("=%s", current_argdef->argparam);
      }
    }
    if (pos > descindent - 2)
      printf("\n%*s", descindent, "");
    else
      printf("%*s", (pos < descindent ? descindent - pos : 2), "");
    miniargv_wrap_and_indent_text(current_argdef->help, descindent, descindent, wrapwidth);
    printf("\n");
    current_argdef++;
  }
}

DLL_EXPORT_MINIARGV void miniargv_env_help (const miniargv_definition envdef[], int descindent, int wrapwidth)
{
  int pos;
  const miniargv_definition* current_envdef = envdef;
  if (!descindent)
    descindent = 25;
  while (current_envdef->callbackfn) {
    pos = printf("  ");
    if (!current_envdef->shortarg && !current_envdef->longarg) {
      pos += printf(" %s", (current_envdef->argparam ? current_envdef->argparam : "param"));
    } else {
      if (current_envdef->longarg) {
        if (current_envdef->shortarg)
          pos += printf(", ");
        pos += printf("%s", current_envdef->longarg);
        if (current_envdef->argparam)
          pos += printf("=%s", current_envdef->argparam);
      }
    }
    if (pos > descindent - 2)
      printf("\n%*s", descindent, "");
    else
      printf("%*s", (pos < descindent ? descindent - pos : 2), "");
    miniargv_wrap_and_indent_text(current_envdef->help, descindent, descindent, wrapwidth);
    printf("\n");
    current_envdef++;
  }
}

DLL_EXPORT_MINIARGV void miniargv_help (const miniargv_definition argdef[], const miniargv_definition envdef[], int descindent, int wrapwidth)
{
  if (argdef) {
    printf("Command line arguments:\n");
    miniargv_arg_help(argdef, descindent, wrapwidth);
  }
  if (envdef) {
    printf("Environment variables:\n");
    miniargv_env_help(envdef, descindent, wrapwidth);
  }
}

DLL_EXPORT_MINIARGV void miniargv_wrap_and_indent_text (const char* text, int currentpos, int indentpos, int wrapwidth)
{
  const char* p;
  const char* q;
  const char* r;
  //set default wrap with if not specified
  if (!wrapwidth)
    wrapwidth = 79;
  p = text;
  //process text
  while (p) {
    q = p;
    r = p;
    //find wrap position
    do {
      //find first blank space
      while (*r && !isspace(*r))
        r++;
      //check if wrap position was exceeded
      if (currentpos + (r - p) > wrapwidth) {
        //if single word exceeds maximum width show it as a whole anyway
        if (q == p)
          q = r;
        break;
      }
      q = r;
      //wrap if line break was found
      if (*r == '\n')
        break;
      //skip blank space
      while (*r && isspace(*r))
        r++;
    } while (*r);
    //print line
    if (!*q) {
      //print as a whole if last section of text
      printf("%s", p);
      p = NULL;
    } else {
      //print partial text, go to new line and indent
      printf("%.*s\n%*s", (int)(q - p), p, indentpos, "");
      currentpos = indentpos;
      //skip blank space
      p = q;
      while (isspace(*p))
        p++;
    }
  }
}



DLL_EXPORT_MINIARGV int miniargv_cb_set_const_str (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(const char**)argdef->userdata = value;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_strdup (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  if (*(char**)argdef->userdata)
    free(*(char**)argdef->userdata);
  *(char**)argdef->userdata = (value ? strdup(value) : NULL);
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  long intval;
  char* end = NULL;
  if (!value || !*value)
    return 1;
  intval = strtol(value, &end, 10);
  if (!end || *end)
    return 2;
  if (intval == LONG_MIN)
    return 3;
  if (intval == LONG_MAX)
    return 4;
  *(int*)argdef->userdata = (int)intval;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_zero (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(int*)argdef->userdata = 0;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(int*)argdef->userdata = 1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_minus_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(int*)argdef->userdata = -1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_increment_int (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((int*)argdef->userdata))++;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_decrement_int (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((int*)argdef->userdata))--;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  long intval;
  char* end = NULL;
  if (!value || !*value)
    return 1;
  intval = strtol(value, &end, 10);
  if (!end || *end)
    return 2;
  if (intval == LONG_MIN)
    return 3;
  if (intval == LONG_MAX)
    return 4;
  *(long*)argdef->userdata = intval;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_zero (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(long*)argdef->userdata = 0;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(long*)argdef->userdata = 1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_minus_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(long*)argdef->userdata = -1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_increment_long (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((long*)argdef->userdata))--;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_decrement_long (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((long*)argdef->userdata))++;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_noop (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_error (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  if (argdef->userdata)
    fprintf(stderr, "%s\n", (const char*)argdef->userdata);
  return -1;
}



DLL_EXPORT_MINIARGV void miniargv_get_version (int* pmajor, int* pminor, int* pmicro)
{
  if (pmajor)
    *pmajor = MINIARGV_VERSION_MAJOR;
  if (pminor)
    *pminor = MINIARGV_VERSION_MINOR;
  if (pmicro)
    *pmicro = MINIARGV_VERSION_MICRO;
}

DLL_EXPORT_MINIARGV const char* miniargv_get_version_string ()
{
  return MINIARGV_VERSION_STRING;
}

