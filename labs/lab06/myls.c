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

#define MAXDIRNAME 100
#define MAXFNAME   200
#define MAXNAME    20

char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);

int main(int argc, char *argv[])
{
   // string buffers for various names
   char dirname[MAXDIRNAME];
   char uname[MAXNAME+1];
   char gname[MAXNAME+1];
   char mode[MAXNAME+1];
   char path[MAXDIRNAME];

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

   // open the directory to start reading
   /*
   DIR *opendir(const char *name);

   The  opendir()  function opens a directory stream corresponding to the directory name, and returns a pointer
   to the directory stream.  The stream is positioned at the first entry in the directory.
   */
  
   DIR *dir_p = opendir(dirname);

   if (dir_p == NULL)
   {
      printf("error, directory open failed\n");
      return 1;
   }

   // read directory entries
   /*
   struct dirent *readdir(DIR *dirp);
    
   The  readdir() function returns a pointer to a dirent structure representing the next directory entry in the
       directory stream pointed to by dirp.  It returns NULL on reaching the end of the directory stream or  if  an
       error occurred.

       On Linux, the dirent structure is defined as follows:

           struct dirent {
               ino_t          d_ino;       // inode number
               off_t          d_off;       // not an offset; see NOTES
               unsigned short d_reclen;    // length of this record
               unsigned char  d_type;      // type of file; not supported
                                              by all filesystem types
               char           d_name[256]; // filename
           };
   */
  
   struct dirent *entry;
   struct stat meta_data;

   while ((entry = readdir(dir_p)) != NULL)
   {
      if (entry->d_name[0] != '.')
      {
         // 1. build path to current file
         memset(path,0,sizeof(path));
         strcat(path, dirname);      
         strcat(path, "/");
         strcat(path, entry->d_name);
         strcat(path, "\0");
         //printf("%s\n", path);
         //printf("%s\n", entry->d_name);

         // 2. build stat struct with meta data from current file
         stat(path, &meta_data);

         // 3. output meta data
         printf("%s  %-8.8s %-8.8s %8lld  %s\n",   rwxmode(meta_data.st_mode, mode),
                                                   username(meta_data.st_uid, uname),
                                                   groupname(meta_data.st_gid, gname),
                                                   (long long)meta_data.st_size,
                                                   entry->d_name);
      }
   }

   //printf("%s\n", dirname);
   //printf("%d\n", dir_len);

   // finish up
   closedir(dir_p);
   return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode(mode_t mode, char *str)
{
   // a ? b : c, is a ternary b or c (b is a is true, c otherwise)
   // chpt
   int i = 0;
   if ((mode & S_IRUSR) ? strcpy(&str[i], "r") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IWUSR) ? strcpy(&str[i], "w") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IXUSR) ? strcpy(&str[i], "x") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IRGRP) ? strcpy(&str[i], "r") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IWGRP) ? strcpy(&str[i], "w") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IXGRP) ? strcpy(&str[i], "x") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IROTH) ? strcpy(&str[i], "r") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IWOTH) ? strcpy(&str[i], "w") : strcpy(&str[i], "-")); i++;
   if ((mode & S_IXOTH) ? strcpy(&str[i], "x") : strcpy(&str[i], "-")); i++;

   return str;

   /*
   S_IRWXU     00700   000000111000000 owner has read, write, and execute permission
   S_IRUSR     00400   000000100000000 owner has read permission
   S_IWUSR     00200   000000010000000 owner has write permission
   S_IXUSR     00100   000000001000000 owner has execute permission

   S_IRWXG     00070   000000000111000 group has read, write, and execute permission
   S_IRGRP     00040   000000000100000 group has read permission
   S_IWGRP     00020   000000000010000 group has write permission
   S_IXGRP     00010   000000000001000 group has execute permission

   S_IRWXO     00007   000000000000111 others (not in group) have read,  write,  and execute permission
   S_IROTH     00004   000000000000100 others have read permission
   S_IWOTH     00002   000000000000010 others have write permission
   S_IXOTH     00001   000000000000001 others have execute permission
   */
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
