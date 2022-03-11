#include <git2/remote.h>
#include <string.h>
#include <unistd.h>
#include <git2.h>
#include <git2/global.h>
#include <git2/types.h>
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
    char *url = malloc(get_alloc_size(package_name, 30));
    strcpy(url, "https://aur.archlinux.org/");
    strcat(url, package_name);
    strcat(url, ".git");

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
        // TODO: Handle already cloned repositories
    }
    
    else if (status_code != 0)
        print_error_and_exit(CLONE_ERROR, status_code);
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

int main(int argc, char **argv)
{
    if (argc < 2)
        print_usage(argv[0]);

    int opt;
    char *package = "";
    enum {INSTALL, REMOVE} mode = -1;

    while((opt = getopt(argc, argv, "S:R:")) != -1)
    {
        switch (opt)
        {
            case 'S':
                mode = INSTALL;
                package = argv[optind - 1];
                break;
            case 'R':
                mode = REMOVE;
                package = argv[optind - 1];
                break;
            default:
                print_usage(argv[0]);
        }
    }

    if (mode == -1 || strcmp(package, "") == 0)
        print_usage(argv[0]);


    initialize_devaur();

    switch (mode)
    {
        case INSTALL:
            install_package(package);
            break;
        case REMOVE:
            remove_package(package);
            break;
    }

    return 0;
}
