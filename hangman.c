#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const char *man[] = { " O\n", "/", "|", "\\\n", " /", "\\\n"};
typedef struct {
	int games;
	int wins;
	int guesses;
} stats;

void get_word(FILE *file, char *secret);
int readwrite_stats(stats *stats, const char *mode);
int finish_game(stats *stats);

int main(int argc, char **argv)
{
	srand(time(NULL));
	int err = 0;
	char homedir[50];
	strncpy(homedir, getenv("HOME"), sizeof(homedir));
	const char *word_file = argc > 1 ? argv[1] : strncat(homedir, "/.words", sizeof(homedir));
	printf("using dictionary file %s\n", word_file);

	FILE *words = fopen(word_file, "r");
	if(!words) {
		perror("Problem opening file");
		return errno;
	}
	char secret[35] = {'\0'};
	get_word(words, secret);
	int secret_len = strlen(secret);
	if(!secret_len) {
		fprintf(stderr, "0 character string returned.\n");
		return 1;
	}
	if(EOF == fclose(words)) {
		perror("Error closing dictionary file");
		return errno;
	}

	stats *stats = malloc(sizeof(*stats));
	err = readwrite_stats(stats, "r");
	if(err) return err;

	
	//prep secret
	int known[35] = {0};
	int used[26] = {0}; // tracks which letters have been tried
	for(int i=0; i < secret_len; i++)
	{
		if(secret[i] >= 'a' && secret[i] <= 'z') secret[i] -= 32;
		known[i] += (secret[i] == ' ');
	}

	int wrong = 0;
	int guesses = 0;
	while(wrong < 6)
	{
		for(int i=0; i < wrong; i++) printf("%s", man[i]);
		printf("\n");
		for(int i=0; i < secret_len; i++)
		{
			if(secret[i] == ' ') printf("  ");
			else printf("%c ", known[i] ? secret[i] : '_');
		}
		double avg = stats->wins > 0 ? (double)stats->guesses/(double)stats->wins : 0;
		printf("\n%i Game%s\n%i Win%s\n%.1f Avg. Guess%s per Winning Game\n",
				stats->games, stats->games == 1 ? "" : "s",
				stats->wins, stats->wins == 1 ? "" : "s",
				avg, avg < 1 || avg > 1 ? "es": "");
		printf("\nUsed: ");
		for(int i=0; i < 26; i++) if(used[i]) printf("%c ", i+65);
		printf("\nGuess: ");
		char c = getchar();
		while('\n' != getchar());
		if(system("clear") < 0) fprintf(stderr, "Error clearing screen.\n");

		if(c < 'A' || (c > 'Z' && c < 'a') || c > 'z') continue;
		guesses++;
		c -= (c >= 'a' && c <= 'z') ? 32 : 0; // Capitalize the guess
		used[c-'A']++;
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
			stats->games++;
			stats->guesses += guesses;
			return finish_game(stats);
		}
	}
	printf("You lose! The answer was %s\n", secret);
	stats->games++;
	return finish_game(stats);
}

void get_word(FILE *file, char *secret)
{
	int n = 0;
	char c;
	char word[36] = {'\0'};
	if(!file || !secret) return;
	while(fgets(word, sizeof(word), file))
	{
		// word has no characters?
		if(word[0] == '\n') continue;
		// word too long?
		if(!word[strlen(word)-1] == '\n') {
			while('\n' != (c=getc(file))); //read until end of line	
			continue;
		}
		word[strlen(word)-1] = '\0';
		// word only alphanumeric + spaces?
		int i;
		for(i=0; i < (int)strlen(word); i++)
		{
			if((word[i] != ' ' && word[i] < 'A') || (word[i] > 'Z' && word[i] < 'a') || word[i] > 'z') {
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
 *
 * returns int
 *  < 0: system call error
 *    0: no error
 *    1: invalid filemode argument ('r' or 'w' only).
 */
int readwrite_stats(stats *stats, const char *mode)
{
	int err = 0;
	char statfilename[50];
	strncpy(statfilename, getenv("HOME"), sizeof(statfilename));
	strncat(statfilename, "/.hangman", sizeof(statfilename));
	FILE *statfile;
	if(mode[0] == 'r') {
		if(access(statfilename, F_OK) == -1) { // if the file doesn't exist
			statfile = fopen(statfilename, "w+");
			if(!statfile) {
				perror("Failed to create new ~/.hangman file.");
				return errno; //dont try to close
			} else {
				if(0 > fprintf(statfile, "0 0 0"))	{
					perror("Output error occurred when initializing new ~/.hangman file.");
					err = errno;
				}
			}
			stats->games = stats->wins = stats->guesses = 0;
		} else { // file already exists
			statfile = fopen(statfilename, mode);
			if(!statfile) {
				perror("Failed to open ~/.hangman for reading.");
				return errno; //dont try to close
			} else if (EOF == fscanf(statfile, "%i %i %i", &(stats->games), &(stats->wins), &(stats->guesses))) {
				perror("Error reading from ~/.hangman. File may be corrupt.");
				err = errno;
			}
		}
	} else if (mode[0] == 'w') {
		statfile = fopen(statfilename, mode);
		if(!statfile) {
			perror("Failed to open ~/.hangman for writing");
			return errno; //dont try to close
		}
		
		if(0 > fprintf(statfile, "%i %i %i", stats->games, stats->wins, stats->guesses))	{
			perror("Output error occurred when writing to ~/.hangman file.");
			err = errno;
		}
	} else {
		perror("Unknown file mode passed to readwrite_stats().");
		return 1;
	}
	if(EOF == fclose(statfile))	{
		perror("Error closing ~/.hangman file.");
		err = errno;
	}
	return err;
}

/* Attempts to write game stats and deallocate
 * Returns  */
int finish_game(stats *stats)
{
	int err = readwrite_stats(stats, "w");
	if(err) return err;
	free(stats);
	return 0;
}
