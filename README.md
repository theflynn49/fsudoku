
# FSUDOKU

Version 1.0 2023/09

A simple game of Sudoku or alike for the AGON Light

It works with the Emulator or the real thing, VDP/MOS 1.03/1.03 and 1.04RC1/1.04RC1

###  Intent

I have being willing to write a backtracking algo for sudoku forever, and since
I was also willing to try to do something for the Agon Light, I just killed two birds with one stone.

So here is a dirty written-at-a-tube little thing which I guarantee contains not a single 
good programming practice, meaning expect ugly menues, a lot of ~~bugs~~ strange features, 
and barely no comments in the code. 

It is a text game,  written in "C" using z88dk (https://z88dk.org) producing ADL=0 type code.
You should use a recent "night build" version, greater than 2.2, to get Agon support 
(Mine is a daily build dated 2023/08/08)

### the Game

It supports Sudoku Classic, Aztec and Jigsaw types. 'Aztec' being a form of jigsaw looking like an aztec pyramid,
which I find fun and easy to play. Jigsaw is a symmetric kind of jigsaw type.

You can get puzzles from a small included library, ask Fsudoku to generate a random one for you,
enter your own puzzle, ask Fsudoku to solve it for you and verify there is only one solution.

Solving a puzzle you can enter notes (only one set infortunately) or ask Fsudoku to compute them
for you.

It show errors if you want, either errors against the sudoku rules, or any difference from the
puzzle solution. "Errors None" is the hard way to go unless it's cheating.

I think Aztec is easy because it implies more constraints, for example if you name the cells
from a1..a9 for the top row to i1..i9 for the bottom one, then

-a1 b1 h1 i1 must be the same set than d2 e2 f2 e3  

and the 3 other equivalent rules obtained by rotating symmetry

### Installation

Drop the two files "fsudoku.bin" and "fsudoku.lib" somewhere on your Agon SDcard, and type :
*load fsudoku.bin
*run
These files should be in the github "Release" folder.

A file "fsudoku.sav" may be generated if you save a self-edited game

To recompile Fsudoku, look at the "build" file. I am too lazy to make a Makefile.


### Main Menu

-T : choose a type of game, Classic, Aztec or Jigsaw

-D : Choose a level of difficulty
     The difficulty level is somehow accurate for the library games, much less for the 
     generated ones, which in fact support only two level, the first from "easy to normal",
     and a more difficult one from "hard to hellish". The generator may take a LONG time
     to generate a puzzle of the latter difficulty level.

-L : Choose a library game randomly, obfuscate it and go to the player Menu

-G : try to generate a random puzzle with a difficulty level somehow alike to the one chosen above.
     You can see its thinking on the screen, for an amusing animation, or not. It should be
     useless to cheat trying to remember the solution, since it will be obfuscated at the end of 
     the process.
     The real difficulty of the puzzle is pretty random, as there is no real way to predict
     how difficult it will be to solve it from the hints. I only made an attempt to make it a little
     more difficult for "hard" and above. But in practice it's just slower to generate...

-E : Enter the Edit Puzzzle Menu

-C : If a previous game is in memory, it goes back to the Player Menu.
     If an invalid game is in memory (an edited game with no or many solutions) an error
     message is displayed. You should go to the "E"dit Menu to fix and "T"est the game
     sucessfully before you can play it.

-X : return to MOS

### The Player Menu

Whether you get there by typing L, G or C at the main Menu, you get the same "Player Menu" screen.

Unless you changed these colors, the grid will show the "Hints" (puzzle definition cells) in blue,
the digits you enter in yellow, and the errors in red. If the error level is "None" there will be
no red anywhere.

The digits bars in the bottom right part of the screen show for each digit from '1' to '9' the number 
left to place in the grid. (Usually, the smallest is the easiest to place)

-You can move the cursor with the arrow keys

-Use the '1' to '9' keys to write a digit in a cell. Use '0' to erase a cell

-You ca enter "notes" in a cell by typing "Alt-1" to "Alt-9" ; AltGr works too. '0' erase the notes also.

-You can use "BackSpace" to UNDO a move, helping backing track manually. "TAB" will REDO your moves.
 The memory for moves is limited to ithe last 40 moves, until z88dk supports ADL=1 code :D

-P will compute the notes for you, filling them with all the possible values for each cell, only
 regarding the sudoku rules. If you hit 'P' again, you'll get your notes back. modifying the 
 computed notes makes them into your personal notes.

-W changes the "W"arning level in case of errors. If "None" is selected, you will NOT be warned until
 the last digit is entered in the last free cell, making finding the error painful and cumbersome, but
 that's how the game works. Is you choose "Sudoku" error level, you'll be warn as soon as you break a 
 Sudoku rule, sparing much frustration. Chossing "All" is cheating.

-S is also cheating by showing up the solution for the puzzle. Use UNDO then REDO to revert this action.

-R restart the game by claring all empty cells, including the notes.

-X return to the Main Menu. The game is not lost, you can get it back from the main menu by hitting "C"

-"-" and "+" change the colors of the grid is you don't like them. The item to be changed may be
 selected by hitting "/". There is no way to record your choice else than by recompiling the code.

### The EDIT Menu

Once in the EDIT Menu, everything looks pretty much like in the Player menu, but you can write everywhere
in the grid, without any constraints.

"1" to "9" will write or overwrite a cell, making it looking as a hint-cell.
It will be "red" is there is a sudoku rules conflict, but you can fix that later.

Hitting "P" will show/hide the auto-generated notes, which upgrade automatically.

At any time you can hit "T" to test the puzzle, telling you if there is no/one/more_than_one solution to
this puzzle. If there is more than one solution, fsudoku will show you the common cells to the two first 
solutions, and will empty cells with each solution's values as notes. This may help to add some hints to the
puzzle to make it a "only one" solution one.

"L" and "S" will respectively read and write the current puzzle in the file "fsudoku.sav"; only one game 
can be saved there.

The other valid commands are "-", "+", "/" and X, identical to the Player Menu.

### Credits

I found Peter Norvig's study (http://norvig.com/sudoku.html) very useful and inspiring. 

This study : https://sites.math.washington.edu/~morrow/mcm/team2280.pdf was also interesting.


