#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termcap.h>
#include <time.h>

const char *man[] = { " O\n", "/", "|", "\\\n", " /", "\\\n"};

void get_word(FILE *file, char *secret);
void clear_screen();

int main(int argc, char **argv)
{
	srand(time(NULL));
	const char *word_file = argc > 1 ? argv[1] : "~/.words";
	FILE *words = fopen(word_file, "r");
	char secret[35] = {'\0'};
	get_word(words, secret);
	if(!strlen(secret)) {
		// some error for 0 length word
		return 1;
	}
	fclose(words);

	int guesses = 6;
	while(guesses-->0)
	{
		// display current hangman status
		// prompt for guess
		// compute guess
		// update stats
	}
	

}

void get_word(FILE *file, char *secret)
{
	if(!file || !secret) return;
//	char word[36] = {'\0'};
	int lines = 0;
	char c;
	while(EOF != (c=fgetc(file))) lines += (c == '\n');
	printf("line count: %i\n", lines);
//	int line_to_get = rand() % lines;
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
