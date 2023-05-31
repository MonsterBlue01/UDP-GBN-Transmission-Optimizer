// #define _GNU_SOURCE

// #include <err.h>
// #include <errno.h>
// #include <linux/limits.h>
// #include <sched.h>
// #include <signal.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/mount.h>
// #include <sys/stat.h>
// #include <wait.h>
// #include <sys/types.h>
// #include <fcntl.h>

// #include "change_root.h"

// #define CONTAINER_ID_MAX 16
// #define CHILD_STACK_SIZE 4096 * 10

// #define IMAGE_DIR_MAX 256
// #define CMD_MAX 256
// #define CWD_MAX 256

// typedef struct container {
//     char id[CONTAINER_ID_MAX];
//     // TODO: Add fields
//     char image_dir[IMAGE_DIR_MAX];
//     char cmd[CMD_MAX];
//     char cwd[CWD_MAX];
// } container_t;

// /**
//  * `usage` prints the usage of `client` and exists the program with
//  * `EXIT_FAILURE`
//  */
// void usage(char* cmd) {
//     printf("Usage: %s [ID] [IMAGE] [CMD]...\n", cmd);
//     exit(EXIT_FAILURE);
// }

// /**
//  * `container_exec` is an entry point of a child process and responsible for
//  * creating an overlay filesystem, calling `change_root` and executing the
//  * command given as arguments.
//  */
// int container_exec(void* arg) {
//     container_t* container = (container_t*)arg;
//     // this line is required on some systems
//     if (mount("/", "/", "none", MS_PRIVATE | MS_REC, NULL) < 0) {
//         err(1, "mount / private");
//     }

//     // TODO: Create a overlay filesystem
//     // `lowerdir`    should be the image directory: ${cwd}/images/${image}
//     // `upperdir`    should be `/tmp/container/{id}/upper`
//     // `workdir`     should be `/tmp/container/{id}/work`
//     // `merged`        should be `/tmp/container/{id}/merged`
//     // ensure all directories exist (create if not exists) and
//     // call `mount("overlay", merged, "overlay", MS_RELATIME,
//     //        lowerdir={lowerdir},upperdir={upperdir},workdir={workdir})`

//     // TODO: Call `change_root` with the `merged` directory
//     // change_root(merged)

//     // TODO: use `execvp` to run the given command and return its return value
//     // Create the necessary directories for the overlay filesystem
//    char lowerdir[PATH_MAX], upperdir[PATH_MAX], workdir[PATH_MAX], merged[PATH_MAX];
//    sprintf(lowerdir, "%s/images/%s", container->cwd, container->image_dir);
//    sprintf(upperdir, "/tmp/container/%s/upper", container->id);
//    sprintf(workdir, "/tmp/container/%s/work", container->id);
//    sprintf(merged, "/tmp/container/%s/merged", container->id);

//    // Ensure directories exist
//    mkdir(upperdir, 0700);
//    mkdir(workdir, 0700);
//    mkdir(merged, 0700);
   
//    // Create the overlay filesystem
//    char options[PATH_MAX];
//    sprintf(options, "lowerdir=%s,upperdir=%s,workdir=%s", lowerdir, upperdir, workdir);
//    if (mount("overlay", merged, "overlay", MS_RELATIME, options) < 0) {
//      err(1, "mount overlay");
//    }

//    // Change root to the merged directory
//    change_root(merged);

//    // Use execvp to run the given command
//    char* cmd[] = { container->cmd, NULL };
//    execvp(cmd[0], cmd);

//    // Should not reach here
//    return 1;
// }

// int main(int argc, char** argv) {
//     if (argc < 4) {
//         usage(argv[0]);
//     }

//     /* Create tmpfs and mount it to `/tmp/container` so overlayfs can be used
//      * inside Docker containers */
//     if (mkdir("/tmp/container", 0700) < 0 && errno != EEXIST) {
//         err(1, "Failed to create a directory to store container file systems");
//     }
//     if (errno != EEXIST) {
//         if (mount("tmpfs", "/tmp/container", "tmpfs", 0, "") < 0) {
//             err(1, "Failed to mount tmpfs on /tmp/container");
//         }
//     }

//     /* cwd contains the absolute path to the current working directory which can
//      * be useful constructing path for image */
//     char cwd[PATH_MAX];
//     getcwd(cwd, PATH_MAX);

//     container_t container;
//     strncpy(container.id, argv[1], CONTAINER_ID_MAX);

//     // TODO: store all necessary information to `container`
//     strncpy(container.id, argv[1], CONTAINER_ID_MAX);
//     strncpy(container.image_dir, argv[2], IMAGE_DIR_MAX);
//     strncpy(container.cmd, argv[3], CMD_MAX);
//     strncpy(container.cwd, cwd, CWD_MAX);

