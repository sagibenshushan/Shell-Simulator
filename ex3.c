

#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>

char *trimWhitespace(char *str);
int countWords(char* inputStr);
char** arrayOfWord(char* str1,int numOfWords, char** arrayOfWord);
char* substring(char*, int, int, char*);
int getNumOfPipes(char* str1);
char** arrayOfCommands(char* str1, int numOfPipes, char **cmd);
void execCommands(char *str1, int numOfPipes);
char* replaceSubstring(char* sourceStr, char* subStr, char* newStr);
void hendler(int number);
bool isSourceStrContainsSubStr(char* sourceStr, char* subStr);
char* parseString(char* str1);


int historyIndex;                     // set history index to public
char history[200][510];               //define public history array of strings

int numOfTotalWords = 0;
int numOfTotalCommands = 0;

int main() {

    signal(SIGCHLD,hendler);
    int numberOfPipes = 0;

    const char fname[] = "history.txt";
    int i = 0;
    historyIndex = 0;
    int currentcommands = 0;

    FILE* fp;

    fp = fopen(fname, "r+");
    if (fp != NULL)
    {
        while (fgets(history[historyIndex], sizeof history[historyIndex], fp))          //read from file and write into history array untill NULL
        {
            history[historyIndex][strlen(history[historyIndex]) - 1] = '\0';
            historyIndex++;
        }
        fclose(fp);
    }

    char* trimedWord = "";
    char str1[512] = "";

    while(strcmp(str1, "done") != 0)     // while input != "done"
    {
        char Buffer[PATH_MAX];
        if (getcwd(Buffer, sizeof(Buffer)) == NULL) {      //get the path of the folder
            perror("getcwd() error");
            exit(1);
        }

        printf("%s>", Buffer);  //show the path of the folder
        fgets(str1, sizeof str1, stdin);                                                    //get input string from the user into str1

        trimedWord = trimWhitespace(str1);                                                  //send str1 to the function and received the string without spaces on sides
        if(strlen(trimedWord) == 0)
            continue;

        parseString(str1);  //replace all !xx to the command from history

        if (strcmp(str1, "done") == 0)
        {
            break;
        }
        else if (strcmp(str1, "cd") == 0)
        {
            printf("command not supported (Yet)\n");
            fflush(stdout);
        }
        else if(strlen(str1) > 0)
        {
            if (str1[0] == ' ') {
                printf("this command is not exist\n");
                fflush(stdout);
            }

            else if (strcmp(str1, "history") == 0)
            {
                for (i = 1; i < historyIndex; ++i) {
                    printf(" %d %s\n", i, history[i]);
                    fflush(stdout);
                }
            }
            else if(strlen(str1) > 0)
            {
                int numOfPipes = getNumOfPipes(str1);
                numberOfPipes += numOfPipes;                                      // for total counter at "done"
                if(numOfPipes > 2)
                {
                    printf("More than 2 pipes is not valid command\n");
                    continue; //
                }
                execCommands(str1, numOfPipes);
            }

            historyIndex++;   //count of the lines in history array and numbers of inputs(commands)
            currentcommands++; // count commands in current run;
        }
    }

    numOfTotalWords += numberOfPipes;
    printf("Num of commands: %d\n", numOfTotalCommands);                                                          //print if entered done , +1 for the command "word"
    printf("Total numbers of words in all commands: %d\n", numOfTotalWords);


    fp = fopen(fname, "w");
    for (i = 0; i < historyIndex; i++)
    {
        fputs(history[i], fp);
        fprintf(fp, "\n");
    }
    fclose(fp);

    return 0;
}

int countWords(char* inputStr)
{
    int countWord = 0;
    char* str1 = trimWhitespace(inputStr);                                                                    //remove the leading and trailing spaces

    for (int i = 0; i <= (strlen(str1)); i++)
    {
        if (str1[i] == ' ' || str1[i] == '\0')                                                                    //stop condition is if we arrive to the end of the word
        {
            countWord++;
        }

    }
    return countWord;
}

char** arrayOfWord(char* str1,int numOfWords, char** arr){

    int countWord = 0;
    int countChars = 0;
    for (int i = 0; i <= (strlen(str1)); i++)                                                                     //stop condition is the lenght of str1
    {
        if (str1[i] == ' ' || str1[i] == '\0')
        {
            countChars = 0;
            countWord++;
        }

        else if (str1[i] != '\n')                                                         //the \n (new line) is a part of the input string and we don't want to count it
        {
            arr[countWord][countChars] = str1[i];                                                                      //build array of words
            countChars++;
        }
    }
    arr[numOfWords] = NULL;                                                                                    // put null after the command
    return arr;
}

