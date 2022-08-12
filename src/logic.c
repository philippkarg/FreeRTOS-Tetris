#include "logic.h"

// **********************************************************************************
// Forward Declarations *************************************************************
// **********************************************************************************
/**
 * @ingroup logic
 * @brief Set a Tetromino's type.
 *
 * Randomly take one of the 7 possible tetromino types 
 * and reduce the number of possible types, 
 * so that every type is selected once for every seven new tetrominos.
 * @param[out] tetromino ( @ref tetromino_t *): Tetromino to set type for.
 * @param[inout] types ( @ref tetromino_type_t []): Array of tetromino types.
 * @param[inout] index (int*): Current index of the types array.
 */
static void setTetrominoType(tetromino_t *tetromino, tetromino_type_t types[], int *index);

/**
 * @ingroup logic
 * @brief Increase score if a row is full.
 * @param[in] score ( @ref score_t *): Score object to change. 
 * @param[in] rowsAmount (uint8_t): NUmber of rows cleared. 
 */
static void increaseScore(score_t *score, uint8_t rowsAmount);

/**
 * @ingroup logic
 * @brief Set the shape array of the tetromino, depending on the rotation & type.
 * @param[in] color ( @ref color_t): Color of the tetromino.
 * @param[in] type ( @ref tetromino_type_t): Type of the tetromino. 
 * @param[in] shape ( @ref color_t [][]): Shape array to set.
 * @param[in] rotateAmount (int): Amount of rotation for the shape. 
 * @param[in] init (bool): For some Tetrominos, the shape is set differently at the start. 
 */
static void setTetrominoShape(  color_t color, 
                                tetromino_type_t type, 
                                color_t shape[FIGURE_SIZE][FIGURE_SIZE],
                                int rotateAmount, 
                                bool init);

// **********************************************************************************
// Function Definitions *************************************************************
// **********************************************************************************
void vLogicInitTetromino(tetromino_t *tetromino, tetromino_type_t types[], int *index, player_mode_t playerMode)
{
    // Set Tetromino type
    if(playerMode == SINGLE_PLAYER)
        setTetrominoType(tetromino, types, index);
    // Set Tetromino rotation
    tetromino->rotation = (rand() % 3);
    // Set Tetromino color
    tetromino->color = (rand() % NUMBER_OF_TETRIS_COLORS) + 1;

    // Init positions & newPositions
    tetromino->position.y = 0;
    // The "I"-Tetromino should be centered nicely
    if(tetromino->type == I)
        tetromino->position.x = COLS/2 - 2;
    else
        tetromino->position.x = COLS/2 - 3;
    tetromino->newPosition = tetromino->position;
    
    // Set the shape array
    setTetrominoShape(tetromino->color, tetromino->type, tetromino->shape,tetromino->rotation, true);
}

static void setTetrominoType(tetromino_t *tetromino, tetromino_type_t types[], int *index)
{
    // If the first element of the sequence has no type,
    // this means that the sequence is empty & a new one needs to be generated
    // Therefore, initialize each element of the array with one of the possible Tetromino types
    if(types[0] == NO_TYPE)
    {
        *index = 7;
        for(int i=0; i<7; i++)
            types[i] = i+1;
    }

    // Generate a number between 1 and the sequence index
    int num = (rand() % (*index)) + 1;

    // Set the tetrominos type to the sequences entry at the random number
    tetromino->type = types[num-1];

    // Remove the element at the randomly generated number from the sequence
    if(num == 7)
        types[num-1] = NO_TYPE;
    else
    {
        for(int i=num-1; i<6; i++)
        {
            if(types[i+1] != NO_TYPE)
            {
                types[i] = types[i+1];
                types[i+1] = NO_TYPE;
            }
            else
                types[i] = NO_TYPE;
        }
    }
    
    // Lower the sequence index by one
    *index -= 1;
}


bool bLogicCheckMove(const color_t newShape[FIGURE_SIZE][FIGURE_SIZE], coord_t newPosition, const color_t landed[ROWS][COLS])
{
    for(int row=0; row<FIGURE_SIZE; row++)
    {
        for(int col=0; col<FIGURE_SIZE; col++)
        {
            if(newShape[row][col]!= EMPTY_SPACE)
            {
                // Collision on the ground
                if(row + newPosition.y >= ROWS) 
                    return false;
                // Collision on the left border
                if(col + newPosition.x < 0)
                    return false;
                // Collision on the right border
                if(col + newPosition.x >= COLS) 
                    return false;
                // Collision with the landed tetrominos
                if(landed   [ROWS-1 - (row + newPosition.y)]
                            [col + newPosition.x] 
                            != EMPTY_SPACE)
                    return false;
            }
        }
    }
    return true;
}

