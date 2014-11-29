#ifndef MAIN_H
#define MAIN_H
#define MAX_DIR_LENGTH 2048
#define MAX_CHILDREN 2048
extern char shell_home[MAX_DIR_LENGTH];
extern char shell_workdir[MAX_DIR_LENGTH];
extern pid_t children_fg[MAX_CHILDREN];
extern pid_t children_bg[MAX_CHILDREN];
extern volatile int num_children_fg;
extern volatile int num_children_bg;
extern pid_t children_bg_terminated[MAX_CHILDREN];
extern int children_bg_terminated_status[MAX_CHILDREN];
extern int num_children_bg_terminated;

#endif
