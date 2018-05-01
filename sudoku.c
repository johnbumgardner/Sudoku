/*
  Name:         John Bumgardner
  Class:        ECE 209 - Computer Systems Programming
  Program 3 -   Sudoku
  Grading -     EOS Linux
  Description - This program helps a user play the game of Sudoku. In order to do this, a data structure must be implemented,
                along with several other basic set and get functions. Additionally, the ability to write to a file was added,
                as well as the ability to predict the next value to be chosen to solve the sudoku puzzle
*/

//headers
#include "sudoku.h"
#include "stdlib.h"
#include "stdio.h"
#include "sudoku.h"
#include <assert.h>

/*
  This file describes the interface for a Sudoku Abstract
  Data Type.  

  You may not change the protoype for any function in this file, but
  you should provide a definition for all functions.  Also, you
  may define any additional functions you need within this file. But, they
  can only be used within this file (i.e. you cannot modify main).

  Your implementation will be tested with a different main function than the
  one provided to fully evaluate your entire interface.  Make sure that your
  function definitions work properly even if the main I provided you doesn't test 
  them.
*/

int sizeOfBoard;
struct sudoku_square 
{
  /* HINT: DEFINE THE CONTENTS OF A SINGLE SQUARE. YOU MAY CHANGE THIS */

  /* This struct should hold the value and other information
     needed per square to play the game or solve the puzzle.
   */

  // value
  char value;
  // value at beginning of play (e.g. loaded from file)
  int original_value;

  // possible solution : initialize array to true, clear an entry when you determine
  // it to be impossible
  int possible[10];
};

struct sudoku 
{  
  /* DEFINE YOUR SUDOKU STRUCT HERE */
	struct sudoku_square game_board[81]; //declare a game board with 81 squares - assuming a 9x9 board  
  /*
    This struct should contain everything you need
    to represent your Sudoku game. See the spec sheet
    for hints about setting this up.
   */
};

/* typedef is already provided by sudoku.h, do not add one here */

struct sudoku game;

/*======================================================================*/
/*======================================================================*/


/* DEFINE EXTRA FUNCTIONS IN HERE */
int checkContradictionInRow(char row, char value)
{
  /*
  This function checks to see if any of the same values occur in the file in the same row
  To do the row is held the same, and the collumn is altered
  If a contradiction is found, return SUDOKU_CONTRADICTION
  otherwise, return SUDOKU_SUCCESS
  */

	int col; //declare the collumn to be altered
	for(col=1; col<=9; col++)
  {
      char get; //declare a variable to store the value found at the other squares in row
      //check to make sure the value here exists, and store the value here into get
      if (Sudoku_get_square(row, col, &get) == SUDOKU_SUCCESS) 
      {
        if (get == value && get != '-') //if the value equals any of the values in the row, return SUDOKU_CONTRADICTION, unless there is a '-' because those contradictions are legal
        {
          return SUDOKU_CONTRADICTION;
        }
      }
  }
  //if no contradiction was found, return sudoku success
  return SUDOKU_SUCCESS;
}
int checkContradictionInCol(int col, char value)
{
  /*
  This function checks to see if any of the same values occur in the file in the same col
  To do the col is held the same, and the row is altered
  If a contradiction is found, return SUDOKU_CONTRADICTION
  otherwise, return SUDOKU_SUCCESS
  */
	char row; //declare the row to be altered
	for(row='A'; row<'A'+9; row++)
  {
    char get; //declare a variable to store the value found at the other squares in col
    //check to make sure the value here exists, and store the value here into get
    if (Sudoku_get_square(row, col, &get) == SUDOKU_SUCCESS)
    {
      if (get == value && get != '-') //if the value equals any of the values in the col, return SUDOKU_CONTRADICTION, unless there is a '-' because those contradictions are legal
      {
        return SUDOKU_CONTRADICTION;
      }
    }
  }
  //if no contradiction was found, return sudoku success
  return SUDOKU_SUCCESS;
}
int checkContradictionInSquare(char row, int col, char value)
{
	//adjust to find the square that will be checked
	char r;
	switch(row)
	{
		case 'A':
		case 'B':
		case 'C':
			r = 'A';
			break;
		case 'D':
		case 'E':
		case 'F':
			r = 'D';
			break;
		case 'G':
		case 'H':
		case 'I':
			r = 'G';
			break;
	}
	int c;
	switch(col)
	{
		case 1:
		case 2:
		case 3:
			c = 1;
			break;
		case 4:
		case 5:
		case 6:
			c = 4;
			break;
		case 7:
		case 8:
		case 9:
			c = 7;
			break;
	}

  //the r and c variable store the coordinates of the upper left most part of the square we are checking.
	char startR = r; //declare and initalize the start positions, and go three each way from there
	int startC = c;
	for(;r < startR + 3; r++)
  {
    for (; c < startC + 3; c++)
    {
      char get;
      if (Sudoku_get_square(r, c, &get) == SUDOKU_SUCCESS) //make sure the value is legal/exists
      {
        if (get == value && get != '-') //return contradiction if match is found unless it's a '-'
				{
          return SUDOKU_CONTRADICTION;
				}          		
			}
    }
   }
	return SUDOKU_SUCCESS;
}
// Convert row and col into a linear index between 0 and 81
static int index(char row, int col)
{
  return (row-'A')*9 + (col-1);
}


