# Poker++

## To compile do following in terminal
#### make
```
./src/chat_server 8000
./src/client 127.0.0.1 8000
```

## Code compiles without any error but if code is crashed, try to compile is again and it works fine

# When 2 player is on the server
- start server
- connect to the server, 1st gui will show up, don't enter anything yet cause the server counts the no.of player
- if you click play and start entering your name it will think there is only one player on the server and it might crash.

# wait until you connect to the server, when 2nd gui pops up then you can start the game
- card is dealt from left to right
- player can swap card, fold, bet
- the game decides winner only by couting the betting round
- *when betting counter reaches 2 round then winner is decided*
- if fold is cliked it does store the player has folded but currently game won't decide the winner
- if you click fold recommeded to restart the game by starting the server again
- the betting must be done to determine winner
- game can be restarted when start again button clicked

# when 3 player on the server
- it deals card to 3 player 
- player can swap, fold ,bet
- game is not yet complete to determine the winner
