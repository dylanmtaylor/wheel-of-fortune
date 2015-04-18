/*
    **       ** **      ** ******** ******** **           *******   ********
    **       ** **      ** **       **       **          **     **  **
    **   *   ** **      ** **       **       **         **       ** **
    **  ***  ** ********** *******  *******  **         **       ** *******
    ** ** ** ** **      ** **       **       **         **       ** **
    ****   **** **      ** **       **       **          **     **  **
    **       ** **      ** ******** ******** ********     *******   **

    ********   *******   *******   ********** **     ** ****     ** ********
    **        **     **  **    **      **     **     ** ** **    ** **
    **       **       ** **    **      **     **     ** **  **   ** **
    *******  **       ** *******       **     **     ** **   **  ** *******
    **       **       ** **   **       **     **     ** **    ** ** **
    **        **     **  **    **      **     **     ** **     **** **
    **         *******   **     **     **      *******  **      *** ********

	THIS PROGRAM CODED BY DYLAN TAYLOR  ///////  PERIOD 6 C++ FEBRUARY, 2010
*/

/*** NOTES ABOUT COMPILING:
In order to get this code to compile properly, it is necessary to change some project settings in Visual C++.
In Visual Studio 2008, this is RIDICULOUSLY easy, all you have to do is go to the project settings, choose General under
Configuration Properties, and change "Character Set" from "Use Multi Byte Character Set" to "Use Unicode Character Set".
----------------------------------------------------------------------------------------------------------------------
In Visual Studio 6 (Unfortunately what this needs to work in), it's MUCH more annoying to set up this project. First,
you need to open the project settings page (Alt+F7). In here, you need to go to the C++ tab, and in the preprocessor definitions
box, you have to remove _MBCS from the list if it's there, and add UNICODE,_UNICODE to the list. Then, you have to go to the
Link tab, choose the Output category, and in the box called "Entry-point Symbol", you need to enter "wWinMainCRTStartup".
(Special Thanks to http://www.mihai-nita.net/article.php?artID=20060723a for this information)...
***/
//Unicode info: http://www.joelonsoftware.com/articles/Unicode.html
#include <iostream>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <dos.h>
#include <tchar.h>
#include <malloc.h>
HANDLE handle= GetStdHandle(STD_OUTPUT_HANDLE);
#else
#include <unistd.h>
#include <curses.h>
#endif
using namespace std;
//Modifiable Settings
char boardcolor = 'B'; //the color of the board's border
char textcolor = 'F'; //the color of the text on the board
char c1color = 'C'; //contestant 1's name color
char c2color = 'E'; //contestant 2's name color
char c3color = '9'; //contestant 3's name color
char moneycolor = 'A';
bool guessed[26] = {false}; // prevents the same letter from being guessed twice, and makes sure all letters can be guessed.
char lguessed[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'}; //contains the actual letters that can be guessed.
bool fscreen = false; //whether or not this program starts and runs in full-screen mode
bool animation = false; //set this to true in order to turn the animations on. disabled to save time testing.
bool skipsplash = false; //whether or not to skip the splash screen
bool skipsetup = false; //if this is true, all the pre-game setup prompts will be skipped and set to defaults. NOTE: This will skip everything in the setup EXCEPT for entering the Player Names.
bool debug = true; //used for checking certain code. If on it displays certain additional info on the screen. WARNING: If this is on, A LOT more information is shown on the screen.
bool cheat = false; //used to display the solution to the puzzle on the board instead of blank spaces... :P Also "suggests" phrases to guess.
bool setupfreeze = false; //whether or not to freeze the program after the setup screen to display debugging information on the screen.
//Variables, Arrays, and Method Headers
char square[52]; //These are the spaces on the wheel of fortune board.
//These are the squares after the board is solved. Filled up with blank spaces by default.
char solved[52] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
bool revealed[52] = {false}; //whether each character on the board has been solved
char phrase[1000]; //This contains the ORIGINAL phrase as entered during setup. It will be similar to solved, but with the characters being in different locations. The array is so large in order to accommodate additional characters.
int srows; //The number of rows of text on the screen.
int c = 0; //Making this global makes coding a _lot_ easier, even though it is only needed by 2 methods.
int lines = 1; //number of lines used by the phrase in the board out of 4 -- no matter what, at least one line will be used.
int wordcount = 0; //how many words are in the phrase
int corrls; //correct letters in a guess
int phraselen; //the length of the whole phrase
int startpos; //the position to start writing data to the solved grid.
int curword = 0; //current word, used for processing strings into words.
string words[5]; //contains the actual words used in the game
int catsize = 0; //Used for checking the size of the category entered since .size() does not work with character arrays, and sizeof returns 50 no matter what.
char category[50]; //The category of the phrase. This can be up to 50 characters. (previously 71)
int wordlen[5]; //The lengths of each word.
int wordlines[5]; //contains the line number of each word.
int offset = 0; //used for making the words fit on the lines.
bool sameline = false; //whether the data is being written to the same line as last time
bool added[5] = {false}; //whether or not each word in the phrase was added to a line yet
bool fits = true; //whether or not the data fits on to all 4 lines
bool doesntfit = false; //whether or not the data on the current line does not fit within the 12 character limit
bool newline = true; //whether or not we just got to a new line... if this is the case, we don't need to add a space before the word.
bool evenborder[4] = {false}; //whether or not it is possible to perfectly center the text horizontally on the line.
string curdata = ""; //the current data that will be added to the line. This is needed because we center the line after the phrase is entered.
char linedata[5][12]; //this is slightly different than the actual data on the lines. This data is not vertically centered on the board. That will be handled later in the program, and the data is centered on the actual board. The 5 instead of 4 for the character array fixes an annoying bug in an easy manner, as time restrictions didn't allow for a proper fix.
int linechars[5]; //The number of characters used on each line
char players[3][50]; //The names of the players.
char selection[9999]; //whether the player wants to guess -- 9999 characters to handle invalid input
int cll; //length of current line -- used to center text horizontally.
int player; //the number of the current player
int money[3]; //the amount of money each player has
int phrlen = 0; //phrase length, used to fix a bug
int curvalue; //the amount of money awarded for guessing correctly. spinning the wheel sets this value.
bool gameover = false; //whether or not the game is over
bool chosen = false;//whether or not they chose to guess a letter or the phrase
void setup(); //Sets up certain variables before initial execution.
void splash(); //The animation that is displayed at the beginning of the program.
void board(); //Draws the wheel of fortune board.
void color(char); //Used to change the color of the console text. Unlike System(color _), this lets you have more than one color on the screen at once.
void cls(); //Used as an alternative of the System "cls" function because cls is too slow to quickly refresh the screen.
bool cr(int,int,int); //Checks if value is within the range of min and max.
int scmp(string, string); //case sensitive string comparison. Returns: 0 if equal. -1: 1st string less in value; 1: 1st string greater in value.
void cursor(int, int); //Moves the "cursor" to specified location in the console. This is in the format of cursor (column, row). (0,0) is the top left of the screen.
void delay(int); //Used to delay the program by x microseconds.
int rand(int, int); //Random number generation function, syntax is min, max
void guessaletter();
void spin(); //Spins the wheel. Generates values between 100 and 1000 dollars.
void fullscreen(); //Forces the program into full-screen mode
void lettersguessed(); //shows the letters guessed
void letter(); //Lets the contestant guess a letter
void wphrase(); //Let the contestant guess the whole phrase
void complete(); //Checks if the puzzle is complete.
void turnover(); //when a players turn is over -- switches contestants.
void completed(); //Once we know the puzzle is complete, determine who won.

int main() {
    setup();
    fullscreen();
    if (!skipsplash) {
        splash();
    }
    spin();
    board();

    while (true) {
        //do nothing
    }
    return 0;
}

void setup() {
    #ifdef _WIN32
    //system("chcp 65001"); //runs chcp in the console to change the code page
    //SetConsoleOutputCP(65001); //Changes the code page from the default of 437 to 65001 (Unicode). This allows native displaying of Unicode characters.
    #endif
    //Start by setting up the blank spaces in the board. There needs to be exactly 52.
    //Now we have to determine whether or not this will be running in full-screen and set a variable for the number of screen lines.
    if (fscreen == false) {
        srows = 25;
    } else {
        srows = 50;
    }
    cls();
    gameover = false; //just in case
    //Finally, we need to determine the phrase that we will use
    cout<<"Wheel of Fortune Game Setup"<<endl;
    cout<<"==========================="<<endl;
    //cout<<endl;
    string phrs; //temporary string used for the phrase
    bool pds = true; //phrase contains double spaces -- this prevents improper input in some cases
    while ((lines < 1 || lines > 4) || (wordcount < 3 || wordcount > 5)) { //Min lines 1, max lines 4, min words 3, max words 5.
        curword = 0;
        pds = true;
        cout<<endl<<"Enter the player's names below. Each name may be up to 50 characters."<<endl<<endl;
        for (int setupplayers = 0; setupplayers < 3; setupplayers++) {
            cout<<"Enter Contestant "<<(setupplayers + 1)<<"'s Name: ";
            cin.getline(players[setupplayers], 50, '\n');
        }
        //choose a random player to start to make it fair.
        player = (rand(0,2));
        if (!skipsetup) { //have them type in the phrase...
            cout<<endl;
            cout<<"The phrase can be from 20 to 50 characters. Each word must be 12 characters or less. You can have from 3 to 5 words. The phrase must fit within the line limits."<<endl;
            cout<<"You may not have more than 12 characters on a single line. The category name can be from 5 to 50 characters."<<endl<<endl;
            //Before we can enter the phrase, we need to clear all previously saved data in the phrase.
            for (int resetphrase = 0; resetphrase <= 52; resetphrase++) { //'\0' represents an empty character.
                phrase[resetphrase] = '\0';
            }
            cout<<"Enter the Phrase: ";
            cin.getline(phrase, 52, '\n');
        } else { //Have the phrase set to default
            cout<<"The program was set to skip the setup questions at compile time -- using defaults."<<endl;
            string defaultphr = "Wheel Of Fortune"; //The default phrase
            string defaultcategory = "Popular Game Shows"; //The default string
            for (int setphrase = 0; setphrase < sizeof(phrase); setphrase++) {
                phrase[setphrase] = defaultphr[setphrase];
            }
            for (int setcategory = 0; setcategory < sizeof(category); setcategory++) {
                category[setcategory] = defaultcategory[setcategory];
            }
        }
        //Before counting the words, we MUST remove all double spaces. This prevents counting a word twice.
        //The following starts searching for two spaces in a row starting at position 0 (left to right, as opposed to rfind)
        phrs = phrase; //Converts the character array phrase to a string. This makes this kind of modification easier.
        while (phrs.find("  ",0) != string::npos) { //while there are more than one space in a row; npos is returned if the substring "  " is not found.
            int dbls = phrs.find("  ",0); //finds the exact starting position of the double spaces
            phrs.erase(dbls,1); //erase one of the spaces starting from the start of the double spaces
        }
        //Now to convert the string _back_ into a character array for further processing
        for (int a = 0; a <= phrs.size(); a++) {
            phrase[a] = phrs[a];
            //Next, since in wheel of fortune, all characters on the board are uppercase, we need to convert all characters to upper case
            phrase[a] = (char)toupper(phrase[a]);
        }
        //Now we need to calculate how many words there are...
        wordcount = 0;
        if ((phrase[0] != '\0') && (phrase[0] != ' ')) { //if the first character is not empty, and it is a space
            wordcount++;
        }
        for (int cw = 1; cw < sizeof(phrase); cw++) { //cw is short for count words...
            if (phrase[cw] == ' ') { //if the character in the phrase is a space
                if ((phrase[(cw - 1)] != ' ') && (cw != 52) && (phrase[(cw+1)] != '\0')) { //if the previous character wasn't a space (prevents double spaces from counting) and if we are not on the last character in the phrase and if the next character in the phrase is not empty
                    wordcount++; //increment the word counter by 1
                }
            }
        }
        //Now, for debugging purposes, we are going to check that the string was properly converted to a character array, and it survived this far...
        //Now that we know how many words there are, we have to split the phrase into an array of words
        //String tokens are really awesome... :D In this line, we take phrase and separate it by spaces
        //Strtok Reference: http://www.cplusplus.com/reference/clibrary/cstring/strtok/
        char *word = strtok(phrase, " ");
        while (word) { //This works because strtok returns NULL only if there are no more tokens in the string.
            words[curword] = word;
            cout<<"Word "<<(curword + 1)<<": "<<words[curword]<<" ["<<words[curword].size()<<" Characters]"<<endl;
            //Now that we have the phrase split into words, we need to figure out the length of each word and put them in an array
            wordlen[curword] = words[curword].size();
            curword++;
            word = strtok(NULL, " ");
        }
        if ((wordcount >= 3) && (wordcount <= 5)) { //Let's check to see if we have at least 3 words...
            if (debug) {
                cout<<"Total Words: "<<wordcount<<endl;
                //cout<<"Beginning line count..."<<endl;
            }
            //Next, we need to figure out how many lines are necessary to display the phrase (max line size 12), and what line each word will be on.
            for (int cl = 1; cl <= wordcount; cl++) {
                if (debug) {
                    //cout<<"Current Loop: "<<cl<<endl;
                }
                while (added[(cl - 1)] == false) { //While we still haven't added the word
                    if (fits) { //if the phrase fits on the lines so far, continue adding words to the lines
                        if (debug) { //If we have debug on, and the lines are so far within the limits
                            //cout<<"Current Number of Lines Needed: "<<lines<<endl; //display how many lines we need
                        } //end debug
                        if (newline) { //If it _IS_ a new line
                            if (debug) { //If we have debug on, display misc. info about calculations
                                /*     cout<<"This is a new line..."<<endl;
                                    cout<<"Current Line Space Used: "<<curdata.size()<<endl;
                                    cout<<"Additional Space Needed: "<<(words[(cl - 1)].size())<<endl;
                                    cout<<"Text to be added: \""<<words[(cl - 1)]<<"\""<<endl; */
                            }
                            if (curdata.size() + (words[(cl - 1)].size()) <= 12) { //if we can fit the next word on the line
                                if (debug) {
                                    //cout<<"\""<<words[(cl - 1)]<<"\""<<" fits on the line"<<endl;
                                }
                                curdata = words[(cl - 1)];
                                added[(cl - 1)] = true;
                                newline = false;
                                if (cl  == wordcount) { //If we are on the last word and it fits on the current line
                                    //Write the data to the line
                                    for (int z = 0; z < 12; z++) {
                                        if (debug) { /*
                                                cout<<"Writing Last Line to Array...";
                                                cout<<"Line: "<<(lines - 1)<<endl; */
                                        }
                                        linedata[(lines - 1)][z] = curdata[z]; //Write the line info to the linedata array.
                                    }
                                    break; //break out of the loop -- we are DONE.
                                }
                            } else { //If it IS a new line, yet we can't fit a single word on the line we seriously have a problem... :(
                                added[(cl - 1)] = false; //mark it down that the word was NOT added
                                //Adding another line won't do anything, as the word won't fit on that either.
                                break; //break out of the loop, this situation is utterly hopeless
                            }
                        } else { //if it's a _NOT_ new line, we don't need a space before the word...
                            if (debug) { /*
                                    cout<<"This is NOT a new line..."<<endl;
                                    cout<<"Current Line Space Used: "<<curdata.size()<<endl;
                                    cout<<"Additional Space Needed: "<<(words[(cl - 1)].size() + 1)<<endl;
                                    cout<<"Text to be added: \""<<words[(cl - 1)]<<"\""<<endl; */
                            }
                            if ((curdata.size() + (words[(cl - 1)].size() + 1)) <= 12) { //if we can fit the next word AND a space on the line
                                if (debug) {
                                    //cout<<"\""<<words[(cl - 1)]<<"\""<<" fits on the line"<<endl;
                                }
                                curdata = curdata + " " + words[(cl - 1)]; //since it's not the first word, we need a space here.
                                if (cl  == wordcount) { //If we are on the last word and it fits on the current line
                                    //Write the data to the line
                                    for (int z = 0; z < 12; z++) {
                                        if (debug) { /*
                                                cout<<"Writing Last Line to Array...";
                                                cout<<"Line: "<<(lines - 1)<<endl; */
                                        }
                                        linedata[(lines - 1)][z] = curdata[z]; //Write the line info to the linedata array.
                                    }
                                    break; //break out of the loop -- we are DONE.
                                }
                                added[(cl - 1)] = true;
                                newline = false;
                            } else { // if its not a new line and the word does not fit on the line
                                //First, we have to write the data from the current line...
                                for (int z = 0; z < 12; z++) {
                                    linedata[(lines - 1)][z] = curdata[z]; //Write the line info to the linedata array.
                                    added[(cl - 1)] = true; //mark the word as being added
                                }
                                //Then We need to write the word that didn't fit onto the next line
                                lines++; //increase the number of lines by 1
                                curdata = words[(cl - 1)]; //set curdata to the current word
                                for (int r = 0; r < 12; r++) {
                                    linedata[(lines - 1)][r] = curdata[r]; //Write the line info to the linedata array.
                                    added[(cl - 1)] = true; //mark the word as being added
                                }
                                if (debug) { /*
                                        cout<<"\""<<curdata<<"\""<<" did not fit on the line."<<endl;
                                        cout<<"It was written to a new line."<<endl;
                                        cout<<"Line: "<<(lines - 1)<<endl; */
                                }
                            }
                        }
                    } // end of check to see if it fits
                    if (lines > 4) { //checks if this fits on the board or not...
                        fits = false;
                        cout<<"ERROR!!! The phrase does NOT fit on the lines!"<<endl;
                        break; //Breaks out of the loop -- by the time we get this far, the phrase is already invalid, and that won't improve any time soon...
                    } else { //if there are NOT more than 4 lines needed
                        if (doesntfit) {
                            lines++; //Increment the number of lines by 1
                            curdata = ""; //empty the curdata string...
                            newline = true;
                            doesntfit = false;
                        } else {
                            newline = false;
                        }
                    }
                } //end of while statement
            } // end of for statement
        } else { // If the number of words entered was NOT at least 3
            //invalid input
            cout<<"Invalid Input: Phrases MUST be at least 3 words long, and less than 5 words long."<<endl;
        }
        for (int showlines = 0; showlines < lines; showlines++) {
            linechars[(showlines)] = 0;
            if (debug) {
                cout<<"Line "<<(showlines + 1)<<": \"";
            }
            for (int countline = 0; countline < sizeof(linedata[(showlines - 1)]); countline++) {
                if (linedata[(showlines)][countline] != '\0') {
                    linechars[(showlines)] = linechars[(showlines)] + 1;
                }
            }
            //In order to not have non-existent trailing spaces shown after the actual contents of the line, we need to do this in a seperate for statement.
            for (int displine = 0; displine < sizeof(linedata[(showlines - 1)]); displine++) {
                if ((debug) && (displine < linechars[(showlines)])) {
                    cout<<linedata[(showlines)][displine];
                }

            }
            if (debug) {
                cout<<"\" ["<<linechars[(showlines)]<<" Characters]"<<endl;
            }
        }
        //It seems kinda odd to do this so late in the setup process, but we're now going to ask for the Category.
        if (!skipsetup) { //if we are NOT skipping the setup process...
            while (!((catsize <= 50) && (catsize >= 5))) {
                catsize = 0;
                cout<<"The phrase appears to be valid. Enter a category to describe it: "<<endl;
                cin.getline(category, 50, '\n');
                for (int checkcatsize = 0; checkcatsize < sizeof(category); checkcatsize++) {
                    if (category[checkcatsize] != '\0') {
                        catsize++;
                    }
                }
                if (!((catsize <= 50) && (catsize >= 5))) {
                    cout<<"The category name must be between 5 and 50 characters. Please try again."<<endl<<endl;
                }
            }
        }
        //Now to figure out where each character will be located on the board and calculate the location in the solved array in relationship to the location on the board and add the characters to the array
        /* These values are where each line starts and stops in the square array and the solved array...
        LINE 1: 0 -- 11
        LINE 2: 12 -- 25
        LINE 3: 26 -- 39
        LINE 4: 40 -- 52
        -------------------------------------------------------------------------------------------
        if 1 line needed, stick on line 2
        if 2 needed, lines 2 and 3
        if 3 neeeded, lines 2, 3, and 4
        if 4 needed, all 4 lines.
        -------------------------------------------------------------------------------------------
        The line character count is located in linechars. we need to write the solution to the solved array.
        The array subscript is the number of the line minus 1.
        */
        //First, we have to determine how to horizontally center the words on each line... this can be done using the modulous operator... :D
        cout<<"Total Lines: "<<lines<<endl;
        for (int centerwords = 0; centerwords < lines; centerwords++) {
            //line lengths are stored in linechars[centerwords].
            if (lines == 4) {
                if (centerwords == 0 || centerwords == 3) { //lines 1 and 4 have 12 characters.
                    cll = 12; //current line length is 12
                } else { //lines 2 and 3 have 14 characters
                    cll = 14; //current line length is 14
                }
            } else {
                if (centerwords == 1 || centerwords == 2) { //lines 1 and 4 have 12 characters.
                    cll = 14; //current line length is 12
                } else { //lines 2 and 3 have 14 characters
                    cll = 12; //current line length is 14
                }
            }

            startpos=(int)((cll-linechars[centerwords])/2); //start position is the current line lenght minus the number of characters in the line divided by 2

            if (centerwords == 0) {
                offset = 0;
            } else if (centerwords == 1) {
                offset = 12;
            } else if (centerwords == 2) {
                offset = 26;
            } else if (centerwords == 3) { //this isn't simply an else just in case the centerwords gets higher than 3. That would cause a line to be overwritten.
                offset = 40;
            }
            if (lines < 4) {
                offset = offset + 13;
            }
            if (debug) {
                cout<<"Current Line Length: "<<cll<<"\tCurrent Line Characters: "<<linechars[centerwords]<<endl;
                cout<<"Difference: "<<(cll-linechars[centerwords])<<"\tDivided by 2: "<<((cll-linechars[centerwords])/2)<<endl;
                cout<<"startpos: "<<startpos<<"\toffset: "<<offset<<endl;
            }
            if ((cll - linechars[centerwords]) % 2 == 0) { //If you can perfectly center the word
                if (debug) {
                    cout<<"Line "<<(centerwords + 1)<<" has an even border."<<endl;
                }
            } else { // if the words do NOT fit onto the line perfectly
                if (debug) {
                    cout<<"Line "<<(centerwords + 1)<<" has an odd border."<<endl;
                    //startpos = startpos + 1;
                }
            }
            //FINALLY, after ALL of that, we will attempt to write the border spaces to the line...
            if (debug) {
                cout<<"offset + startpos: "<<(offset + startpos)<<endl;
                cout<<"offset + (cll - startpos): "<<(offset + (cll - startpos))<<endl;
            }
            for (int positiontext = (offset + startpos); positiontext < (offset + (cll - startpos)); positiontext++) {
                solved[(positiontext)] = linedata[centerwords][positiontext - (offset + startpos)];
                if (debug) {
                    cout<<"Writing Character \""<<linedata[centerwords][positiontext - (offset + startpos)]<<"\" at "<<positiontext<<endl;
                }
            }
        }
        //And then, we need to draw squares on the board where the characters are located...
        for (int setsquares = 0; setsquares < 52; setsquares++) {
            if ((solved[setsquares] != ' ') && (solved[setsquares] != '\0')) {
                #ifdef _WIN32
                square[setsquares] = '\xDB'; //Unicode character 2588, from Code Page 437.
                #else
                square[setsquares] = '\u2588'; //Unicode character 2588, from Code Page 437.
                #endif
            }
        }
        //Additional Debugging Information
        //if (debug) {
        cout<<"Displaying Board Solution:"<<endl;
        for (int showsolved = 0; showsolved < sizeof(solved); showsolved++) {
            if (showsolved == 0 || (showsolved == 40)) { //this is used for making the solution look vaguely similar to the actual rendered board. these ones are the the LEFT of the grid.
                cout<<"\xF0\xF0\xF0\xFF\xF0\xF0\xF0\xFF";
            }
            if (showsolved == 12 || (showsolved == 26)) { //for the lines with 14 characters, we still need to make them look centered.
                cout<<"\xF0\xF0\xF0\xFF";
            }
            cout<<"\""<<solved[showsolved]<<"\" ";
            if (showsolved == 11 || showsolved == 51) { //this is also used to make the solution look similar to the actual game board, yet these are to the right of the grid.
                cout<<"\xF0\xF0\xF0\xFF\xF0\xF0\xF0\xFF";
            }
            if (showsolved == 25 || (showsolved == 39)) { //also for the lines with 14 characters, but at the end of the lines to give the illusion that it is a solid block.
                cout<<"\xF0\xF0\xF0\xFF";
            }
            if (showsolved == 11 || showsolved == 25 || showsolved == 39 || showsolved == 51) {
                cout<<endl;
            }
        }
        cout<<"The phrase entered was: \"";
		phrlen = 0;
        for (int phrword = 0; phrword < wordcount; phrword++) {
            cout<<words[phrword];
			phrlen = phrlen + wordlen[phrword];
            if (phrword < (wordcount - 1)) {
                cout<<" ";
				phrlen++;
            }
        }
        cout<<"\""<<endl;
		cout<<"The phrase is "<<phrlen<<" characters long."<<endl;
        if (lines != 0) {
            cout<<"Lines Needed: "<<lines<<endl;
        } else {
            cout<<"There were some problems with your input, please try again."<<endl;
        }
		for (int spaces = 0; spaces < 52; spaces++) {
			if ((solved[spaces] == ' ') || (solved[spaces] == '\0')) {
				revealed[spaces] = true;
			}
		}
    }
    if (debug) {
        cout<<"Everything seems to have gone PERFECTLY fine! :D";
    }
    for (int zerocash = 0; zerocash < 3; zerocash++) {
        money[zerocash] = 0;
    }
    if (setupfreeze) {
        while (true) {
        }
    }
}

void splash() { //Displays the splash screen with the text "Wheel of Fortune".
    int character = 0;
    /* "\u" (should) escape a Unicode character according to http://msdn.microsoft.com/en-us/library/2k3te2cs%28VS.80%29.aspx, but it's not working right yet (at least not in Visual Studio 6). */
    color('6');
    cls();
    cursor(0,0);
    for (int ln = 1; ln <= 17; ln++) {
        for (c = 1; c <= 76; c++) {
            #ifdef _WIN32
            cursor(ln,(c - 2)); //This positions the cursor on the screen. The -2 shifts the text over two columns to the left.
            #else
            cursor(ln,c); //Still positions the cursor on the screen, however, it looks fine without adjustments.
            #endif
            if (((ln == 1) && ((cr(6,7,c)) || (cr(15,16,c)) || (cr(18,19,c)) || (cr(26,27,c)) || (cr(29,35,c)) || (cr(38,45,c)) || (cr(47,48,c)) || (cr(60,66,c)) || (cr(70,77,c))))
                    || ((ln == 2) && ((cr(6,7,c)) || (cr(15,16,c)) || (cr(18,19,c)) || (cr(26,27,c)) || (cr(29,30,c)) || (cr(38,39,c)) || (cr(47,48,c)) || (cr(59,60,c)) || (cr(66,67,c)) || (cr(70,71,c))))
                    || ((ln == 3) && ((cr(6,7,c)) || (cr(11,11,c)) || (cr(15,16,c)) || (cr(18,19,c)) || (cr(26,27,c)) || (cr(29,30,c)) || (cr(38,39,c)) || (cr(47,48,c)) || (cr(58,59,c)) || (cr(67,68,c)) || (cr(70,71,c))))
                    || ((ln == 4) && ((cr(6,7,c)) || (cr(10,12,c)) || (cr(15,16,c)) || (cr(18,27,c)) || (cr(29,35,c)) || (cr(38,44,c)) || (cr(47,48,c)) || (cr(58,59,c)) || (cr(67,68,c)) || (cr(70,76,c))))
                    || ((ln == 5) && ((cr(6,7,c)) || (cr(9,10,c)) || (cr(12,13,c)) || (cr(15,16,c)) || (cr(18,19,c)) || (cr(26,27,c)) || (cr(29,30,c)) || (cr(38,39,c)) || (cr(47,48,c)) || (cr(58,59,c)) || (cr(67,68,c)) || (cr(70,71,c))))
                    || ((ln == 6) && ((cr(6,9,c)) || (cr(13,16,c)) || (cr(18,19,c)) || (cr(26,27,c)) || (cr(29,30,c)) || (cr(38,39,c)) || (cr(47,48,c)) || (cr(59,60,c)) || (cr(66,67,c)) || (cr(70,71,c))))
                    || ((ln == 7) && ((cr(6,8,c)) || (cr(14,16,c)) || (cr(18,19,c)) || (cr(26,27,c)) || (cr(29,36,c)) || (cr(38,45,c)) || (cr(47,48,c)) || (cr(60,66,c)) || (cr(70,71,c))))
                    /* Line 8 is blank. */
                    || ((ln == 9) && ((cr(6,13,c)) || (cr(17,23,c)) || (cr(27,33,c)) || (cr(37,46,c)) || (cr(48,49,c)) || (cr(55,56,c)) || (cr(58,61,c)) || (cr(66,67,c)) || (cr(70,77,c)) || (cr(70,71,c)) || (cr(0,0,c)) || (cr(0,0,c))))
                    || ((ln == 10) && ((cr(6,7,c)) || (cr(16,17,c)) || (cr(23,24,c)) || (cr(27,28,c)) || (cr(33,34,c)) || (cr(41,42,c)) || (cr(48,49,c)) || (cr(55,56,c)) || (cr(58,59,c)) || (cr(62,63,c)) || (cr(66,67,c)) || (cr(70,71,c))))
                    || ((ln == 11) && ((cr(6,7,c)) || (cr(15,16,c)) || (cr(24,25,c)) || (cr(27,28,c)) || (cr(33,34,c)) || (cr(41,42,c)) || (cr(48,49,c)) || (cr(55,56,c)) || (cr(58,59,c)) || (cr(62,63,c)) || (cr(66,67,c)) || (cr(70,71,c))))
                    || ((ln == 12) && ((cr(6,12,c)) || (cr(15,16,c)) || (cr(24,25,c)) || (cr(27,33,c)) || (cr(41,42,c)) || (cr(48,49,c)) || (cr(55,56,c)) || (cr(58,59,c)) || (cr(63,64,c)) || (cr(66,67,c)) || (cr(70,76,c)) || (cr(0,0,c))))
                    || ((ln == 13) && ((cr(6,7,c)) || (cr(15,16,c)) || (cr(24,25,c)) || (cr(27,28,c)) || (cr(32,33,c)) || (cr(41,42,c)) || (cr(48,49,c)) || (cr(55,56,c)) || (cr(58,59,c)) || (cr(64,65,c)) || (cr(66,67,c)) || (cr(70,71,c))))
                    || ((ln == 14) && ((cr(6,7,c)) || (cr(16,17,c)) || (cr(23,24,c)) || (cr(27,28,c)) || (cr(33,34,c)) || (cr(41,42,c)) || (cr(48,49,c)) || (cr(55,56,c)) || (cr(58,59,c)) || (cr(65,67,c)) || (cr(70,71,c)) || (cr(0,0,c))))
                    || ((ln == 15) && ((cr(6,7,c)) || (cr(17,23,c)) || (cr(27,28,c)) || (cr(34,35,c)) || (cr(41,42,c)) || (cr(49,55,c)) || (cr(58,59,c)) || (cr(66,67,c)) || (cr(70,77,c)) || (cr(70,71,c)) || (cr(0,0,c)) || (cr(0,0,c))))
                    /* Line 16 is blank. */
                    || ((ln == 17) && ((cr(6,76,c))))) {
                //wcslen returns the length of a wide-character string.
                #ifdef _WIN32
                cout<<"\xDF";
                #else
                cout<<"\u2580";
                #endif
                //WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), u2580, wcslen(u2580), NULL, NULL);
                //cout<<"*";
            }
            if (animation == true) {
                delay(1);
            }
        }
    }
    cursor(19,4);
    cout<<"WRITTEN BY DYLAN TAYLOR; PRESS ANY KEY TO CONTINUE...";
//Custom Blinking Cursor Animation
    fflush(stdin); //ignored any keys pressed up to this point, as they break the animation
    #ifdef _WIN32
    while (true) {
        //_kbhit is a function that returns 0 if no key has been hit, and if a key has been hit, it returns another integer...
        if (_kbhit()) { //if a keyboard key has been hit
            break; //break out of the while loop
        } else { //otherwise, if a keyboard key has NOT been hit
            cursor(19,57);
            #ifdef _WIN32
            cout<<"\xDC";
            #else
            cout<<"\u2584";
            #endif
            delay(500);
            cursor(19,57);
            cout<<"\xFF";
            delay(500);
        }
    }
    #endif
}

void board() {
    /*
        LINE 1: 0 -- 11
        LINE 2: 12 -- 25
        LINE 3: 26 -- 39
        LINE 4: 40 -- 52
    */
    cls(); //clear the screen
    while (!gameover) {
        cursor(1,15);
        color(boardcolor);
        cout<<"\xC9";
        for (int top = 0; top < 12; top++) {
            cout<<"\xCD\xCD\xCD";
            if (top != 11) {
                cout<<"\xCB";
            } else {
                cout<<"\xBB"<<endl;
            }
        }
        cursor(2,15);
        color(boardcolor);
        cout<<"\xBA";
        for (int line1 = 0; line1 < 12; line1++) {
            cout<<"\xFF";
            color(textcolor);
            if (cheat) {
                cout<<solved[line1];
            } else {
                if (!revealed[line1]) {
                    cout<<square[line1];
                } else {
                    cout<<solved[line1];
                }
            }
            color(boardcolor);
            cout<<"\xFF\xBA";
        }
        cursor(3,11);
        cout<<"\xC9";
        for (int toplong = 0; toplong < 14; toplong++) {
            cout<<"\xCD\xCD\xCD";
            if (toplong != 13) {
                cout<<"\xCE";
            } else {
                cout<<"\xBB"<<endl;
            }
        }
        cursor(4,11);
        color(boardcolor);
        cout<<"\xBA";
        for (int line2 = 12; line2 < 26; line2++) {
            cout<<"\xFF";
            color(textcolor);
            if (cheat) {
                cout<<solved[line2];
            } else {
                if (!revealed[line2]) {
                    cout<<square[line2];
                } else {
                    cout<<solved[line2];
                }
            }
            color(boardcolor);
            cout<<"\xFF\xBA";
        }
        cursor(5,11);
        cout<<"\xCC";
        for (int middle = 0; middle < 14; middle++) {
            cout<<"\xCD\xCD\xCD";
            if (middle != 13) {
                cout<<"\xCE";
            } else {
                cout<<"\xB9"<<endl;
            }
        }
        cursor(6,11);
        color(boardcolor);
        cout<<"\xBA";
        for (int line3 = 26; line3 < 40; line3++) {
            cout<<"\xFF";
            color(textcolor);
            if (cheat) {
                cout<<solved[line3];
            } else {
                if (!revealed[line3]) {
                    cout<<square[line3];
                } else {
                    cout<<solved[line3];
                }
            }
            color(boardcolor);
            cout<<"\xFF\xBA";
        }
        cursor(7,11);
        cout<<"\xC8";
        for (int botlong = 0; botlong < 14; botlong++) {
            cout<<"\xCD\xCD\xCD";
            if (botlong != 13) {
                cout<<"\xCE";
            } else {
                cout<<"\xBC"<<endl;
            }
        }
        cursor(8,15);
        cout<<"\xBA";
        for (int line4 = 40; line4 < 52; line4++) {
            cout<<"\xFF";
            color(textcolor);
            if (cheat) {
                cout<<solved[line4];
            } else {
                if (!revealed[line4]) {
                    cout<<square[line4];
                } else {
                    cout<<solved[line4];
                }
            }
            color(boardcolor);
            cout<<"\xFF\xBA";
        }
        cursor(9,15);
        cout<<"\xC8";
        for (int bot = 0; bot < 12; bot++) {
            cout<<"\xCD\xCD\xCD";
            if (bot != 11) {
                cout<<"\xCA";
            } else {
                cout<<"\xBC"<<endl;
            }
        }
        cursor(11,4);
        cout<<"Category: "<<category<<endl;
        cursor(13,4);
        color(c1color);
        cout<<players[0]<<": ";
        color(moneycolor);
        cout<<"$"<<money[0];
        cursor(14,4);
        color(c2color);
        cout<<players[1]<<": ";
        color(moneycolor);
        cout<<"$"<<money[1];
        cursor(15,4);
        color(c3color);
        cout<<players[2]<<": ";
        color(moneycolor);
        cout<<"$"<<money[2];
        cursor(17,4);
        color(textcolor);
        cin.clear();
        fflush(stdin);
        chosen = false;
        while (!chosen) {
            cout<<endl;
            cout<<"It is currently ";
            if (player == 0) {
                color(c1color);
            } else if (player == 1) {
                color(c2color);
            } else if (player == 2) {
                color(c3color);
            }
            cout<<players[player];
            color(textcolor);
            cout<<"'s turn. Guess a [L]etter or the Whole [P]hrase? ";
            cin.getline(selection, 9999, '\n');
            cout<<endl;
            chosen = true;
            if (toupper(selection[0]) == 'P') {
                wphrase();
            } else if (toupper(selection[0]) == 'L') {
                letter();
            } else {
                chosen = false;
                cout<<"Invalid Selection. Only Enter \"L\" or \"P\""<<endl;
                cin.clear();
            }
        }
    }
}

void color(char x) { //This custom code is needed to allow more than one color on the screen at a time.
    #ifdef _WIN32
    if (x == '0') { //Black
        system("color 0");
    } else if (x == '1') { //Blue
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    } else if (x == '2') { //Green
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    } else if (x == '3') { //Aqua
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    } else if (x == '4') { //Red
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED);
    } else if (x == '5') { //Purple
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED |
                                FOREGROUND_BLUE);
    } else if (x == '6') { //Yellow
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED |
                                FOREGROUND_GREEN);
    } else if (x == '7') { //White
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED |
                                FOREGROUND_GREEN | FOREGROUND_BLUE);
    } else if (x == '8') { //Gray
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);
    } else if (x == '9') { //Light Blue
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    } else if (x == 'A' || x == 'a') { //Light Green
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    } else if (x == 'B' || x == 'b') { //Light Aqua
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY | FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    } else if (x == 'C' || x == 'c') { //Light Red
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY | FOREGROUND_RED);
    } else if (x == 'D' || x == 'd') { //Light Purple
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY | FOREGROUND_RED |
                                FOREGROUND_BLUE);
    } else if (x == 'E' || x == 'e') { //Light Yellow
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY | FOREGROUND_RED |
                                FOREGROUND_GREEN);
    } else if (x == 'F' || x == 'f') { //Bright White
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_INTENSITY | FOREGROUND_RED |
                                FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    #else
    if (x == '0') { //Black
        cout<<"\033[0;30;40m";
    } else if (x == '1') { //Blue
        cout<<"\033[0;34;40m";
    } else if (x == '2') { //Green
        cout<<"\033[32;40m";
    } else if (x == '3') { //Aqua
        cout<<"\033[0;36;40m";
    } else if (x == '4') { //Red
        cout<<"\033[0;31;40m";
    } else if (x == '5') { //Purple
        cout<<"\033[0;35;40m";
    } else if (x == '6') { //Yellow
        cout<<"\033[0;33;40m";
    } else if (x == '7') { //White
        cout<<"\033[0;37;40m";
    } else if (x == '8') { //Gray
        cout<<"\033[1;30;40m";
    } else if (x == '9') { //Light Blue
        cout<<"\033[1;34;40m";
    } else if (x == 'A' || x == 'a') { //Light Green
        cout<<"\033[1;32;40m";
    } else if (x == 'B' || x == 'b') { //Light Aqua
        cout<<"\033[1;36;40m";
    } else if (x == 'C' || x == 'c') { //Light Red
        cout<<"\033[1;31;40m";
    } else if (x == 'D' || x == 'd') { //Light Purple
        cout<<"\033[1;35;40m";
    } else if (x == 'E' || x == 'e') { //Light Yellow
        cout<<"\033[1;33;40m";
    } else if (x == 'F' || x == 'f') { //Bright White
        cout<<"\033[1;37;40m";
    }
    #endif
}