/*======================================================================*/
/*======================================================================*/

/* IMPLEMENT SUDOKU INTERFACE */

/* 
   Sudoku_initialize: Initialize any global variables or other
      data that you need.  This function should be called first,
      before any other function.
   
   Return values:
      SUDOKU_SUCCESS: initialize succeeded.
      SUDOKU_UNKNOWN_ERROR: there was a problem.
 */
int Sudoku_initialize (void)
{
 	sizeOfBoard = 81; //set size of board to 81
	int i; //will serve as index for array of the board
	for(i = 0; i < sizeOfBoard; ++i)
	{
		game.game_board[i].value = '-'; //initialize the game board values all to '-'
    //set all the values that could possibly occur to possible - as the board is empty, therefore all values are possible
		int possibleValues;		
		for(possibleValues = 0; possibleValues < 10; possibleValues++)
		{
			game.game_board[i].possible[possibleValues] = 1; //set all to true because they can all possibly exist
		}
	}
  	return SUDOKU_SUCCESS;
}

/*
  Sudoku_loadfile: Initialize the game to the board held in filename. The
  current game, if any, is discarded.

  The structure of the file must be as follows:
    - A digit represents a value from 1-9 and a - represents an empty game position.
    - There will be 9 lines in the file. Each line corresponds to one
      row of the Sudoku puzzle
    - Each line contains 9 digits/dashes. Each digit/dash falls into one of the 9 columns. 
    - Read in the file and populate your structure.

  Return the following error codes:
     SUDOKU_SUCCESS : file read successfully
     SUDOKU_NO_FILE_ERROR : no file found or cannot open file
     SUDOKU_BAD_FORMAT : file contents in invalid format:
           - fewer than 9 digits/dashes on a line
           - fewer than 9 lines in the file
	   - unknown character
     SUDOKU_UNKNOWN_ERROR : any other error you think of
*/
int Sudoku_loadfile(const char *filename)
{
  //open the file
  FILE *file = fopen(filename,"r");
  if (file==NULL) //if the file cannot be found, return an error
  {
    return SUDOKU_NO_FILE_ERROR;
  }
  //declare variables to look through the input file and to store into the data structure
  int row;
  int col;

  for(row=0; row<9; row++) //control the row
  {
    for(col=1; col<=9; col++) //control the column
    {
	   char val; //declare variable to get the value
	   if(fscanf(file,"%c", &val)!=EOF) //check to see if the end of the file was reached and read in the value into val
	   {
		  if((val >= '1' && val <= '9') || val == '-') //if the val received is legal, set the square to that
		  {	  
	      Sudoku_set_square((char)(row+'A'),col,val);
		  }
		  else //in the event the value received was illegal - close the file and return an error
		  {
			 fclose(file);
			 return SUDOKU_BAD_FORMAT;
		  }
	   }
	   else //if the end of the file was reached, return an error and close the file
	   {
		  fclose(file);
		  return SUDOKU_BAD_FORMAT;
	   }
	 }
  // eat newline
  fscanf(file,"%*c");
  }
  //the end of the file was reached - close the file
  fclose(file);
  //return SUDOKU_SUCCESS because the file was read in successfully
  return SUDOKU_SUCCESS;
}


/*
  Sudoku_savefile: Save the current game to filename. 

  The structure of the file must be compatible with Sudoku_loadfile.

  Return the following error codes:
     SUDOKU_SUCCESS : file read successfully
     SUDOKU_NO_FILE_ERROR : no file found or cannot open file
     SUDOKU_UNKNOWN_ERROR : any other error
 */

