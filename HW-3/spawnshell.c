/* $begin shellmain */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <spawn.h>

#define MAXARGS 128
#define MAXLINE 8192 /* Max text line length */

typedef enum { IS_SIMPLE, IS_PIPE, IS_INPUT_REDIR, IS_OUTPUT_REDIR, IS_INPUT_OUTPUT_REDIR, IS_SEQ, IS_ANDIF} Mode; /* simple command, |, >, <, ;, && */
typedef struct { 
    char *argv[MAXARGS]; /* Argument list */
    int argc; /* Number of args */
    int bg; /* Background job? */
    Mode mode; /* Handle special cases | > < ; */
} parsed_args; 

extern char **environ; /* Defined by libc */

/* Function prototypes */
void eval(char *cmdline);
parsed_args parseline(char *buf);
int builtin_command(char **argv, pid_t pid, int status);
void signal_handler(int sig);
int exec_cmd(char** argv, posix_spawn_file_actions_t *actions, pid_t *pid, int *status, int bg);
int find_index(char** argv, char* target); 

void unix_error(char *msg) /* Unix-style error */
{
  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  exit(EXIT_FAILURE);
}

int main() {
  char cmdline[MAXLINE]; /* Command line */
  /* TODO: register signal handlers */
  signal(SIGINT,  signal_handler);
  signal(SIGTSTP, signal_handler);
  signal(SIGCHLD, signal_handler);

  while (1) {
    char *result;
    /* Read */
    printf("CS361 >"); /* TODO: correct the prompt */
    result = fgets(cmdline, MAXLINE, stdin);
    if (result == NULL && ferror(stdin)) {
      fprintf(stderr, "fatal fgets error\n");
      exit(EXIT_FAILURE);
    }

    if (feof(stdin)) exit(EXIT_SUCCESS);

    /* Evaluate */
    eval(cmdline);
  }
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) {
  char buf[MAXLINE];   /* Holds modified command line */
  pid_t pid;           /* Process id */
  pid_t pid2;
  int status;          /* Process status */
  int locate;
  int num = 1;
  int null = 0;
  posix_spawn_file_actions_t actions; /* used in performing spawn operations */
  posix_spawn_file_actions_t act2;
  posix_spawn_file_actions_init(&actions);
  posix_spawn_file_actions_init(&act2);
  int pipe_fds[2];

  strcpy(buf, cmdline);
  parsed_args parsed_line = parseline(buf);
  if (parsed_line.argv[0] == NULL) return; /* Ignore empty lines */

  /* Not a bultin command */
  if (!builtin_command(parsed_line.argv, pid, status)) {
    switch (parsed_line.mode) {
      case IS_SIMPLE: /* cmd argv1 argv2 ... */
        if (!exec_cmd(parsed_line.argv, &actions, &pid, &status, parsed_line.bg)) return;
        break;
      case IS_PIPE: /* command1 args | command2 args */
        // TODO: handle pipe 
        locate = find_index(parsed_line.argv, "|");
        parsed_line.argv[locate] = NULL;
        //posix_spawn_file_actions_init(&actions);
        //posix_spawn_file_actions_init(&act2);
        pipe(pipe_fds);
        posix_spawn_file_actions_adddup2(&actions, pipe_fds[1], STDOUT_FILENO);
        posix_spawn_file_actions_addclose(&actions, pipe_fds[0]);
        posix_spawn_file_actions_adddup2(&act2, pipe_fds[0], STDIN_FILENO);
        posix_spawn_file_actions_addclose(&act2, pipe_fds[1]);
        if (0 != posix_spawnp(&pid, parsed_line.argv[null], &actions, NULL, parsed_line.argv, environ)) {
          perror("spawn failed");
          exit(1);
        }
        if (0 != posix_spawnp(&pid2, parsed_line.argv[locate+num], &act2, NULL, parsed_line.argv+locate+num, environ)) {
          perror("spawn failed");
          exit(1);
        }
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        waitpid(pid, &status, 0);
        waitpid(pid2, &status, 0);
        //printf("pipe not yet implemented :(\n");
        break;
      case IS_OUTPUT_REDIR: /* command args > output_redirection */
        // TODO: handle output redirection 
        printf("output redirection not yet implemented :(\n");
        break;
      case IS_INPUT_REDIR: /* command args < input_redirection */
        // TODO: handle input redirection 
        printf("input redirection not yet implemented :(\n");
        break;
      case IS_INPUT_OUTPUT_REDIR: /* command args < input_redirection > output_redirection */
        // TODO: handle input output redirection 
        printf("input output redirection not yet implemented :(\n");
        break;
      case IS_SEQ: /* command1 args ; command2 args */
        // TODO: handle sequential 
        locate = find_index(parsed_line.argv, ";");
        parsed_line.argv[locate] = NULL;
        exec_cmd(parsed_line.argv, &actions, &pid, &status, parsed_line.bg);
        exec_cmd(parsed_line.argv+locate+num, &act2, &pid2, &status, parsed_line.bg);
        //posix_spawnp(&pid, parsed_line.argv[locate], &actions, NULL, parsed_line.argv, environ);
        //posix_spawnp(&pid2, parsed_line.argv[locate+1], &act2, NULL, parsed_line.argv, environ);
        printf("sequential commands not yet implemented :(\n");
        break;
      case IS_ANDIF: /* command1 args && command2 args */
        // TODO: handle "and if"
        printf("composed && commands not yet implemented :(\n");
        break;
    }
    if (parsed_line.bg)
      printf("%d %s", pid, cmdline);
    
  }
  posix_spawn_file_actions_destroy(&actions);
  return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv, pid_t pid, int status) {
  if (!strcmp(argv[0], "exit")) /* exit command */
    exit(EXIT_SUCCESS);
  if (!strcmp(argv[0], "&")) /* Ignore singleton & */
    return 1;
  // TODO: implement special command "?"

  return 0; /* Not a builtin command */
}
/* $end eval */

