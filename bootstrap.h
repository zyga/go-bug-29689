#ifndef BOOTSTRAP_H 
#define BOOTSTRAP_H 

extern int bootstrap_errno;
extern const char *bootstrap_msg;

void bootstrap(int argc, char **argv, char **envp);

#endif
