/*
 * S20 CSCI 460 Operating Systems
 * 
 * Impelment a print string and get string routine to integrate with 
 * the assembly code provided
 * 
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */

// Manifest constants
#define EOL '\r'
#define NL '\n'
#define EOS '\0'
#define BUF_SZ 50

extern void putc(char);

extern char getc();

// needed for reference from assembly
unsigned short NSEC = 0;

/*
 * Outputs a character array to the terminal by
 * calling the putc() function from the assembly
 */
void prints(char *s)
{
	while (*s != EOS)
		putc(*s++);
} // end prints

/*
 * Reads a line of text from the terminal 
 * and implements basic line editing 
 */
void gets(char *s, int sz)
{ 
	char *cptr = s;
	char *t;
	sz--;
	while ( (*(t=s+sz) = getc()) != EOL)
	{
		if ( (*(t) == '\b' && (cptr == s)) || (*(t) != '\b' && ( (cptr - s) == sz) ) )
		{
			continue;
		}
		putc(*cptr++ = *(t) );
		if ( *(cptr-1) == '\b')
		{
			prints(" \b");
			cptr -= 2;
		}
	}
	*cptr = EOS;
	
} // end gets

/*
 * Prompt the user for their name
 * Print their name to the terminal
 * Continue in this fashion until the
 * user types blank line - then halt
 */
int main() { 
	char buf[BUF_SZ];
	do {
		prints("$: ");
		gets(buf, BUF_SZ);
		prints("\n\r");
		prints(buf);
		prints("\n\r");
	} while (buf[0] != EOS);
} // end main

