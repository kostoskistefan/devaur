#include <git2.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <git2/types.h>
#include <git2/remote.h>
#include <git2/global.h>

#include "modes.h"
#include "messages.h"
#include "print_utilities.h"

#define TEMP_DIR "/.cache/devaur/"

void initialize_devaur()
{
    git_libgit2_init();
}

int get_alloc_size(const char *package_name, int offset)
{
    int package_name_length = strlen(package_name);
    return package_name_length + offset;
}

char * get_url_from_package_name(const char *package_name)
{
    char *url = malloc(get_alloc_size(package_name, 26));
    strcpy(url, "https://aur.archlinux.org/");
    strcat(url, package_name);

    return url;
}

char * get_temp_dir(const char *package_name)
{
    char *home_dir = getenv("HOME");

    int home_dir_length = strlen(home_dir);
    int temp_dir_length = strlen(TEMP_DIR);
    int alloc_offset = home_dir_length + temp_dir_length;

    char *temp_dir = malloc(get_alloc_size(package_name, alloc_offset));
    strcpy(temp_dir, home_dir);
    strcat(temp_dir, TEMP_DIR);
    strcat(temp_dir, package_name);

    return temp_dir;
}

void get_repository(const char *package_name)
{
    print_message(CLONING_IN_PROGRESS);

    git_repository *repository = NULL;

    char *url = get_url_from_package_name(package_name);
    char *temp_dir = get_temp_dir(package_name);

    int status_code = git_clone(&repository, url, temp_dir, NULL);

    if (status_code == GIT_EEXISTS)
    {
        git_remote *remote = NULL;
        git_repository_open(&repository, temp_dir);

        if((status_code = git_remote_lookup(&remote, repository, "origin")) != 0)
            print_error_and_exit(REMOTE_ERROR, status_code);

        if((status_code = git_remote_fetch(remote, NULL, NULL, "pull")) != 0)
            print_error_and_exit(FETCH_ERROR, status_code);
    }

    else if (status_code != 0)
        print_error_and_exit(CLONE_ERROR, status_code);

    git_repository_free(repository);
}

void make_package(const char *package_name)
{
    print_message(INSTALLATION_IN_PROGRESS);

    char *temp_dir = get_temp_dir(package_name);
    int cache_dir_length = strlen(temp_dir);

    char *command = malloc(get_alloc_size(package_name, 20 + cache_dir_length));
    strcpy(command, "cd ");
    strcat(command, temp_dir);
    strcat(command, " && makepkg -si");

    int status_code = system(command);

    if (status_code != 0)
        print_error_and_exit(INSTALL_ERROR, status_code);
}

void install_package(const char *package_name)
{
    get_repository(package_name);
    make_package(package_name);
}

void remove_package(const char *package_name)
{
    char *command = malloc(get_alloc_size(package_name, 18));
    strcpy(command, "sudo pacman -Rcns ");
    strcat(command, package_name);

    int status_code;

    if ((status_code = system(command)) != 0)
        print_error_and_exit(REMOVE_ERROR, status_code);
}

void check_empty_package_name(char *package)
{
    if (strcmp(package, "") == 0)
        print_usage();
}

void parse_arguments(int argc, char **argv, char **package, modes_t *mode)
{
    if (argc < 2)
        print_usage();

    int opt;

    while((opt = getopt(argc, argv, "S:R:C")) != -1)
    {
        switch (opt)
        {
            case 'S':
                *mode = INSTALL;
                *package = argv[optind - 1];
                check_empty_package_name(*package);
                break;
            case 'R':
                *mode = REMOVE;
                *package = argv[optind - 1];
                check_empty_package_name(*package);
                break;
            case 'C':
                *mode = CLEAN_CACHE;
                break;
            default:
                print_usage(argv[0]);
        }
    }
}

void clean_cache()
{
    char answer;
    printf(CLEAN_CACHE_PROMPT);
    scanf("%c", &answer);

    if (answer == 'y')
    {
        char *home_dir = getenv("HOME");
        int home_dir_length = strlen(home_dir);
        int temp_dir_length = strlen(TEMP_DIR);

        char *command = malloc(home_dir_length + temp_dir_length + 7);
        strcpy(command, "rm -rf ");
        strcat(command, home_dir);
        strcat(command, TEMP_DIR);

        system(command);
    }

    else print_message(EXIT_MESSAGE);
}

int main(int argc, char **argv)
{
    char *package = "";
    modes_t mode = UNDEFINED;

    parse_arguments(argc, argv, &package, &mode);

    initialize_devaur();

    switch (mode)
    {
        case UNDEFINED:
            print_usage();
            break;
        case INSTALL:
            install_package(package);
            break;
        case REMOVE:
            remove_package(package);
            break;
        case CLEAN_CACHE:
            clean_cache();
            break;
    }

    git_libgit2_shutdown();

    return 0;
}
