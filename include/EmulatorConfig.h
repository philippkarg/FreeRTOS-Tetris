#ifndef __EMULATOR_CONFIG_H__
#define __EMULATOR_CONFIG_H__

/*!
 * \addtogroup config 
 * @{  
 * \name Emulator Configuration
 * @{
 */

#define WINDOW_TITLE "FreeRTOS Tetris"          ///< Window title
#define SCREEN_WIDTH 700                        ///< Window width
#define SCREEN_HEIGHT 640                       ///< Window height
#define RESOURCES_DIRECTORY "../resources"      ///< Directory path for resources, e.g. images
#define FONTS_DIRECTORY "../resources/fonts"    ///< Direcotory path for fonts
#define GAMEBOY_FONT "Early GameBoy.ttf"        ///< Gameboy Font (default Font)
#define DEFAULT_FONT GAMEBOY_FONT               ///< Default Font
#define DEFAULT_FONT_SIZE 15                    ///< Default Font Size

#define configFPS_LIMIT 1                       
#define configFPS_LIMIT_RATE 50
#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)

///@}@}

#endif //__EMULATOR_CONFIG_H__
