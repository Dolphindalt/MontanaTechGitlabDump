/*
 * S20 CSCI 460 Operating Systems
 * 
 * Implement the search function and then use 
 * the search function to walk through the ext2 
 * filesystem and print out the directory entries
 * 
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define EOL '\r'
#define NL '\n'
#define EOS '\0'

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"

typedef struct ext2_super_block SB;
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

/* include your prints from part-1 */
void prints(char *s)
{
   while (*s != EOS)
      putc(*s++);
}

/* include your gets from part-1 */
void gets(char *s, int sz)
{
   char *cptr = s;
   char *t;
   sz--;
   while ((*(t = s + sz) = getc()) != EOL)
   {
      if ((*(t) == '\b' && (cptr == s)) || (*(t) != '\b' && ((cptr - s) == sz)))
      {
         continue;
      }
      putc(*cptr++ = *(t));
      if (*(cptr - 1) == '\b')
      {
         prints(" \b");
         cptr -= 2;
      }
   }
   *cptr = EOS;
}

u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

/*
 * Mailman algorithm to convert from blocks to 
 * cylinder, head, sector so disk BIOS can be
 * used from the assembly - via readfd
 */
u16 getblk(u16 blk, char *buf)
{
   readfd(blk / 18, ((blk) % 18) / 9, (((blk) % 18) % 9) << 1, buf);
}

/*
 * Search the ext2 filesystem and print out the 
 * directory entries that are found starting at
 * the INODE given
 */
u16 search(INODE *ip, char *name)
{
   // understand the ext2 filesystem layout
   // use the resources provided
   // remember the B-Tree data structure from CSCI 232
   u8 i, j, d;
   u16 ret = -1;
   DIR *dp = (DIR *) (ip->i_block);
   for (i = 0; i < 12; i++)
   {
	  if ((u16)ip->i_block[i])
	  {
      	getblk((u16)ip->i_block[i], buf2);
	  	dp = (DIR *) buf2;
		while ((char *)dp < &(buf2[BLK]))
		{
			prints(dp->name);
			putc(' ');
			j = 0;
			d = 0;
			do {
				d += dp->name[j] - name[j];
			} while (++j < dp->name_len);
			if (d == 0)
				ret = (u16)dp->inode;
			dp = (DIR *)((char *) dp + dp->rec_len);
		}
		prints("\n\r");
	  }
   }
   return ret;
   // remember to return something
} // end search

/*
 * using the prints, gets, getblk, and search
 * print out the directory entries for the 
 * root directory (/) and the boot directory
 * (/boot)
 */
int main()
{
   SB *sblk;
   u16 i;
   u16 me, blk, iblk;
   char *cp;
   u32 *up;
   GD *gp;
   INODE *ip;
   DIR *dp;

   // read blk#1 to get file system characteristics.
   getblk(1, buf1);
   sblk = (SB *)buf1;
   if (sblk->s_magic != EXT_2_MAGIC)
      error();

   /* read blk#2 to get group descriptor 0 */
   getblk(2, buf1);
   gp = (GD *)buf1;
   iblk = (u16)gp->bg_inode_table;
   getblk(iblk, buf1);     // read first inode block block
   ip = (INODE *)buf1 + 1; // ip->root inode #2

   /* search for boot in the file system  */
   prints("/ dir\n\r");
   me = search(ip, "boot") - 1;
   if (me < 0)
      error();                    // defined in assembly
   getblk(iblk + (me / 8), buf1); // read block inode of me
   ip = (INODE *)buf1 + (me % 8);

   prints("/boot dir\n\r");
   search(ip, "d");

   /* read indirect block into b2 */
   if ((u16)ip->i_block[12]) // only if has indirect blocks
      getblk((u16)ip->i_block[12], buf2);

   while(1);

   return 0;
}
