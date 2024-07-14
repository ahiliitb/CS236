#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
 *
 */
char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++)
	{

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t')
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0)
			{
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else
		{
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}

int main(int argc, char *argv[])
{
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;

	while (1)
	{
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; // terminate with new line
		tokens = tokenize(line);

		int no_of_tokens = 0;
		while (tokens[no_of_tokens] != NULL)
		{
			no_of_tokens++;
		}
		

		// do whatever you want with the commands, here we just print them

		///////////////////// My Code Starts here ///////////////////////////

		if (strcmp(tokens[no_of_tokens-1], "&") == 0)
		{
			char* new_tokens[no_of_tokens];
			int i = 0;
			while (i != no_of_tokens)
			{
				new_tokens[i] = tokens[i+1];
				i++;
			}
			
			int fork_ret = fork();
			if (fork_ret == 0)
			{
				execvp(new_tokens[0], new_tokens);
				if (new_tokens[0] != NULL)
				{
					printf("Shell: Incorrect command\n");
				}
			}
			else
			{
				int wstatus;
				waitpid(-1, &wstatus, WNOHANG);
				kill(fork_ret, SIGKILL);
			}
		}
		else
		{
			if (strcmp(tokens[0], "cd") == 0)
			{
				int ch_ret = chdir(tokens[1]);
				if (ch_ret < 0)
				{
					printf("Shell: Incorrect command\n");
				}
			}
			else
			{
				int fork_ret = fork();
				if (fork_ret == 0)
				{
					execvp(tokens[0], tokens);
					if (tokens[0] != NULL)
					{
						printf("Shell: Incorrect command\n");
					}
				}
				else
				{
					wait(NULL);
				}
			}
		}

		//////////////////// My Code ends here ////////////////////////////////

		// for (i = 0; tokens[i] != NULL; i++)
		// {
		// 	printf("found token %s (remove this debug output later)\n", tokens[i]);
		// }

		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++)
		{
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}
