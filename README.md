# ESPL Tetris
## by Philipp Karg

### Config:
**Some configurations can be made in the [`tetrisConfig.h`](include/tetrisConfig.h) file:**
* There are 6 usernames that can be set
* If you want to enable soundeffects, set `ENABLE_SOUND_EFFECTS` to 1  
**WARNING: Sound effects may be very annoying or not in sync at all time**
* You can also change the background color, the block textures & sound effect files from here


### Controls:
* Up: Rotating the tetromino
* Down: Falling faster (this button is not debounced)
* Left/Right: Moving the tetromino to the left/right

### Menus/States:  
* **Main Menu Screen:** 
	* Click on the rotation/player mode to select
	* If a rotation & player mode have been selected, press S to start
	* Click on the "Select level" text to go to the level/highscores screen
		* Click on a level to select that level
	* If multi player mode is selected & a connection has been established,  
		click on a mode to select that mode  

* **Game Screen:**
	* Press Esc to Pause  

* **Pause Screen:**
	* Press Esc to continue
	* Press R to restart the game
	* Press M to go back to Main Menu
	* If the game is over:
		* Select your username by clicking
		* Press R to restart
		* Press M to go back to Main Menu
