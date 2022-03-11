#include <string.h>
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

int clone_repository(const char *package_name)
{
    print_message(CLONING_IN_PROGRESS);

    git_repository *repository = NULL;

    char *url = get_url_from_package_name(package_name);
    char *temp_dir = get_temp_dir(package_name);

    int error = git_clone(&repository, url, temp_dir, NULL);
    
    return error;
}

int install_package(const char *package_name)
{
    print_message(INSTALLATION_IN_PROGRESS);

    char *temp_dir = get_temp_dir(package_name);
    int cache_dir_length = strlen(temp_dir);

    char *command = malloc(get_alloc_size(package_name, 20 + cache_dir_length));
    strcpy(command, "cd ");
    strcat(command, temp_dir);
    strcat(command, " && makepkg -si");

    int system_call_error = system(command);

    return system_call_error;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 2;

    initialize_devaur();

    char *package_name = argv[1];

    if (clone_repository(package_name) != 0)
        print_error_and_exit(CLONE_ERROR);

    if (install_package(package_name) != 0)
        print_error_and_exit(INSTALL_ERROR);

    print_message(SUCCESS);

    return 0;
}