/* Run commands using posix_spawnp */
int exec_cmd(char** argv, posix_spawn_file_actions_t *actions, pid_t *pid, int *status, int bg) {
  printf("simple command not yet implemented :(\n");
  // Lab 5 TODO: use posix_spawnp to execute commands 

  posix_spawnp(pid, argv[0], actions, NULL, argv, environ);

  // Lab 5 TODO: when posix_spawnp is ready, uncomment me
  if (!bg)
    if (waitpid(*pid, status, 0) < 0) unix_error("waitfg: waitpid error");
  return 1;
}
/* $end exec_cmd */

/* signal handler */
void signal_handler(int sig) {
  // TODO: handle SIGINT and SIGTSTP and SIGCHLD signals here
  if (sig == SIGINT)
  {
    printf("Nike");
  }
  else if (sig == SIGTSTP)
  {
    printf("Love");
  }
  else if (sig == SIGCHLD)
  {
    wait(NULL);
  }
  // char nike[] = "\nNike's_Love_CS361\n";
  // write(1, nike, sizeof(nike));
}

/* finds index of the matching target in the argumets */
int find_index(char** argv, char* target) {
  for (int i=0; argv[i] != NULL; i++)
    if (!strcmp(argv[i], target))
      return i;
  return 0;
}

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
parsed_args parseline(char *buf) {
  char *delim; /* Points to first space delimiter */
  parsed_args pa;

  buf[strlen(buf) - 1] = ' ';   /* Replace trailing '\n' with space */
  while (*buf && (*buf == ' ')) /* Ignore leading spaces */
    buf++;

  /* Build the argv list */
  pa.argc = 0;
  while ((delim = strchr(buf, ' '))) {
    pa.argv[pa.argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')){ /* Ignore spaces */
      buf++;
    }
  }
  pa.argv[pa.argc] = NULL;

  if (pa.argc == 0){ /* Ignore blank line */
    return pa;
  }

  /* Should the job run in the background? */
  if ((pa.bg = (*pa.argv[pa.argc - 1] == '&')) != 0) pa.argv[--pa.argc] = NULL;

  /* Detect various command modes */
  pa.mode = IS_SIMPLE;
  if (find_index(pa.argv, "|"))
    pa.mode = IS_PIPE;
  else if(find_index(pa.argv, ";")) 
    pa.mode = IS_SEQ; 
  else if(find_index(pa.argv, "&&"))
    pa.mode = IS_ANDIF;
  else {
    if(find_index(pa.argv, "<")) 
      pa.mode = IS_INPUT_REDIR;
    if(find_index(pa.argv, ">")){
      if (pa.mode == IS_INPUT_REDIR)
        pa.mode = IS_INPUT_OUTPUT_REDIR;
      else
        pa.mode = IS_OUTPUT_REDIR; 
    }
  }

  return pa;
}
/* $end parseline */