bool cr(int a, int b, int value) { //Checks if value is within the range of the two numbers.
    if ((a <= value && value <= b) || (b <= value && value <= a)) {
        return true;
    } else {
        return false;
    }
}


void cls() { //clears the screen without cls, preventing flickering with fast refresh rates. This is necessary because cls is waaaay too slow.
    cursor(0,0); //moves the cursor to the top left corner of the screen
    for (int clear = 0; clear < srows; clear++) { //while the variable clear is less than the number of rows on the screen
        for (int c = 0; c < 80; c++) {
            cout<<endl;
            //putchar('\n'); //write a new blank line to the current line on the screen
        }
    }
    //when done clearing the screen, move the cursor back to the top left corner of the screen.
    cursor(0,0);
}

void delay(int stime) { //delays the execution of the program by [stime] microseconds.
    #ifdef _WIN32
    Sleep(stime);
    #else
    //usleep needs to be used here because on non-windows systems, sleep accepts the time in seconds.
    //If you do the math, that would mean the delay time would be multiplied by 1,000.
    stime = stime * 1000;
    usleep(stime);
    #endif
}

void cursor(int x, int y) { //moves the console's cursor to the specified location on the screen.
    #ifdef _WIN32
    COORD location = {y,x}; //This is a coordinate pair.
    SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), location );
    #else
    cout<<"\033["<<(x + 1)<<";"<<y<<"f";
    #endif
}