char *trimWhitespace(char *str)
{
    char *end;                                                                // pointer of the last char in str

    // Trim leading space
    while(isspace((unsigned char)*str))
        str++;         // one step over if the char is space

    if(*str == 0)  // All spaces, if the string is null
        return str;

    end = str + strlen(str) - 1;                // after we clean the leading spaces we step strln(str) times to make end point to the last char
    while(end > str && isspace((char)*end))    // Trim trailing space (remove the spaces in the end of the string)
        end--;

    end[1] = '\0';    // add \0 to the end of str

    return str;
}

char *substring(char *string, int position, int length, char *p)     //check if null and then i copy the string without the leading '!'.
{
    int c;

    for (c = 0; c < length; c++)
    {
        *(p+c) = *(string+position-1);                    //every loop i make 1 step of the pointer on the string and copy the char to p
        string++;
    }
    *(p+c-1) = '\0';                                     //end of p

    return p;
}

int getNumOfPipes(char* str1)
{
    int numOfPipes = 0;
    for (int i = 0; i <= (strlen(str1)); i++)                                                    //stop condition is the lenght of str1
    {
        if (str1[i] == '|')
        {
            numOfPipes++;
        }
    }
    return numOfPipes;
}

char** arrayOfCommands(char* str1, int numOfPipes, char **arr){

    int countComm = 0;
    int countChars = 0;
    for (int i = 0; i <= (strlen(str1)); i++)                                                                     //stop condition is the lenght of str1
    {
        if (str1[i] == '|' || str1[i] == '\0')
        {
            countChars = 0;
            countComm++;
        }

        else if (str1[i] != '\n')                                                                                    //the \n (new line) is a part of the input string and we don't want to count it
        {
            arr[countComm][countChars]=str1[i];                                                                      //build array of words by that the number of command is the lines(row)
            countChars++;
        }
    }
    return arr;

}

void execCommands(char *str1, int numOfPipes)
{
    int fd[2];
    pid_t pid;
    int fdd = 0;
    int fdnohup = 0;

    char** command;

     command= (char**)malloc((numOfPipes + 1)*sizeof(char*));                                                       //malloc for array of commands
    for(int i = 0; i < (numOfPipes + 1); i++){
        command[i] = (char*)malloc(strlen(str1)*sizeof(char));
    }
    if(command == NULL){
        perror("Error, memory allocation failed");
        exit(1);
    }
    char** cmd = command;
    arrayOfCommands(str1,numOfPipes, cmd);                                                                      //build array of commands

    cmd[numOfPipes + 1] = NULL;                                                                                           //put null in the end of cmd

    strcpy(history[historyIndex], str1);                                                                       //copy str1 that include pipes to the history

    while (*cmd != NULL) {
        numOfTotalCommands++;                                                                                               //count every command from cmd
        bool isAmpersand = false;                                                                                       //import for bool
        bool isNohup = false;
        int stat;
        pipe(fd);

        char *cmdTrimed = trimWhitespace(*cmd);                                                                     //remove the unnecessary spaces in cmd
        int numOfWordsincmd = countWords(cmdTrimed);

        numOfTotalWords += numOfWordsincmd;

        if(isSourceStrContainsSubStr(cmdTrimed, "nohup "))
        {
            isNohup = true;                            // set flag nohup to true
            replaceSubstring(cmdTrimed, "nohup ", "");  // remove the nohup from the command
        }
        if(cmdTrimed[strlen(cmdTrimed)-1] == '&')
        {
            isAmpersand = true;
            cmdTrimed[strlen(cmdTrimed)-1] = '\0'; // removing the last character
        }

        if ((pid = fork()) == -1) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) // child process
        {
            if(isNohup)                 // if nohup - declare file nohup.txt and rout the output to this file
            {
                fdnohup = open("nohup.txt", O_WRONLY | O_CREAT | O_APPEND,  S_IRUSR | S_IWUSR | S_IRGRP |S_IROTH);
                dup2(fdnohup, 1);
            }
            else
            {
                dup2(fdd, 0);                                                              //not needed for the first son, for the second son fdd = fd[0]
                if (*(cmd + 1) != NULL) {
                    dup2(fd[1], 1);                                                         // change the output for the screen
                }

            }
            close(fd[0]);                                                          // change the output for the screen

            int numOfWordsincmd = countWords(cmdTrimed);

            char **arrrcmd;
            arrrcmd = (char**)malloc((numOfWordsincmd + 2)*sizeof(char*));
            for(int i = 0; i < numOfWordsincmd; i++){
                arrrcmd[i] = (char*)malloc(strlen(str1)*sizeof(char));
            }
            if(arrrcmd == NULL){
                perror("Error, memory allocation failed");
                exit(1);
            }

            arrayOfWord(cmdTrimed, numOfWordsincmd, arrrcmd);

            stat = execvp(arrrcmd[0], arrrcmd);                                         //pointer for the first char in the array and get the all array
            if(stat == -1){
                perror("ERROR");
            }
            for (int indx = 0; indx < numOfWordsincmd; indx++) // free memory allocation - first to each word in array, and after - the hole array
                free(arrrcmd[indx]);
            free(arrrcmd);

            exit(1);
        }
        else // parent process
        {
            if(!isAmpersand)
            {
                waitpid(pid,&stat,0);
            }

            close(fd[1]);                                                                          //close the output
            fdd = fd[0];                                                                               // makes fdd to be input
            cmd++;                                                                                    // one step to the next command in cmd array
        }

    }
    free(command);
}

