#include <errno.h>
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
	char *homedir = getenv("HOME");
	const char *word_file = argc > 1 ? argv[1] : strcat(homedir, "/.words");
	printf("using %s\n", word_file); //REMOVE
	FILE *words = fopen(word_file, "r");
	if(!words) {
		perror("Problem opening file");
		return errno;
	}
	char secret[35] = {'\0'};
	int known[35] = {0};
	int used[26] = {0};
	get_word(words, secret);
	if(!strlen(secret)) {
		// some error for 0 length word
		return 1;
	}
	fclose(words);

	int secret_len = strlen(secret);
	//prep secret
	for(int i=0; i < secret_len; i++)
	{
		if(secret[i] >= 'a' && secret[i] <= 'z') secret[i] -= 32;
		known[i] += (secret[i] == ' ');
	}

	int wrong = 0;
	while(wrong < 6)
	{
		clear_screen();
		for(int i=0; i < wrong; i++) printf("%s", man[i]);
		printf("\n");
		for(int i=0; i < secret_len; i++)
		{
			if(secret[i] == ' ') printf("  ");
			else printf("%c ", known[i] ? secret[i] : '_');
		}
		printf("\nUsed: ");
		for(int i=0; i < 26; i++) if(used[i]) printf("%c ", i+65);
		printf("\nGuess: ");
		char c = getchar();
		if(c < 'A' || (c > 'Z' && c < 'a') || c > 'z') continue;
		c -= (c >= 'a' && c <= 'z') ? 32 : 0; // Capitalize the guess
		used[c-65]++;
		char n;
		while('\n' != (n=getchar()));
		int flag = 0;
		for(int i=0; i < secret_len; i++) {
			flag += (c == secret[i]);
			known[i] += (c == secret[i]);
		}
		wrong += (!flag);
		int i;
		for(i=0; i < secret_len; i++)
		{
			if(!known[i]) break;
		}
		if(i == secret_len)
		{
			printf("\n");
			for(int i=0; i < secret_len; i++)
			{
				if(secret[i] == ' ') printf("  ");
				else printf("%c ", known[i] ? secret[i] : '_');
			}
			printf("\nYou win!\n");
			return 0;
		}
		// update stats
	}
	printf("You lose!\n");
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