int Sudoku_savefile(const char *filename)
{
  FILE *file = fopen(filename,"w"); //open the file to write into it
  if (file == NULL) //check to see if the file can be found
  {
      return SUDOKU_NO_FILE_ERROR; //if not, return the 
  }
  //initialize and declare the variables needed to loop through the sudoku board
  char row = 'A';
  int col = 1;
  char value; //declare variable to get the value of the char in the square

  //loop throught the sudoku structure
	for(; row <= 'I'; ++row )
	{
		for(col = 1; col <= 9; ++col)
		{
      int returnCode = Sudoku_get_square(row, col, &value); //get the value of the square into value
			if(returnCode == SUDOKU_SUCCESS) //check to make sure we successfully got a value
       {
          fprintf(file, "%c", value); //put the value into the file  
       }
		}
		fprintf(file, "%c", '\n'); //place new line in the file
	}
  fclose(file);
	return SUDOKU_SUCCESS;
  
}

/*
  Sudoku_set_square: set the square at position (col, row) to value
  digit. 

  Return values:
     SUDOKU_SUCCESS : square set successfully
     SUDOKU_CONTRADICTION : square set but it leads to a contradiction.
        - You do not need to solve the puzzle, just look at current values
	  and see if a contradiction already exists.
     SUDOKU_ILLEGAL_VALUE : digit is not legal.
     SUDOKU_ILLEGAL_INDEX : col and row do not forma legal index
 */
int Sudoku_set_square(char row, int col, char digit)
{
	//check to make sure the index is allowed
	if(index(row, col) >= 81 || index(row, col) < 0)
	{
		return SUDOKU_ILLEGAL_INDEX;
	}
  //make sure the value you are reading in is legal
	if(!(digit <= '9' && digit >= '1') && digit != '-')
	{
		return SUDOKU_ILLEGAL_VALUE;
	}
	//check the various contradictions
	int val = checkContradictionInRow(row, digit);
	if( val == SUDOKU_CONTRADICTION)
	{
		return val;
	}
	val = checkContradictionInCol(col, digit);
	if(val == SUDOKU_CONTRADICTION)
	{
		return val;
	}
	val = checkContradictionInSquare(row, col, digit);
	if(val == SUDOKU_CONTRADICTION)
	{
		return val;
	}
	//set the data structure and return sudoku success
	game.game_board[index(row, col)].value = digit;
	return SUDOKU_SUCCESS;
		
	
		
  	
}

/*
  Sudoku_get_square: return the square at position (col, row) by reference in *value.

  Return values:
     SUDOKU_SUCCESS : value is returned successfully in *value
     SUDOKU_ILLEGAL_INDEX : col and row do not form a legal index
 */
int Sudoku_get_square (char row, int col, char *value)
{
  	// REMOVE THIS PRINTF
  	/*printf("You need to implement Sudoku_get_square()\n");*/

  	if(!(row >= 'A' && row <= 'I' && col >= 1 && col <= 9))
	{
		return SUDOKU_ILLEGAL_INDEX;
	}
	else
	{
		*value = game.game_board[index(row, col)].value;
	}
	return SUDOKU_SUCCESS;
}