void fullscreen() {
    #ifdef _WIN32 //Because this code is Windows specific, it needs to be within an #ifdef directive
    if (fscreen) {
        typedef BOOL (WINAPI *PROCSETCONSOLEDISPLAYMODE)(HANDLE,DWORD,LPDWORD);
        typedef BOOL (WINAPI *PROCGETCONSOLEDISPLAYMODE)(LPDWORD);
        PROCSETCONSOLEDISPLAYMODE SetConsoleDisplayMode;
        PROCGETCONSOLEDISPLAYMODE GetConsoleDisplayMode;
        HMODULE hKernel32 = GetModuleHandleA("kernel32");
        SetConsoleDisplayMode = (PROCSETCONSOLEDISPLAYMODE )
                                GetProcAddress(hKernel32,"SetConsoleDisplayMode");
        GetConsoleDisplayMode = (PROCGETCONSOLEDISPLAYMODE)
                                GetProcAddress(hKernel32,"GetConsoleDisplayMode");
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwOldMode;
        SetConsoleDisplayMode(hOut,1,&dwOldMode);
        cls();
    }
    #endif
}

void spin() {
    int multiple = rand(1,10); //The multiple of the value is from 1 to 10.
    curvalue = (multiple * 100); //Multiplies the multiple by 100 so that the range is 100 to 1000.
    cout<<"Spinning the spinner to determine the value of each letter... $"<<curvalue<<"!!!"<<endl;
}

