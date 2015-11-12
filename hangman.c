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
    int n = 0;
    char c;
    char word[36] = {'\0'};
    if(!file || !secret) return;
    while(fgets(word, sizeof(word), file))
	{
        // word too long?
        if(!word[strlen(word)-1] == '\n')
        {
            while('\n' != (c=getc(file))); //read until end of line
            continue;
        }
        word[strlen(word)-1] = '\0';
        // word only alphanumeric + spaces?
        int i;
        for(i=0; i < (int)strlen(word); i++)
        {
            if((word[i] != ' ' && word[i] < 'A') ||
               (word[i] > 'Z' && word[i] < 'a') ||
                word[i] > 'z')
            {
                 break;
            }
        }
         n += (i == (int)(strlen(word)));
        double x = (double)rand() / (double)(RAND_MAX);
		if(x < (1/(double)n)) strncpy(secret, word, 35);
	}
	printf("word is %s\n", secret);
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