bool bLogicCheckGameOver(const tetromino_t *tetromino, const color_t landed[ROWS][COLS])
{
    // If the y-position of the current tetromino is 0
    // & a collision has been detected, the game is over
    return (tetromino->position.y == 0 && !bLogicCheckMove(tetromino->shape, tetromino->position, landed));
}

void vLogicUpdateXCoord(tetromino_t *tetromino, const color_t landed[ROWS][COLS], int pressedButton)
{
    if(pressedButton == LEFT_PRESSED)
    {
        // Try to move the Tetromino to the left
        tetromino->newPosition.x = tetromino->position.x - 1;
        if(bLogicCheckMove(tetromino->shape, tetromino->newPosition, landed))
            tetromino->position.x = tetromino->newPosition.x;
        else
            tetromino->newPosition.x = tetromino->position.x;
    } 
    if(pressedButton == RIGHT_PRESSED)
    {
        // Try to move the Tetromino to the right
        tetromino->newPosition.x = tetromino->position.x + 1;
        if(bLogicCheckMove(tetromino->shape, tetromino->newPosition, landed))
            tetromino->position.x = tetromino->newPosition.x;
        else
            tetromino->newPosition.x = tetromino->position.x;
    }
}

bool bLogicUpdateYCoord(tetromino_t *tetromino, const color_t landed[ROWS][COLS])
{
    tetromino->newPosition.y++;
    if(bLogicCheckMove(tetromino->shape, tetromino->newPosition, landed))
    { 
        tetromino->position.y = tetromino->newPosition.y;
        return true;
    }
    else
    {
        tetromino->newPosition.y = tetromino->position.y;
        return false;
    }
}


void vLogicRotate(tetromino_t *tetromino, const color_t landed[ROWS][COLS], rotation_t rotationMode)
{
    // Increase/decrease Tetromino's rotation counter, depending on the rotation mode
    int rotation = tetromino->rotation;
    if(rotationMode == LEFT)
    {
        if(tetromino->rotation > 0) rotation--;
        else rotation = 3;
    }
    if(rotationMode == RIGHT) rotation++;

    // Set Tetrominos newShape, that it have after the rotation
    setTetrominoShape(tetromino->color, tetromino->type, tetromino->newShape, rotation, false);

    // Check if the new shape collides with anything
    if(bLogicCheckMove(tetromino->newShape, tetromino->position, landed))
    {
        // If so, set the new shape as shape for the Tetromino
        for(int row=0; row<FIGURE_SIZE; row++)
            for(int col=0; col<FIGURE_SIZE; col++)
                tetromino->shape[row][col] = tetromino->newShape[row][col];
        // Set new rotation value
        tetromino->rotation = rotation;        
    }
}

void vLogicAddToLanded(const tetromino_t *tetromino, color_t landed[ROWS][COLS])
{
    for(int row=0; row<FIGURE_SIZE; row++)
        for(int col=0; col<FIGURE_SIZE; col++)
            if(tetromino->shape[row][col] != EMPTY_SPACE)
            {
                // The landed array's rows start at the bottom of the window
                // Therefore, the Tetromino's y-coord needs to be subtracted
                uint16_t rowIndex = ROWS-1 - (row + tetromino->position.y);
                uint16_t colIndex = col + tetromino->position.x;
                landed[rowIndex][colIndex] = tetromino->shape[row][col]; 
            }
}

bool vLogicRowFull(color_t landed[ROWS][COLS], score_t *score)
{
    uint8_t rowsAmount = 0;

    // If there is one empty space left in a row, rowFull is set to false
    for(int row=0; row<ROWS; row++)
    {
        bool rowFull = true;
        for(int col=0; col<COLS; col++)
            if(landed[row][col] == EMPTY_SPACE)
            {
                rowFull = false;
                break;
            }
        // If the row is indeed full, the rows above are shifted down
        if(rowFull)
        {
            for(int rowAbove = row; rowAbove<ROWS-1; rowAbove++)
                for(int col=0; col<COLS; col++)
                    landed[rowAbove][col] = landed[rowAbove+1][col];
            rowsAmount++;
            row--;
        }
    }
    
    if(rowsAmount)
    {
        increaseScore(score, rowsAmount);
        return true;
    }
    return false;  
}

static void increaseScore(score_t *score, uint8_t rowsAmount)
{   
    // Add full rows to the score
    score->rows += rowsAmount;
    
    // Increase score depending on how many rows were cleared & the level
    switch(rowsAmount)
    {
        case 1: 
            score->score += (score->level+1)*40;      
            break;
        case 2: 
            score->score += (score->level+1)*100;     
            break;
        case 3: 
            score->score += (score->level+1)*300; 
            break;
        case 4: 
            score->score += (score->level+1)*1200;
            break;
        default:
            break;
    }

    // For each 10 rows cleared, increase the level
    if(score->rows%10 == 0 && score->rows > 0)
        score->level++;
}