int scmp(string a,string b) { //Case Sensitive STRING Comparison -- This does NOT compare character arrays.
    int result = (a.compare(b));
    return result;
}

int rand(int min, int max) {
    int r = (rand() % max + min);
    return r;
}

void lettersguessed() { //display letters guessed
    cout<<"Letters Guessed: ";
    for (int checkletters = 0; checkletters < 26; checkletters++) {
        if (guessed[checkletters]) {
            cout<<lguessed[checkletters];
        }
    }
    cout<<endl;
}

void letter() { //guessing a letter
    char guess = ' ';
	bool guessed = false;
    cout<<"Enter a letter to guess: ";
    cin>>guess;
    int mc = 0; //counter for how many letters match
    for (int ckl = 0; ckl < 52; ckl++) {
        if ((toupper(guess)) == (toupper(solved[ckl])) && (revealed[ckl] != true)) {
            mc++;
            revealed[ckl] = true; //makes the guessed letters appear.
            square[ckl] = toupper(solved[ckl]); //displays the letter on the grid
        }
		if (revealed[ckl] != true) {
			guessed = true;
		}
    }
    //spin the spinner to get the value of the letters
    spin();
    money[player] = money[player] + (mc * curvalue);
    if (mc != 0) {
        if (mc == 1) {
            cout<<"The character "<<guess<<" appears in the phrase "<<mc<<" time."<<endl;
        } else {
            cout<<"The character "<<guess<<" appears in the phrase "<<mc<<" time."<<endl;
        }
        cout<<"Congratulations! You just won $"<<(mc * curvalue)<<"!"<<endl;
    } else {
		if (!guessed) {
			cout<<"Sorry... the character "<<guess<<" is not in the phrase. Try again next time!"<<endl;
		} else {
			cout<<"Sorry... the character "<<guess<<" was alread guessed. Try again next time!"<<endl;
		}
    }
    system("pause");
    turnover();
}

