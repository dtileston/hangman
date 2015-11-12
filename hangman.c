#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termcap.h>
#include <time.h>

const char *man[] = { " O\n", "/", "|", "\\\n", " /", "\\\n"};
typedef struct {
	int games;
	int wins;
	int guesses;
} stats;

void get_word(FILE *file, char *secret);
void readwrite_stats(stats *stats, const char *mode);
void clear_screen();

int main(int argc, char **argv)
{
	// testing readwrite_stats
	stats *stats = malloc(sizeof(stats));;
	readwrite_stats(stats, "r");
	stats->games++;
	printf("%i games, %i wins, %i guesses\n", stats->games, stats->wins, stats->guesses);
	srand(time(NULL));
	char homedir[50];
	strncpy(homedir, getenv("HOME"), sizeof(homedir));
	const char *word_file = argc > 1 ? argv[1] : strncat(homedir, "/.words", sizeof(homedir));
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
	int guesses = 0;
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
		guesses++;
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
			stats->wins++;
			return 0;
		}
		// update stats
		stats->guesses += guesses;
		readwrite_stats(stats, "w");
	}
	printf("You lose! The answer was %s\n", secret);
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

/* File stores the following on one line
 * <# of games> <# of wins> <# total guesses>
 *
 * function accepts pointer to stats structure and a file mode
 * mode can be either "r" or "w"
 */
void readwrite_stats(stats *stats, const char *mode)
{
	char statfilename[50];;
	strncpy(statfilename, getenv("HOME"), sizeof(statfilename));
	strncat(statfilename, "/.hangman", sizeof(statfilename));
	printf("file %s\n", statfilename);
	FILE *statfile = fopen(statfilename, mode);
	if(mode[0] == 'r') {
		fscanf(statfile, "%i %i %i", &(stats->games), &(stats->wins), &(stats->guesses));
	} else {
		fprintf(statfile, "%i %i %i", stats->games, stats->wins, stats->guesses);
	}
	fclose(statfile);
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
