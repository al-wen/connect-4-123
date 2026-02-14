## Connect 4

# Game

For this implementation of Connect 4, I used the Tic Tac Toe code as a template to help me get started. I adjusted Application.cpp to include the option of Connect 4, and a way to switch between player first and AI first.

For the Connect 4 portion, I adjusted the Tic Tac Toe board to have the right board. For placing pieces, I would check the piece below with getS() until there was a piece there or it was the bottom of the board. For checking the winner, I used a 2D array, which listed every possible win combination, to check if and who won.

# Negamax AI with Alpha Beta Pruning

For the AI portion, I used the Negamax algorithm with alpha beta pruning. To evaluate the board, I checked every single 4 combination with a similar list of the one used above that was used to check wins. I then evaluated each combination to score the board. For alpha beta pruning, alpha shows the best possible score for the player while beta shows the worst. When alpha is greater than beta, the code would stop searching the branch. This is used to improve efficiency without affecting the outcome.