void wphrase() { //guessing the whole phrase
    char guess[52];
    bool correctguess = true; //initial value only, don't worry... :P
    if (cheat) {
        cout<<"I suggest entering ";
        for (int cheatphrase = 0; cheatphrase < 52; cheatphrase++) {
            cout<<phrase[cheatphrase];
        }
        //cout<<"\" as your phrase..."<<endl;
        cout<<endl;
    }
    cout<<"Enter what you think the phrase is, all at once: "<<endl;
    cin.getline(guess, 52, '\n');
    for (int checkit = 0; checkit < phrlen; checkit++) {
        if ((toupper(guess[checkit]) != toupper(phrase[checkit])) && ((phrase[checkit] != ' ') && (phrase[checkit] != '\0') && (guess[checkit] != 32))) {
            correctguess = false;
        }
        if (debug) {
            //cout<<toupper(guess[checkit])<<"\t"<<toupper(phrase[checkit]);
        }
        cout<<endl;
    }
    if (correctguess) {
        cout<<"Congratulations!!! You guessed the whole phrase correctly!"<<endl;
        money[player] = money[player] + 6000;
        completed(); //game is over
    } else {
        cout<<"Sorry, that guess is incorrect. You need to enter the phrase in its original format."<<endl;
        system("pause");
        turnover();
    }
}

