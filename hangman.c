#include <stdio.h>
#include <stdlib.h>
#include <termcap.h>

void clear_screen();

int main(void)
{

}

/* Retrieved from: http://stackoverflow.com/a/17271828
*/
void clear_screen()
{
	char buf[512];
	char *str;
	char cl[] = {'c','l','\0'};

	tgetent(buf, getenv("TERM"));
	str = tgetstr(cl, NULL);
	fputs(str, stdout);
} 
