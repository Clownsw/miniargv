/**
 * @file miniargv.h
 * @brief miniargv library header file with main functions
 * @author Brecht Sanders
 *
 * The miniargv library C library is a lightweight cross-platform C/C++ library for processing command line arguments and displaying command line help.
 */

#ifndef INCLUDED_MINIARGV_H
#define INCLUDED_MINIARGV_H

/*! \cond PRIVATE */
#if !defined(DLL_EXPORT_MINIARGV)
# if defined(_WIN32) && (defined(BUILD_MINIARGV_DLL) || defined(MINIARGV_EXPORTS))
#  define DLL_EXPORT_MINIARGV __declspec(dllexport)
# elif defined(_WIN32) && !defined(STATIC) && !defined(BUILD_MINIARGV_STATIC) && !defined(BUILD_MINIARGV)
#  define DLL_EXPORT_MINIARGV __declspec(dllimport)
# else
#  define DLL_EXPORT_MINIARGV
# endif
#endif
/*! \endcond */



#ifdef __cplusplus
extern "C" {
#endif

/*! \brief data type for command line argument definition
 * \sa     miniargv_definition_struct
 * \sa     miniargv_process_arg()
 * \sa     miniargv_arg_help()
 * \sa     miniargv_handler_fn
 */
typedef struct miniargv_definition_struct miniargv_definition;

/*! \brief callback function called by miniargv_process_arg() for each argument encountered
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 value if specified, otherwise NULL (always specified for standalone value arguments or if \a argdef->argparam is not NULL)
 * \param  callbackdata          user data as passed to \a miniargv_process_arg()
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_definition
 * \sa     miniargv_definition_struct
 */
typedef int (*miniargv_handler_fn)(const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief structure for command line argument definition
 *
 * This structure contains the specification for a specific command line argument.
 *
 * The type \a miniargv_definition refers to this structure.
 *
 * Use a \a miniargv_definition[] array to define all possible arguments.
 *
 * The last entry in the array must have \a callbackfn set to NULL.
 *
 * An entry with both \a shortarg and \a longarg set to NULL refers to standalone value arguments.
 * Standalone value arguments are arguments not starting with either "-" or "--" (except for "-" all by itself which is als considered a standalone value argument).
 *
 * \sa     miniargv_definition
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_arg_help()
 * \sa     miniargv_handler_fn
 */
struct miniargv_definition_struct {
  char shortarg;                    /**< short argument (used as argument with one hyphen, ignored for environment variables) */
  const char* longarg;              /**< long argument (used as argument with two hyphens) */
  const char* argparam;             /**< NULL if argument takes no value or name of value to be used in help generated by \a miniargv_arg_help() */
  miniargv_handler_fn callbackfn;   /**< callback function to be called when argument is encountered in \a miniargv_process_arg() */
  const void* userdata;             /**< user data specific for this argument, can be used in callback functions */
  const char* help;                 /**< description of what this command line argument is for, used by \a miniargv_arg_help() */
};

/*! \brief first process environment variables, then process command line argument flags and finally process command line arguments values, and call the appropriate callback function for each match
 * \param  argv          NULL-terminated array of arguments (first one is the application itself)
 * \param  env           NULL-terminated array of environment variables
 * \param  argdef        definitions of possible command line arguments
 * \param  envdef        definitions of possible environment variables
 * \param  badfn         callback function for bad arguments
 * \param  callbackdata  user data passed to callback functions
 * \return zero on success or index of argument that caused processing to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_process_ltr()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 * \sa     miniargv_get_next_arg_param()
 */
DLL_EXPORT_MINIARGV int miniargv_process (char* argv[], char* env[], const miniargv_definition argdef[], const miniargv_definition envdef[], miniargv_handler_fn badfn, void* callbackdata);

/*! \brief first process environment variables and then process command line arguments (flags and values, from left to right), and call the appropriate callback function for each match
 * \param  argv          NULL-terminated array of arguments (first one is the application itself)
 * \param  env           NULL-terminated array of environment variables
 * \param  argdef        definitions of possible command line arguments
 * \param  envdef        definitions of possible environment variables
 * \param  badfn         callback function for bad arguments
 * \param  callbackdata  user data passed to callback functions
 * \return zero on success or index of argument that caused processing to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 * \sa     miniargv_get_next_arg_param()
 */
DLL_EXPORT_MINIARGV int miniargv_process_ltr (char* argv[], char* env[], const miniargv_definition argdef[], const miniargv_definition envdef[], miniargv_handler_fn badfn, void* callbackdata);

/*! \brief process command line arguments and call the appropriate callback function for each one (except the first one which is the application name)
 * \param  argv          NULL-terminated array of arguments (first one is the application itself)
 * \param  argdef        definitions of possible command line arguments
 * \param  badfn         callback function for bad arguments
 * \param  callbackdata  user data passed to callback functions
 * \return zero on success or index of argument that caused processing to abort
 * \sa     miniargv_definition
 * \sa     miniargv_handler_fn
 * \sa     miniargv_process()
 * \sa     miniargv_process_ltr()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 * \sa     miniargv_get_next_arg_param()
 */
DLL_EXPORT_MINIARGV int miniargv_process_arg (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata);

/*! \brief process only flag command line arguments and call the appropriate callback function for each one (except the first one which is the application name)
 * \param  argv          NULL-terminated array of arguments (first one is the application itself)
 * \param  argdef        definitions of possible command line arguments
 * \param  badfn         callback function for bad arguments
 * \param  callbackdata  user data passed to callback functions
 * \return zero on success or index of argument that caused processing to abort
 * \sa     miniargv_definition
 * \sa     miniargv_handler_fn
 * \sa     miniargv_process()
 * \sa     miniargv_process_ltr()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_process_arg_flags (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata);

/*! \brief process only standalone value command line arguments and call the appropriate callback function for each one (except the first one which is the application name)
 * \param  argv          NULL-terminated array of arguments (first one is the application itself)
 * \param  argdef        definitions of possible command line arguments
 * \param  badfn         callback function for bad arguments
 * \param  callbackdata  user data passed to callback functions
 * \return zero on success or index of argument that caused processing to abort
 * \sa     miniargv_definition
 * \sa     miniargv_handler_fn
 * \sa     miniargv_process()
 * \sa     miniargv_process_ltr()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_env()
 * \sa     miniargv_get_next_arg_param()
 */
DLL_EXPORT_MINIARGV int miniargv_process_arg_params (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata);

/*! \brief process environment variables and call the appropriate callback function for each match
 * \param  env           NULL-terminated array of environment variables
 * \param  envdef        definitions of possible environment variables
 * \param  callbackdata  user data passed to callback functions
 * \return zero on success or index of argument that caused processing to abort
 * \sa     miniargv_definition
 * \sa     miniargv_handler_fn
 * \sa     miniargv_process()
 * \sa     miniargv_process_ltr()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 */
DLL_EXPORT_MINIARGV int miniargv_process_env (char* env[], const miniargv_definition envdef[], void* callbackdata);

/*! \brief get next value command line argument
 * \param  argindex      index of current argument or 0 for the first call
 * \param  argv          NULL-terminated array of arguments (first one is the application itself)
 * \param  argdef        definitions of possible command line arguments
 * \param  badfn         callback function for bad arguments
 * \return index of next value command line argument, zero when done or negative on error
 * \sa     miniargv_definition
 * \sa     miniargv_handler_fn
 * \sa     miniargv_process()
 * \sa     miniargv_process_ltr()
 * \sa     miniargv_process_arg()
 */
DLL_EXPORT_MINIARGV int miniargv_get_next_arg_param (int argindex, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn);

/*! \brief get application name and length
 *
 * Gets the name of the current application from the first argv entry (argv[0]) as passed to main().
 *
 * Though the result is a null-terminated string, only the number of characters pointed to by \a length should be used (e.g. to avoid the ".exe" extension on Windows).
 *
 * NULL is returned on error or if \a argv0 is NULL or points to an empty string.
 *
 * \param  argv0                 argv[0] as passed to main()
 * \param  length                pointer that will receive the length of the application name (can be NULL)
 * \return application name, or NULL on error
 */
DLL_EXPORT_MINIARGV const char* miniargv_getprogramname (const char* argv0, int* length);

/*! \brief lists possible command line arguments on one line
 * \param  argdef                definitions of possible command line arguments
 * \param  shortonly             set to non-zero to only show short arguments
 * \sa     miniargv_env_list()
 * \sa     miniargv_help()
 * \sa     miniargv_env_help()
 */
DLL_EXPORT_MINIARGV void miniargv_arg_list (const miniargv_definition argdef[], int shortonly);

/*! \brief lists possible environment variables  on one line
 * \param  argdef                definitions of possible command line arguments
 * \param  noparam               set to non-zero to only show the variable names without value parameter
 * \sa     miniargv_arg_list()
 * \sa     miniargv_help()
 * \sa     miniargv_env_help()
 */
DLL_EXPORT_MINIARGV void miniargv_env_list (const miniargv_definition envdef[], int noparam);

/*! \brief display help text explaining command line arguments
 * \param  argdef                array of command line argument definitions
 * \param  descindent            indent where description starts, defaults to 25 if set to 0
 * \param  wrapwidth             maximum line length, defaults to 79 if set to 0
 * \sa     miniargv_help()
 * \sa     miniargv_env_help()
 * \sa     miniargv_wrap_and_indent_text()
 * \sa     miniargv_definition
 * \sa     miniargv_definition_struct
 */
DLL_EXPORT_MINIARGV void miniargv_arg_help (const miniargv_definition argdef[], int descindent, int wrapwidth);

/*! \brief display help text explaining environment variables
 * \param  envdef                definitions of possible environment variables
 * \param  descindent            indent where description starts, defaults to 25 if set to 0
 * \param  wrapwidth             maximum line length, defaults to 79 if set to 0
 * \sa     miniargv_help()
 * \sa     miniargv_arg_help()
 * \sa     miniargv_arg_list()
 * \sa     miniargv_env_list()
 * \sa     miniargv_wrap_and_indent_text()
 * \sa     miniargv_definition
 * \sa     miniargv_definition_struct
 */
DLL_EXPORT_MINIARGV void miniargv_env_help (const miniargv_definition envdef[], int descindent, int wrapwidth);

/*! \brief display help text explaining command line arguments and environment variables
 * \param  argdef                array of command line argument definitions
 * \param  envdef                definitions of possible environment variables
 * \param  descindent            indent where description starts, defaults to 25 if set to 0
 * \param  wrapwidth             maximum line length, defaults to 79 if set to 0
 * \sa     miniargv_arg_help()
 * \sa     miniargv_env_help()
 * \sa     miniargv_arg_list()
 * \sa     miniargv_env_list()
 * \sa     miniargv_wrap_and_indent_text()
 * \sa     miniargv_definition
 * \sa     miniargv_definition_struct
 */
DLL_EXPORT_MINIARGV void miniargv_help (const miniargv_definition argdef[], const miniargv_definition envdef[], int descindent, int wrapwidth);

/*! \brief display help text wile wrapping it at a maximum width and indenting new lines
 * \param  text                  text to display
 * \param  currentpos            current position when this function is called (leftmost position is 0)
 * \param  indentpos             position to indent new lines at (leftmost position is 0)
 * \param  wrapwidth             maximum line length, defaults to 79 if set to 0
 * \sa     miniargv_help()
 * \sa     miniargv_arg_help()
 * \sa     miniargv_env_help()
 * \sa     miniargv_arg_list()
 * \sa     miniargv_env_list()
 * \sa     miniargv_definition
 * \sa     miniargv_definition_struct
 */
DLL_EXPORT_MINIARGV void miniargv_wrap_and_indent_text (const char* text, int currentpos, int indentpos, int wrapwidth);



/*! \brief predefined callback function to set constant string \b userdata to \b value
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 value if specified, otherwise NULL (always specified for standalone value arguments or if \a argdef->argparam is not NULL)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_const_str (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set string \b userdata to a copy of \b value
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 value if specified, otherwise NULL (always specified for standalone value arguments or if \a argdef->argparam is not NULL)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_strdup (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the integer pointed to by \b userdata to the numeric value of \b value
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 value, must be specified and must be a number
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_long()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_int (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the integer pointed to by \b userdata to 0
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_int()
 * \sa     miniargv_cb_set_int_to_one()
 * \sa     miniargv_cb_set_int_to_minus_one()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_zero (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the integer pointed to by \b userdata to 1
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_int()
 * \sa     miniargv_cb_set_int_to_zero()
 * \sa     miniargv_cb_set_int_to_minus_one()
 * \sa     miniargv_process()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_one (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the integer pointed to by \b userdata to -1
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_int()
 * \sa     miniargv_cb_set_int_to_zero()
 * \sa     miniargv_cb_set_int_to_one()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_minus_one (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to increment the integer pointed to by \b userdata
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_decrement_int()
 * \sa     miniargv_cb_increment_long()
 * \sa     miniargv_cb_decrement_long()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_increment_int (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to decrement the integer pointed to by \b userdata
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_increment_int()
 * \sa     miniargv_cb_increment_long()
 * \sa     miniargv_cb_decrement_long()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_decrement_int (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the long integer pointed to by \b userdata to the numeric value of \b value
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 value, must be specified and must be a number
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_int()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_long (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the long integer pointed to by \b userdata to 0
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_long()
 * \sa     miniargv_cb_set_long_to_one()
 * \sa     miniargv_cb_set_long_to_minus_one()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_zero (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the long integer pointed to by \b userdata to 1
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_long()
 * \sa     miniargv_cb_set_long_to_zero()
 * \sa     miniargv_cb_set_long_to_minus_one()
 * \sa     miniargv_process()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_one (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to set the long integer pointed to by \b userdata to -1
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_set_long()
 * \sa     miniargv_cb_set_long_to_zero()
 * \sa     miniargv_cb_set_long_to_one()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_minus_one (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to increment the long integer pointed to by \b userdata
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_increment_int()
 * \sa     miniargv_cb_decrement_int()
 * \sa     miniargv_cb_decrement_long()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_increment_long (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function to decrement the long integer pointed to by \b userdata
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_cb_increment_int()
 * \sa     miniargv_cb_decrement_int()
 * \sa     miniargv_cb_increment_long()
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_decrement_long (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function that does nothing
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return 0
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_noop (const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief predefined callback function that returns an error and displays the error pointed to by \b userdata (if not NULL)
 * \param  argdef                definition of command line argument, or NULL for standalone value argument
 * \param  value                 (unused)
 * \param  callbackdata          (unused)
 * \return -1
 * \sa     miniargv_handler_fn
 * \sa     miniargv_definition
 * \sa     miniargv_process()
 * \sa     miniargv_process_arg()
 * \sa     miniargv_process_arg_flags()
 * \sa     miniargv_process_arg_params()
 * \sa     miniargv_process_env()
 */
DLL_EXPORT_MINIARGV int miniargv_cb_error (const miniargv_definition* argdef, const char* value, void* callbackdata);



/*! \brief get miniargv library version string
 * \param  pmajor        pointer to integer that will receive major version number
 * \param  pminor        pointer to integer that will receive minor version number
 * \param  pmicro        pointer to integer that will receive micro version number
 * \sa     miniargv_get_version_string()
 * \sa     MINIARGV_VERSION
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_STRING
 */
DLL_EXPORT_MINIARGV void miniargv_get_version (int* pmajor, int* pminor, int* pmicro);

/*! \brief get miniargv library version string
 * \return version string
 * \sa     miniargv_get_version()
 * \sa     MINIARGV_VERSION_STRING
 * \sa     MINIARGV_VERSION
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 */
DLL_EXPORT_MINIARGV const char* miniargv_get_version_string ();

#ifdef __cplusplus
}
#endif



/*! \addtogroup MINIARGV_VERSION_
 *  \brief version number constants
 * \sa     MINIARGV_VERSION
 * \sa     miniargv_get_version()
 * \sa     miniargv_get_version_string()
 * @{
 */
/*! \brief major version number \hideinitializer */
#define MINIARGV_VERSION_MAJOR 0
/*! \brief minor version number \hideinitializer */
#define MINIARGV_VERSION_MINOR 2
/*! \brief micro version number \hideinitializer */
#define MINIARGV_VERSION_MICRO 9
/** @} */

/*! \brief packed version number (bits 24-31: major version, bits 16-23: minor version, bits 8-15: micro version)
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_
 * \hideinitializer
 */
#define MINIARGV_VERSION (MINIARGV_VERSION_MAJOR * 0x01000000 + MINIARGV_VERSION_MINOR * 0x00010000 + MINIARGV_VERSION_MICRO * 0x00000100)

/*! \cond PRIVATE */
#define MINIARGV_VERSION_STRINGIZE_(major, minor, micro) #major"."#minor"."#micro
#define MINIARGV_VERSION_STRINGIZE(major, minor, micro) MINIARGV_VERSION_STRINGIZE_(major, minor, micro)
/*! \endcond */

/*! \brief string with dotted version number
 * \sa     MINIARGV_FULLNAME
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_
 * \hideinitializer
 */
#define MINIARGV_VERSION_STRING MINIARGV_VERSION_STRINGIZE(MINIARGV_VERSION_MAJOR, MINIARGV_VERSION_MINOR, MINIARGV_VERSION_MICRO)

/*! \brief string with name of miniargv library */
#define MINIARGV_NAME "miniargv"

/*! \brief string with name and version of miniargv library
 * \sa     MINIARGV_VERSION_STRING
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_
 * \hideinitializer
 */
#define MINIARGV_FULLNAME MINIARGV_NAME " " MINIARGV_VERSION_STRING

#endif
