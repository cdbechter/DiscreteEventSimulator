/* This is a simple read file program
    Professor Salvatore helped us with this is 2107 last semester,
    so my tweaking it a little bit from databases to a simple config file,
    it will read in our config file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getNumColumns(char*, char);
ssize_t readline(char **, FILE *);
char* getValue(char*, char*, int, int);


/**********************************************************************
  Counts the number of columns in the config.

  Input arguments:
      char* line - The row of data to use for counting
      char delimiter - The delimiter used in data to separate columns

  Return value:
      columns - the amount of columns in the config
**********************************************************************/
int getNumColumns(char* line, char delimiter) {
	int columns = 0;
	if(line == NULL) {
	    return columns;
	}
	columns++;

	while(*line) {
		columns += (*(line++) == delimiter); //if char in line equals the delimiter it will add 1, otherwise it'll add nothing 
	}
	return columns;
}

/**********************************************************************
  This is just a wrapper for the getline library function. The getline
  function on its own does not remove trailing newline characters.
  Because we wish to examine each line of the database file without
  the newline character, we must take an extra step in order to
  do this. This also ignores the size of the buffer len = 0.

  Input arguments:
      char ** lineptr - Pointer to the c string where the line should
                        be stored when it is read
      FILE * stream - The file from which the line should be read

  Return value:
      chars - how many chars to return
**********************************************************************/
ssize_t readline(char **lineptr, FILE *stream) {
    size_t len = 0;

    ssize_t chars = getline(lineptr, &len, stream);

    if((*lineptr)[chars-1] == '\n') {
        (*lineptr)[chars-1] = '\0';
        --chars;
    }
    return chars;
}

/**********************************************************************
  Gets the value stored in a particular column of a particular
  file (config) based on a search value and the column where it will
  be found if it exists.

  Input arguments:
      char* file - The name of config file (filename)
      char* searchStr - The string to search for within the config
      int colSearch - The column in which to look for searchStr
      int colResult - If searchStr is found, the column of that row
                      from which to retrive the return value

  Return value:
      NULL - If there was a problem or if searchStr could not be found
      char ** - Returns an array of strings, containing any value
                found in colResult of the row in which searchStr was
                found
**********************************************************************/
char* getValue(char* file, char* searchStr, int colSearch, int colResult) {
	FILE* fp;
	char *line = NULL, *temp = NULL;
	size_t len = 0;
	ssize_t chars;
	int numCols = 0, currCol = 0, searchFound = 0;

	/* file open: error if no file */
	if((fp = fopen(file, "r")) == NULL) {
		printf("ERROR opening file \"%s\"\n", file );
		return NULL;
	}

    /* simple if statement to make sure there are line in the config file for 
        get columns to read, otherwise theres a problem */
	if((chars = readline(&line, fp)) == -1 || (numCols = getNumColumns(line, ' ')) == 0) {
		return NULL;
	}
	/* this makes sure that colSearch and colResult are within a certain range, this range is: (0, numCols) */
	if(numCols <= colSearch || numCols <= colResult) {
		printf("Error: Search or Result column index does not exist.\n");
	}
	do {
		/* very important to allocate memory for the line */
		char * tempLine = (char *) malloc((chars + 1) * sizeof(char));
    	strcpy(tempLine, line);

	 	//locate the column to search at and check if it matches 
		while((temp = strsep(&line, " ")) != NULL && !searchFound) {
			//compare strings only if the current column is the column to search 
			if(currCol == colSearch) {
				searchFound = (strcmp(temp, searchStr) == 0);
			}
			currCol++;
		}
		currCol = 0; //reset current column in aid to retrieve result
		
		while((temp = strsep(&tempLine, " "))!= NULL && searchFound) {
			if(currCol == colResult) {
				return temp;	 
			}
			currCol++;
		}
	} while((chars = readline(&line, fp) != -1)); //read new line of file
	return NULL;
}

/*
char* getValue(char* name, char* str){
	int spce = 0;
	char line[30];
	char * output;
	FILE *fp = fopen(name, "r+");
	if(!fp){
		printf("File not found.\n");
		return NULL;
	} else {
		while ((fgets(line, 30, fp)) != NULL){
			if(strstr(line, str)) {
				char* temp = strtok(line, " ");
				temp = strtok(NULL, line);
				output = (char *) malloc((strlen(temp) + 1) * sizeof(char));
				strcpy(output, temp);
			}
		}
	}
	return output;
} */