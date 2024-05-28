#include <mt.h>

signed
main (signed argc, char * argv[], char * envp[]) {

    if ( argc > 1 ) {
        char * cmd = calloc(1, strlen(argv[0]) + sizeof "-" + strlen(argv[1]));
        sprintf(cmd, "%s-%s", argv[0], argv[1]);

        if ( access(cmd, X_OK) != 0 ) {
            fprintf(stderr, "Unknown Command: %s\n", cmd);
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();

        signed status = EXIT_SUCCESS;

        if ( pid == 0 ) { // child
            execve(cmd, &argv[1], envp);
            abort();
        } else { // parent
            waitpid(pid, &status, 0);
        }

        return status;
    }

    char bin_path [BUFSIZ] = {0};
    if ( readlink("/proc/self/exe", bin_path, BUFSIZ) == -1 ) {
        return EXIT_FAILURE;
    }

    puts("Available commands:");

    struct dirent * f = NULL;
    dirname(bin_path);
    DIR * dir = opendir(bin_path);
    if ( !dir ) {
        return EXIT_FAILURE;
    }

    while ( (f = readdir(dir)) ) {
        if ( !strncmp("mt-", f->d_name, sizeof "mt-" - 1) ) {
            printf("\t%s\n", f->d_name + sizeof "mt-" - 1);
        }
    }

    return EXIT_SUCCESS;
}
