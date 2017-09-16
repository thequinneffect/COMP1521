// myls.c ... my very own "ls" implementation

#include <stdlib.h>
#include <stdio.h>
#include <bsd/string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#define MAXDIRNAME 100
#define MAXFNAME   200
#define MAXNAME    20
#define IDENTICAL  0

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[01;32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[01;34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[00;0m"

struct meta_data {
   blkcnt_t blocks;
   char perms[MAXNAME];
   nlink_t links;
   char user[MAXNAME];
   char group[MAXNAME];
   off_t size;
   char time[MAXFNAME];
   char name[MAXNAME];

   struct meta_data *next;
};

char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);
struct meta_data *sorted_append(struct meta_data *head, blkcnt_t blocks, char *perms, nlink_t links, char *user, char *group, off_t size, char *time, char *name);
void print_list(struct meta_data *head, int block_padding, int user_padding, int group_padding, int size_padding);
char *clean_time(char *time);

int main(int argc, char *argv[])
{
   // string buffers for various names
   char dirname[MAXDIRNAME];
   char uname[MAXNAME+1];
   char gname[MAXNAME+1];
   char mode[MAXNAME+1];
   char path[MAXDIRNAME];
   char time[MAXFNAME];

   // collect the directory name, with "." as default
   if (argc < 2)
      strlcpy(dirname, ".", MAXDIRNAME);
   else
      strlcpy(dirname, argv[1], MAXDIRNAME);

   // check that the name really is a directory
   struct stat info;
   //int stat(const char *pathname, struct stat *buf);
   if (stat(dirname, &info) < 0)
      { perror(argv[0]); exit(EXIT_FAILURE); }
   if ((info.st_mode & S_IFMT) != S_IFDIR)
      { fprintf(stderr, "%s: Not a directory\n",argv[0]); exit(EXIT_FAILURE); }

   DIR *dir_p = opendir(dirname);

   if (dir_p == NULL)
   {
      printf("error, directory open failed\n");
      return 1;
   }
  
   struct dirent *entry;
   struct stat fileInfo;
   struct meta_data *head = NULL;
   blkcnt_t total = 0;
   int block_padding = 0, size_of_block_padding = 0;
   int user_padding = 0, size_of_user_padding = 0;
   int group_padding = 0, size_of_group_padding = 0;
   int size_padding = 0, size_of_size_padding = 0;

   while ((entry = readdir(dir_p)) != NULL) //dirp knows where i am up to
   {
      if (entry->d_name[0] != '.')
      {
         // 1. build path to current file
         memset(path,0,sizeof(path));
         strcat(path, dirname);      
         strcat(path, "/");
         strcat(path, entry->d_name);
         strcat(path, "\0");

         // 2. build stat struct with meta data from current file
         lstat(path, &fileInfo);

         char *c_time = ctime(&fileInfo.st_mtime);
         char *nullchar = "\0";
         strncpy(time, c_time+4, 12);
         strcpy(&time[12], nullchar);

         total += (long long)fileInfo.st_blocks/2;

         // nlink_t   st_nlink;

         // 3. copy to separate struct
         head = sorted_append(head, (long long)fileInfo.st_blocks, rwxmode(fileInfo.st_mode, mode), (long long)fileInfo.st_nlink,
                        username(fileInfo.st_uid, uname),
                        groupname(fileInfo.st_gid, gname),
                        (long long)fileInfo.st_size, time,
                        entry->d_name); 

         if ((size_of_block_padding = floor(log10(abs((int)fileInfo.st_blocks/2))) + 1) > block_padding) block_padding = size_of_block_padding;
         if ((size_of_user_padding = strlen(username(fileInfo.st_uid, uname))) > user_padding) user_padding = size_of_user_padding;
         if ((size_of_group_padding = strlen(groupname(fileInfo.st_gid, gname))) > group_padding) group_padding = size_of_group_padding;
         if ((size_of_size_padding = floor(log10(abs((int)fileInfo.st_size))) + 1) > size_padding) size_padding = size_of_size_padding;

         //printf("%lld\n", (long long)fileInfo.st_rdev);
      }
   }

   printf("total %ld\n", total);
   print_list(head, block_padding, user_padding, group_padding, size_padding);

   // finish up
   closedir(dir_p);
   return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode(mode_t mode, char *str)
{
   int i = 0;
   str[i] = '?';
   if ((mode & S_IFLNK) == S_IFLNK) str[i] = 'l'; 
   if ((mode & S_IFDIR) == S_IFDIR) str[i] = 'd';
   if ((mode & S_IFREG) == S_IFREG) str[i] = '-';
   i++;

   (mode & S_IRUSR) ? (str[i] = 'r') : (str[i] = '-'); i++;
   (mode & S_IWUSR) ? (str[i] = 'w') : (str[i] = '-'); i++;
   (mode & S_IXUSR) ? (str[i] = 'x') : (str[i] = '-'); i++;
   (mode & S_IRGRP) ? (str[i] = 'r') : (str[i] = '-'); i++;
   (mode & S_IWGRP) ? (str[i] = 'w') : (str[i] = '-'); i++;
   (mode & S_IXGRP) ? (str[i] = 'x') : (str[i] = '-'); i++;
   (mode & S_IROTH) ? (str[i] = 'r') : (str[i] = '-'); i++;
   (mode & S_IWOTH) ? (str[i] = 'w') : (str[i] = '-'); i++;
   (mode & S_IXOTH) ? (str[i] = 'x') : (str[i] = '-'); i++;
   str[i] = '\0';

   //printf("%s\n", str);

   return str;
}

// convert user id to user name
char *username(uid_t uid, char *name)
{
   struct passwd *uinfo = getpwuid(uid);
   if (uinfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)uid);
   else
      snprintf(name, MAXNAME, "%s", uinfo->pw_name);
   return name;
}

// convert group id to group name
char *groupname(gid_t gid, char *name)
{
   struct group *ginfo = getgrgid(gid);
   if (ginfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)gid);
   else
      snprintf(name, MAXNAME, "%s", ginfo->gr_name);
   return name;
}

// returns the head of the sorted list of directories
struct meta_data *sorted_append(struct meta_data *head, blkcnt_t blocks, char *perms, nlink_t links, char *user, char *group, off_t size, char *time, char *name)
{
   // allocate mem
   //printf("%s\n", perms);
   struct meta_data *new_node = malloc(sizeof(struct meta_data));

   // mem check
   if (new_node == NULL)
   {
      printf("error: memory could not be allocated for struct\n");
      return NULL;
   }
   
   // populate structure
   new_node->blocks = blocks/2;
   strcpy(new_node->perms, perms);
   new_node->links = links;
   strcpy(new_node->user, user);
   strcpy(new_node->group, group);
   new_node->size = size;
   strcpy(new_node->time, time);
   strcpy(new_node->name, name);
   new_node->next = NULL;

   // perform linkage of structure
   if (head == NULL)
   {
      head = new_node;
      new_node->next = NULL;
      return head;
   }

   struct meta_data *curr = head;
   struct meta_data *prev = head;
   struct meta_data *tmp = NULL;
   while (curr != NULL)
   {
      int comparison = strcasecmp(new_node->name, curr->name);
      if (comparison == IDENTICAL)
      {
         if (strcmp(curr->name, new_node->name) > IDENTICAL)
         {
            if (curr == head)
            {
               tmp = head;
               head = curr;
               curr->next = tmp;
               return head;
            } 
            tmp = curr;
            prev->next = curr;
            curr->next = tmp;
            return head;
         }
      }
      else if (comparison < IDENTICAL) {
         if (curr == head)
         {
            tmp = head;
            head = new_node;
            new_node->next = tmp;
            return head;
         } 
            tmp = curr;
            prev->next = new_node;
            new_node->next = tmp;
            return head;
      } 
      if (comparison > IDENTICAL && curr->next == NULL)
      {
         curr->next = new_node;
         return head;
      }

      prev = curr;
      curr = curr->next;
   }

   return head;
}

void print_list(struct meta_data *head, int block_padding, int user_padding, int group_padding, int size_padding)
{
   if (head == NULL)
   {
      printf("no list to print\n");
      return;
   }
   struct meta_data *iterator = head;
   while (iterator != NULL)
   {
      // printing separately for more control over colour and padding
      printf("%*ld ", block_padding, iterator->blocks);
      printf("%s ", iterator->perms);
      printf("%ld ", iterator->links);
      printf("%*s ", user_padding, iterator->user);
      printf("%*s ", group_padding, iterator->group);
      printf("%*ld ", size_padding, iterator->size);
      printf("%s ", iterator->time);
      (iterator->perms[0] == 'd') ? printf(ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, iterator->name) : (iterator->perms[3] == 'x' || iterator->perms[6] == 'x' || iterator->perms[9] == 'x') ? printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, iterator->name) : printf("%s\n", iterator->name);

      iterator = iterator->next;
   }
}

// printf ("% *d\n", 3, 5); //where 3 is the amount of padding and 5 is the number to print