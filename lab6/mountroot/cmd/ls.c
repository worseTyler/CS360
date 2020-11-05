#include "../type.h"

// globals
extern MINODE minode[NMINODE];
extern MINODE *root;

extern PROC   proc[NPROC], *running;

extern char gpath[128]; // global for tokenized components
extern char *name[32];  // assume at most 32 components in pathname
extern int   n;         // number of component strings

extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;

int ls_file(MINODE *mip, char *name);
int ls_dir(MINODE *mip);
int ls(char *pathname);

int ls_file(MINODE *mip, char *name)
{
    // READ Chapter 11.7.3 HOW TO ls

    char *t1 = "xwrxwrxwr-------";
    char *t2 = "----------------";

    char ftime[256];
    u16 mode = mip->INODE.i_mode;

    printf("hi\n");

    if (S_ISREG(mode)) // if (S_ISREG())
        printf("%c", '-');
    if (S_ISDIR(mode)) // if (S_ISDIR())
        printf("%c", 'd');
    if (S_ISLNK(mode)) // if (S_ISLNK())
        printf("%c", 'l');
    for (int i = 8; i >= 0; i--)
    {
        if (mode & (1 << i))
            printf("%c", t1[i]); // print r|w|x printf("%c", t1[i]);
        else
            printf("%c", t2[i]); // or print -
    }
    printf("%4d ", mip->INODE.i_links_count); // link count
    printf("%4d ", mip->INODE.i_gid);   // gid
    printf("%4d ", mip->INODE.i_uid);   // uid
    printf("%8ld ", mip->INODE.i_size); // file size

    strcpy(ftime, ctime(&(mip->INODE.i_mtime))); // print time in calendar form ftime[strlen(ftime)-1] = 0; // kill \n at end
    ftime[strlen(ftime) - 1] = 0;        // removes the \n
    printf("%s ", ftime);                // prints the time

    printf("%s", name);
    if (S_ISLNK(mode))
    {
        printf(" -> %s", (char *) mip->INODE.i_block); // print linked name }
    }

    printf("\n");
    return 0;
}

int ls_dir(MINODE *mip)
{
    char buf[BLKSIZE], temp[256];
    DIR *dp;
    char *cp;

    // Assume DIR has only one data block i_block[0]
    get_block(dev, mip->INODE.i_block[0], buf);
    dp = (DIR *)buf;
    cp = buf;

    while (cp < buf + BLKSIZE)
    {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;

        printf("[%d %s]  ", dp->inode, temp); // print [inode# name]

        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    printf("\n");
}

int ls(char *pathname)
{
    printf("ls %s\n", pathname);
    if (!strcmp(pathname, ""))
    {
        ls_dir(running->cwd);
    }
    else
    {
        int ino = getino(pathname);
        if (ino == 0)
        {
            printf("inode DNE\n");
            return -1;
        }
        else
        {
            int dev = root->dev;
            MINODE *mip = iget(dev, ino);
            ls_file(mip, pathname);
        }
    }
}