char* replaceSubstring(char* sourceStr, char* subStr, char* newStr)
{
    char output[256];
    int i = 0, j = 0, flag = 0, start = 0;

    while (sourceStr[i] != '\0')                               // find start index of substring to replace in the source
    {
        if (sourceStr[i] == subStr[j])
        {
            if (!flag)
                start = i;

            j++;
            if (subStr[j] == '\0')
                break;

            flag = 1;                                          // flag == 1 if substring was found in source string
        }
        else
        {
            flag = start = j = 0;                              // substring was not found in source string
        }
        i++;
    }

    if (flag > 0)                                              // if substring was found in the source string
    {
        for (i = 0; i < start; i++)                            // copy the start part - before the substring of the source to the output string
        {
            output[i] = sourceStr[i];
        }

        for (j = 0; j < strlen(newStr); j++)                // copy the new substring to the output instead of the old substring, for example after this loop  output = ls -l
        {
            output[i] = newStr[j];
            i++;
        }


        for (j = start + strlen(subStr); j < strlen(sourceStr); j++) // copy the remaining part of the source string, for example after this loop we get ls -l | !56, add the pipe and the rest str
        {
            output[i] = sourceStr[j];
            i++;
        }
        output[i] = '\0'; // set the end of string
    }

    strcpy(sourceStr, output);  // copy to the input sourceStr for the return
    return sourceStr;
}

void hendler(int number){
    int status;
    waitpid(-1, &status, WNOHANG);
}

bool isSourceStrContainsSubStr(char* sourceStr, char* subStr)
{
    int i = 0, j = 0, flag = 0;

    while (sourceStr[i] != '\0')
    {
        if (sourceStr[i] == subStr[j])
        {
            j++;
            if (subStr[j] == '\0')
                break;

            flag = 1;                                    // flag == 1 if substring was found in source string
        }
        else
        {
            flag = j = 0;                              // substring was not found in source string
        }
        i++;
    }

    if (flag > 0)                                      // if substring was found in the source string
        return true;
    else
        return false;
}

char* parseString(char* str1)
{
    char output[510];  // output string with all !xx replaced to commands
    int countComm = 0;
    int countChars = 0;

    bool found = false; // boolean - set true if !xx found
    char substr[3][20]; // array of all !xx strings for replace with command from history - max 2 pipes - 3 commands
    int j = 0;       // index for substr

    for (int i = 0; i <= (strlen(str1)); i++)    //stop condition is the lenght of str1
    {
        if (str1[i] == '\0')  // stop run loop at end of str1
            break;

        if (str1[i] == '!')  // if ! found - find number - till nex space and replace the number with command
        {
            found = true;
        }
        if(found)
        {
            if(str1[i] == ' ') // end of the !xx, initialize the countChar and ++ to countCommand for the next !xx
            {
                countChars = 0;
                countComm++;
                found = false;
            }
            else
            {
                substr[countComm][countChars] = str1[i];
                countChars++;
            }
        }
    }

    if(countChars > 0) // last !xx not ended with space (at end of command line) - we need to countComm++
        countComm++;

    if(countComm > 0)  // if !xx found in source string
    {
        for(int i = 0; i < countComm; i++)
        {
            char *sub;

            int length = strlen(substr[i]);                                                                          //keep the length of cmd
            sub = malloc(length+1);
            if(sub == NULL){
                perror("Error, memory allocation failed");
                exit(1);
            }

            substring(substr[i], 2, length, sub);                                                        //substring the command - remove the leading !

            int indexInHistory = atoi(sub);                                                                        //convert string to int, indexInHistory is the number of the input
            free(sub);

            if(indexInHistory > historyIndex)
            {
                printf("NOT IN HISTORY\n");
                fflush(stdout);
                continue;                                                                                               // the command is not exists in history array, continue to the next loop
            }
            else
            {
                str1 = replaceSubstring(str1, substr[i], history[indexInHistory]);
            }
        }
    }
    return str1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////