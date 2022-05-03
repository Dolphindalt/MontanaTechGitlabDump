/*
 * S20 CSCI 460 Operating Systems
 * 
 * Using part-1 and part-2 results, obtain from the user the kernel
 * image to load, locate this in the ext2 filesystem, and then load
 * into memory and return to the assembly
 * 
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRK 18
#define CYL 36
#define BLK 1024
#define EOL '\r'
#define NL '\n'

#include "ext2.h"
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

/* include your prints from part-1 */
void prints(char *s)
{
   while (*s != '\0')
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
   *cptr = '\0';
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
   u8 i, j, d;
   DIR *dp;
   for (i = 0; i < 12; i++)
   {
         getblk((u16)ip->i_block[i], buf2);
         dp = (DIR *)buf2;
         while ((char *)dp < &(buf2[BLK]))
         {
			if (dp->file_type == 2 || dp->file_type == 1)
				prints(dp->name);
            j = 0;
            d = 0;
            do
            {
               d += dp->name[j] - name[j];
            } while (++j < dp->name_len);
            if (d == 0)
            	return (u16)dp->inode;
            dp = (DIR *)((char *)dp + dp->rec_len);
         }
   }
} // end search

/*
 * using the prints, gets, getblk, and search
 * obtain from the user the kernel image to  
 * boot and then search for this image in the
 * file system, and if found load into memory
 */
int main()
{
   u16 i;
   u16 me, blk, iblk;
   char *cp, *name[2], filename[64];
   u32 *up;
   GD *gp;
   INODE *ip;
   DIR *dp;

   name[0] = "boot";
   name[1] = filename;

   prints("boot (mtx) : ");

   gets(filename, 64);

   if (filename[0] == 0)
      name[1] = "mtx";

   /* read blk#2 to get group descriptor 0 */
   getblk(2, buf1);
   gp = (GD *)buf1;
   iblk = (u16)gp->bg_inode_table;
   getblk(iblk, buf1);     // read first inode block block
   ip = (INODE *)buf1 + 1; // ip->root inode #2

   /* serach for system name */
   for (i = 0; i < 2; i++)
   {
      me = search(ip, name[i]) - 1;
      getblk(iblk + (me / 8), buf1); // read block inode of me
      ip = (INODE *)buf1 + (me % 8);
   }

   /* read indirect block into b2 */
   if ((u16)ip->i_block[12])
      getblk((u16)ip->i_block[12], buf2);

   setes(0x1000);
   /* load direct blocks into memory
    * remember to use inces() to increment es 
    * stack register
    */
   for (i = 0; i < 12; i++)
   {
      getblk((u16)ip->i_block[i], 0);
      inces();
   }

   if ((u16)ip->i_block[12])
   {  // only if file has indirect blocks
      /* load indirect blocks into memory
    * remember again to use inces() to increment es
    * stack register 
    */
	  // The docs say these are 4 byte values.
      u32 *bp = (u32 *) buf2;
	  while (*bp)
	  {
		getblk((u16)*bp++, 0);
		inces(); 
	  }
   }
   
   prints("\n\rBoot? ");
   getc();
}