static void setTetrominoShape(  color_t color, 
                                tetromino_type_t type,
                                color_t shape[FIGURE_SIZE][FIGURE_SIZE],
                                int rotateAmount, 
                                bool init)
{

    for(int row=0; row<FIGURE_SIZE; row++)
        for(int col=0; col<FIGURE_SIZE; col++)
            shape[row][col] = 0;
    
    switch(type)
    {
        // * S-Tetromino *
        case S:
            switch(rotateAmount%4)
            {
                case 0:
                    shape[0][2] = color;
                    shape[0][3] = color;
                    shape[1][1] = color;
                    shape[1][2] = color;
                    break;
                case 1:
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    shape[2][3] = color;
                    break;
                case 2:
                    shape[0][2] = color;
                    shape[0][3] = color;
                    shape[1][1] = color;
                    shape[1][2] = color;
                    break;
                case 3:
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    shape[2][3] = color;
                    break;
                default:
                    break;
            }
            break;
        // * Z-Tetromino *
        case Z:
            switch(rotateAmount%4)
            {
                case 0:
                    shape[0][1] = color;
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    break;
                case 1:
                    shape[0][3] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    shape[2][2] = color;
                    break;
                case 2:
                    shape[0][1] = color;
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    break;
                case 3:
                    shape[0][3] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    shape[2][2] = color;
                    break;
                default:
                    break;
            }
            break;
        // * J-Tetromino *
        case J:
            switch(rotateAmount%4)
            {
                case 0:
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[2][1] = color;
                    shape[2][2] = color;
                    break;
                case 1:
                    shape[0][1] = color;
                    shape[1][1] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    break;
                case 2:
                    shape[0][2] = color;
                    shape[0][3] = color;
                    shape[1][2] = color;
                    shape[2][2] = color;
                    break;
                case 3:
                    if(init)
                    {
                        shape[1][1] = color;
                        shape[1][2] = color;
                        shape[1][3] = color;
                        shape[2][3] = color;
                    }
                    else
                    {
                        shape[1][1] = color;
                        shape[1][2] = color;
                        shape[1][3] = color;
                        shape[2][3] = color;
                    }
                    break;
                default:
                    break;
            }
            break;

        // * L-Tetromino *
        case L:
            switch(rotateAmount%4)
            {
                case 0:
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[2][2] = color;
                    shape[2][3] = color;
                    break;
                case 1:
                    if(init)
                    {
                        shape[0][1] = color;
                        shape[0][2] = color;
                        shape[0][3] = color;
                        shape[1][1] = color;
                    }
                    else
                    {
                        shape[1][1] = color;
                        shape[1][2] = color;
                        shape[1][3] = color;
                        shape[2][1] = color;
                    }
                    break;
                case 2:
                    shape[0][1] = color;
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[2][2] = color;
                    break;
                case 3:
                    shape[0][3] = color;
                    shape[1][1] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    break;
                default:
                    break;
            }
            break;

        // * T-Tetromino *
        case T:
            switch(rotateAmount%4)
            {
                case 0:
                    shape[0][2] = color;
                    shape[1][1] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    break;
                case 1:
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[1][3] = color;
                    shape[2][2] = color;
                    break;
                case 2:
                    if(init)
                    {
                        shape[0][1] = color;
                        shape[0][2] = color;
                        shape[0][3] = color;
                        shape[1][2] = color;
                    }
                    else
                    {
                        shape[1][1] = color;
                        shape[1][2] = color;
                        shape[1][3] = color;
                        shape[2][2] = color;
                    }
                    break;
                case 3:
                    shape[0][2] = color;
                    shape[1][1] = color;
                    shape[1][2] = color;
                    shape[2][2] = color;
                    break;
                default:
                    break;
            }
            break;

        // * O-Tetromino *
        case O:
            shape[0][2] = color;
            shape[0][3] = color;
            shape[1][2] = color;
            shape[1][3] = color;
            break;

        // * I-Tetromino *
        case I:
            switch(rotateAmount%4)
            {
                case 0:
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[2][2] = color;
                    shape[3][2] = color;
                    break;
                case 1:
                    if(init)
                    {
                        shape[0][0] = color;
                        shape[0][1] = color;
                        shape[0][2] = color;
                        shape[0][3] = color;
                    }
                    else
                    {
                        shape[2][0] = color;
                        shape[2][1] = color;
                        shape[2][2] = color;
                        shape[2][3] = color;
                    }
                    break;
                case 2:
                    shape[0][2] = color;
                    shape[1][2] = color;
                    shape[2][2] = color;
                    shape[3][2] = color;
                    break;
                case 3:
                    shape[2][0] = color;
                    shape[2][1] = color;
                    shape[2][2] = color;
                    shape[2][3] = color;
                    break;
                default:
                    break;
            }
            break;

        default: 
            break;
    } 
}