/*
  Sudoku_give_hint: return a hint by value.  The col, row, and value are returned
     by reference in *col, *row, and *value.

  Return value:
     SUDOKU_SUCCESS : hint supplied
     SUDOKU_NO_HINT : no hint because solver could not find one.
*/
int Sudoku_give_hint (char *r, int *c, char *value)
{
  //all values are possible at the start of the function.
  //determine what values are impossible at the start of every call.

  //loop through the entire game board - ruling out possible values based on other values in the row and col
  //declare the values to control the loop
  char row;
  int col;
  //loop through the rows
  for(row = 'A'; row <='I'; row++)
  {
    //loop through the collumns
    for(col = 1; col <= 9; col++)
    {
      char currVal; //declare variable to get the value we are currently checking the other values with
      if(Sudoku_get_square(row, col, &currVal) == SUDOKU_SUCCESS) //if a value is found here
      {
        if(currVal == '-') //if value is presently unknown
        {
          // update the array that stores possible values based on the values stored in the current row - alter col
          int changeCol;
          for(changeCol = 1; changeCol <= 9; changeCol++)
          {
            char valueAsColChanges;
            if(Sudoku_get_square(row, changeCol, &valueAsColChanges) == SUDOKU_SUCCESS) //makes sure valid char is read
            {
              if(valueAsColChanges != '-') //makes sure we have a value here
              {
                game.game_board[index(row, col)].possible[valueAsColChanges - 48] = 0; //subtract 48 to get the actual number value
              }
            }
          }
          //do the same algorithm but alter the row instead
          char changeRow;
          for(changeRow = 'A'; changeRow <= 'I'; changeRow++)
          {
            char valueAsRowChanges; 
            if(Sudoku_get_square(changeRow, col, &valueAsRowChanges) == SUDOKU_SUCCESS) //makes sure valid char is read
            {
              if(valueAsRowChanges != '-') //makes sure we have a value here
              {
                game.game_board[index(row, col)].possible[valueAsRowChanges - 48] = 0; //subtract 48 to get the actual number value
              }
            }

          }
        //do the same but for the surrounding square
        //adjust to find the square that will be checked
        char adjustedR;
        switch(row)
        {
          case 'A':
          case 'B':
          case 'C':
            adjustedR = 'A';
            break;
          case 'D':
          case 'E':
          case 'F':
            adjustedR = 'D';
            break;
          case 'G':
          case 'H':
          case 'I':
            adjustedR = 'G';
            break;
        }
        int adjustedC;
        switch(col)
        {
          case 1:
          case 2:
          case 3:
            adjustedC = 1;
            break;
          case 4:
          case 5:
          case 6:
            adjustedC = 4;
            break;
          case 7:
          case 8:
          case 9:
            adjustedC = 7;
            break;
          }

          //the adjustedR and adjustedC variable store the coordinates of the upper left most part of the square we are checking.
          char startR = adjustedR;
          int startC = adjustedC;
          //follow a similar process as the check contradiction in square
          for(;adjustedR < startR + 3; adjustedR++) //loop through rows nearby
          {
            for (; adjustedC < startC + 3; adjustedC++) //loop through columns nearby
            {
              char get;
              if (Sudoku_get_square(adjustedR, adjustedC, &get) == SUDOKU_SUCCESS) //read the value at each addreess
              {
                if (get != '-')
                {
                  game.game_board[index(row, col)].possible[get - 48] = 0; //subtract 48 to get the actual number value
                }             
              }
            }
          }
        }
      }
    }
  }
  /*
  every array has been properly set to rule out impossible values
  now loop through the entire data structure to find where there
  is a a square that only hase one possible value
  -return SUDOKU_SUCCESS
  -pass the row, col, and value that work here by reference
  */
  for(row = 'A'; row <='I'; row++)
  {
    for(col = 1; col <= 9; col++)
    {
      int statusValue = 0; 
      /*loop through the possible values array.
        if there is a one - that value is possible
          add one to the status value for every one found in the array
        after summing all the contents of the array, if the status value is one
        , there is only one possible values, therefore we have a solution, return 
        SUDOKU_SUCCESS and set the row and col and value
        */
      int i; //index variable
      for(i = 1; i < 10; i++)
      {
        statusValue += game.game_board[index(row, col)].possible[i]; //add to the statusValue whatever is in the array
      }
      if(statusValue == 1)
      {
        //pass the values by reference of the location in the array
        *r = row; //pass the row
        *c = col; //pass the collumn
        //pass the value found that will work
        for(i = 1; i < 10; i++) //loop back through array
        {
          if(game.game_board[index(row, col)].possible[i] == 1) //for the one that equals one
          {
            *value = game.game_board[index(row, col)].value; //pass the value by address

          }
          //return the success message
          return SUDOKU_SUCCESS; 
          
        }

      }

    }
  }
  
  //if no value was found, no hint can be returned
  return SUDOKU_NO_HINT;
}

/*
  Print the Sudoku game to the screen.  Your code must work with this 
  function as implemented.

  DO NOT MODIFY
 */
void Sudoku_print(void)
{
  char row = 'A';
  int col;
  char value;

  // Print the board: DO NOT MODIFY

  printf("   | 1  2  3 | 4  5  6 | 7  8  9 |\n");
  for(row='A'; row <= 'I'; row++)
    {
      if ((row-'A') % 3 == 0)
      printf("   |---------+---------+---------|\n");

      printf(" %c |",row);
      col = 1;

      for(; col<=3; col++)
	{
	  Sudoku_get_square(row,col,&value);
	  printf(" %c ",value);
	}
      printf("|");
      for(; col<=6; col++)
	{
	  Sudoku_get_square(row,col,&value);
	  printf(" %c ",value);
	}
      printf("|");
      for(; col<=9; col++)
	{
	  Sudoku_get_square(row,col,&value);
	  printf(" %c ",value);
	}
      printf("|\n");
    }
    printf("   |---------+---------+---------|\n");
}