void complete() {
    bool gamesolved = true;
    for (int cc = 0; cc < 52; cc++) {
        //if ((square[cc] != solved[cc]) && (square[cc] != '\0') && (square[cc] != ' ') && (solved[cc] != '\0') && (solved[cc] != ' ')) {
		if (!revealed[cc]) {
            gamesolved = false;
        }
    }
    if (gamesolved) {
        completed();
    }
}

void completed() { //the whole phrase was guessed -- determine the winner.
    gameover = true;
	cls();
	cursor(1,1);
    color('E');
    cout<<"Game Over! The whole phrase has been guessed! ";
    int max = 0;
    int winner;
    for (int repeat = 0; repeat < 3; repeat++) {
        for (int findwinner = 0; findwinner < 3; findwinner++) {
            if (money[findwinner] > max) {
                max = money[findwinner];
                winner = findwinner;
            }
        }
    }
    cout<<"The winner is ";
    if (winner == 0) {
        color(c1color);
    } else if (winner == 1) {
        color(c2color);
    } else {
        color(c3color);
    }
    cout<<players[winner];
    color('E');
    cout<<"!"<<endl;
	cursor(13,4);
	color(c1color);
    cout<<players[0]<<": ";
    color(moneycolor);
    cout<<"$"<<money[0];
    cursor(14,4);
    color(c2color);
    cout<<players[1]<<": ";
    color(moneycolor);
    cout<<"$"<<money[1];
    cursor(15,4);
    color(c3color);
    cout<<players[2]<<": ";
    color(moneycolor);
    cout<<"$"<<money[2];
    cursor(17,4);
}

void turnover() { //switches turns
    complete();
    player++;
    if (player >= 3) {
        player = 0;
    }
	if (!gameover) {
		cls();
	} else {
		while (true) {

		}
	}
}
