
Shell simulator with pipes ampresent and nohup
Authored by sagi ben shushan
209351147

==Description for ex3 ==

The program open terminal for the user and get input command, if the command exsist in the shall we make this command work(by execvp).
If the command is not exsist in the shall we print for the user "this command is not exist".
If the command is "cd" we print for the user "command not supported (Yet)".
The program end by reciving the string "done", when we get from the user the "done" command we print the numbers of words and commands in current run(we using counters for this), and we update the history file.
When the command is "history" we display for user all the commands that saved in file(exept to "cd" and "done").
I used the general structure of ex2 for ex3, for exmple opening a file for read, filling an array for a history file, a main loop with reciving input from the user which checks the input for specific words, using methods to check number of words.
In the end i used to open a file to write the same commands that recived(pass from the array to the file).


functions from ex2:

main: call the other functuins, check if the the string is compare to one of the words like cd,history,done...
check if exsist pipes in str and count them.

countWords: count the number of words of the input string.

arrayOfWords: malloc for the array that we keep there the seperate words,build this array.

substring :copy string to string from specific position in the string, remove the leading '!'.

 
Changes and additions to Exercise 3 relative to Exercise 2:
New functions:

getNumOfPipes : Receives input and returns number of pipes

arrayOfCommands : receives the input and returns an array of commands - strings between the pipes

execCommands : A function that execute the commands in a loop - command after command between the pipes, if they are in the input line.
Relative to Exercise 2 - we moved the functionality from the main to a separate function both so that the program would be more readable and also because the functionality is complete and can be activated from several places.

replaceSubstring : the function replace in original string one (old) sub-string with the another ( new) sub-string.
the function input is: the original string, the  old sub-string that needs to be replaced with a new sub-string and the new sub-string   ( for example "!55 | !56" to " ls -l | wc -c".)

isSourceStrContainsSubStr :  gets an original string and a sub-string, checks if an original string contains the sub-string, and returns true if the original string is contains the sub-string and false if not.

parseString -:receives the string of the input, in a loop to the end of the input string look for the exclamation marks (!) and if find them - write the sub-string from the exclamation mark to the first space to a local array, and advance the counter of the array.
 When we went through all the input we were left with the array of strings. If the counter is greater than zero - it means the input includes the !xx that needs to be replaced with commands from the history array by calling the replaceSubstring function.

Ampersand (&) : If we have detected that we have an ampersand in the input -
1. Set a "flag" that we have & in the command and delete the mark itself from the command so as not to impair the execution of the command
2. Do not run waitpid (pid, & stat, 0);

Nohup - if we detected "nohup" in the command line -
1. Set a "flag" that we have the nohup in the command and delete the mark itself from the command so as not to impair the execution of the command
2. open file nohup.txt and redirect the out to the fd of the file (fdnohup)



==Program Files==
ex3.c 

==How to compile ex2a?==
compile: gcc ex3.c -o ex3
run: ./ex3


==Input:==
a command from the user

==Output:==
execution of the command if exsist in the shell(except command like cd, history and done)