//     /* Use `clone` to create a child process */
//     char child_stack[CHILD_STACK_SIZE];    // statically allocate stack for child
//     int clone_flags = SIGCHLD | CLONE_NEWNS | CLONE_NEWPID;
//     int pid = clone(container_exec, &child_stack, clone_flags, &container);
//     if (pid < 0) {
//         err(1, "Failed to clone");
//     }

//     waitpid(pid, NULL, 0);
//     return EXIT_SUCCESS;
// }

#define _GNU_SOURCE

#include <err.h>
#include <errno.h>
#include <linux/limits.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <wait.h>
#include <unistd.h>

#include "change_root.h"

#define CONTAINER_ID_MAX 16
#define CHILD_STACK_SIZE 4096 * 10

typedef struct container {
  // ID of the container
  char id[CONTAINER_ID_MAX];
  // Path of the container image $(PWD)/images/<image>
  char image[PATH_MAX];
  // Container file system root: /tmp/container/<id>
  char path[PATH_MAX];
  // Commands to execute inside the container
  char **args;
} container_t;

/**
 * `usage` prints the usage of `client` and exists the program with
 * `EXIT_FAILURE`
 */
void usage(char *cmd) {
  printf("Usage: %s [ID] [IMAGE] [CMD]...\n", cmd);
  exit(EXIT_FAILURE);
}

/**
 * `container_exec` is an entry point of a child process and responsible for
 * creating an overlay filesystem, calling `change_root` and executing the
 * command given as arguments.
 */
int container_exec(void *arg) {
  container_t *container = (container_t *)arg;
  // this line is required on some systems
  if (mount("/", "/", "none", MS_PRIVATE | MS_REC, NULL) < 0) {
    err(1, "mount / private");
  }

  // TODO: Create a overlay filesystem
  // `lowerdir`  should be the image directory: ${cwd}/images/${image}
  // `upperdir`  should be `/tmp/container/{id}/upper`
  // `workdir`   should be `/tmp/container/{id}/work`
  // `merged`    should be `/tmp/container/{id}/merged`
  // ensure all directories exist (create if not exists) and
  // call `mount("overlay", merged, "overlay", MS_RELATIME,
  //    lowerdir={lowerdir},upperdir={upperdir},workdir={workdir})`

  // TODO: Call `change_root` with the `merged` directory
  // change_root(merged)
  char *upper;
  char *work;
  char *merged;
  asprintf(&upper, "%s/upper", container->path);
  asprintf(&work, "%s/work", container->path);
  asprintf(&merged, "%s/merged", container->path);
  if (mkdir(upper, 0700) < 0 && errno != EEXIST) {
    err(1, "Failed to create upper directory");
  }
  if (mkdir(work, 0700) < 0 && errno != EEXIST) {
    err(1, "Failed to create work directory");
  }
  if (mkdir(merged, 0700) < 0 && errno != EEXIST) {
    printf("%s\n", merged);
    err(1, "Failed to create merged directory");
  }

  char *ovfs_opts;
  asprintf(&ovfs_opts, "lowerdir=%s,upperdir=%s,workdir=%s", container->image,
           upper, work);
  if (mount("overlay", merged, "overlay", MS_RELATIME, ovfs_opts) < 0)
    err(1, "mount overlay");

  change_root(merged);

  free(upper);
  free(work);
  free(merged);
  free(ovfs_opts);

  return execvp(container->args[0], container->args);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    usage(argv[0]);
  }

  /* Create tmpfs and mount it to `/tmp/container` so overlayfs can be used
   * inside Docker containers */
  if (mkdir("/tmp/container", 0700) < 0 && errno != EEXIST) {
    err(1, "Failed to create a directory to store container file systems");
  }
  if (errno != EEXIST) {
    if (mount("tmpfs", "/tmp/container", "tmpfs", 0, "") < 0) {
      err(1, "Failed to mount tmpfs on /tmp/container");
    }
  }

  /* cwd contains the absolute path to the current working directory which can
   * be useful constructing path for image */
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  container_t container;
  strncpy(container.id, argv[1], CONTAINER_ID_MAX);

  // TODO: store all necessary information to `container`
  sprintf(container.image, "%s/images/%s", cwd, argv[2]);
  sprintf(container.path, "/tmp/container/%s", container.id);
  if (mkdir(container.path, 0700) < 0 && errno != EEXIST) {
    err(1,
        "Failed to create a directory to store file system for this container");
  }
  container.args = &argv[3];

  /* Use `clone` to create a child process */
  char child_stack[CHILD_STACK_SIZE];  // statically allocate stack for child
  int clone_flags = SIGCHLD | CLONE_NEWNS | CLONE_NEWPID;
  int pid = clone(container_exec, &child_stack, clone_flags, &container);
  if (pid < 0) {
    err(1, "Failed to clone");
  }

  waitpid(pid, NULL, 0);
  return EXIT_SUCCESS;
}

