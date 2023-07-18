#include <curses.h>
#include <signal.h>
#include <bits/stdc++.h>
#include <locale.h>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>

#define RED_CARD 1
#undef KEY_ENTER
#define KEY_ENTER 10
#define KEY_ESCAPE 27

struct Card{
    int score;
    std::string value;
    const wchar_t *face;
    bool red;
};

const std::ostringstream sstream;
bool playing = false;
int height, width;
std::vector<Card> playerCards;
int playerScore = 0;
std::vector<Card> dealerCards;
int dealerScore = 0;

std::vector<Card> currentDeck;
const std::vector<Card> deck = {
    {11, "A", L"♤", false},{11, "A", L"♧", false},{11, "A", L"♡", true},{11, "A", L"♢", true},
    {2, "2", L"♤", false},{2, "2", L"♧", false},{2, "2", L"♡", true},{2, "2", L"♢", true},
    {3, "3", L"♤", false},{3, "3", L"♧", false},{3, "3", L"♡", true},{3, "3", L"♢", true},
    {4, "4", L"♤", false},{4, "4", L"♧", false},{4, "4", L"♡", true},{4, "4", L"♢", true},
    {5, "5", L"♤", false},{5, "5", L"♧", false},{5, "5", L"♡", true},{5, "5", L"♢", true},
    {6, "6", L"♤", false},{6, "6", L"♧", false},{6, "6", L"♡", true},{6, "6", L"♢", true},
    {7, "7", L"♤", false},{7, "7", L"♧", false},{7, "7", L"♡", true},{7, "7", L"♢", true},
    {8, "8", L"♤", false},{8, "8", L"♧", false},{8, "8", L"♡", true},{8, "8", L"♢", true},
    {9, "9", L"♤", false},{9, "9", L"♧", false},{9, "9", L"♡", true},{9, "9", L"♢", true},
    {10, "10", L"♤", false},{10, "10", L"♧", false},{10, "10", L"♡", true},{10, "10", L"♢", true},
    {10, "J", L"♤", false},{10, "J", L"♧", false},{10, "J", L"♡", true},{10, "J", L"♢", true},
    {10, "Q", L"♤", false},{10, "Q", L"♧", false},{10, "Q", L"♡", true},{10, "Q", L"♢", true},
    {10, "K", L"♤", false},{10, "K", L"♧", false},{10, "K", L"♡", true},{10, "K", L"♢", true} 
};

void playGame();
void endGame();
void replayGame();
void clearInput(int&);
void hit();
int getNewCard(std::vector<Card>&);
void sizeChangehandler(int);
void updateScreen();
void showScore(int, int);
void showCard(Card);

int main(int argc, char *argv[]){
    // Initialize all the necessities
    initscr();
    setlocale(LC_ALL, "");
    start_color();
    init_pair(RED_CARD, COLOR_RED, COLOR_BLACK);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, height, width);
    signal(SIGWINCH, sizeChangehandler);
    srand (time(NULL));

    playGame();
    endwin();
    return 0;
}

// Handles all the game logic
void playGame(){
    // Clear the window on new game
    clear();
    endwin();
    playing = true;

    // Create a new deck and deal to cards to the dealer and player
    currentDeck = deck;
    playerScore = 0;
    playerCards = {};
    for(int i = 0; i<2; i++) hit();
    dealerScore = 0;
    dealerCards = {};
    for(int i = 0; i<2; i++) dealerScore += getNewCard(dealerCards);
    updateScreen();
    int x = 0;

    // Main game loop that handles all inputs
    while(playing){
        int input = getch();
        if(input == KEY_ESCAPE){
            endwin();
            exit(0);
        }
        if(input == KEY_BACKSPACE){
            if(x>0) x--;
            move(height - 1, x);
            delch();
        }
        else if(input == KEY_ENTER){
            int choice = mvinch(height - 1, 0);
            clearInput(x);
            if(choice == *"s" || choice == *"S") endGame();
            else if(choice == *"h" || choice == *"H"){
                hit();
            }
            updateScreen();
        }
        else x++;
    }

    replayGame();
}

// Called when the game ends
// Checks whether or nor the player wins
void endGame(){
    clear();
    endwin();
    playing = false;

    if(dealerScore < 17) dealerScore += getNewCard(dealerCards);
    if(dealerScore > 21){
        for(Card& card: dealerCards){
            if(card.score == 11){
                card.score = 1;
                dealerScore -= 10;
                if(dealerScore <= 21) break;
            }
        }
        if(dealerScore > 21){
            mvprintw(height*0.5, width*0.5-7, "The dealer busted!");
            return;
        }
    }

    if(dealerScore >= playerScore) mvprintw(height*0.5, width*0.5-6, "The dealer wins!");
    else mvprintw(height*0.5, width*0.5-3, "You win!");
}

// Called after the game finishes
// Asks whether or not the player wants to play again
void replayGame(){
    int x = 0;
    while(true){
        int input = getch();
        if(input == KEY_ESCAPE){
            endwin();
            exit(0);
        }
        if(input == KEY_BACKSPACE){
            if(x>0) x--;
            move(height - 1, x);
            delch();
        }
        else if(input == KEY_ENTER){
            int choice = mvinch(height - 1, 0);
            clearInput(x);
            if(choice == *"y" || choice == *"Y"){
                playGame();
                break;
            }
            else if(choice == *"n" || choice == *"Y"){
                endwin();
                exit(0);
            }
        }
        else x++;
    }
}

// Deletes user input
void clearInput(int& x){
    for(int i = x; i>=0; i--){
            move(height - 1, i);
            delch();
        }
    x = 0;
}

// Called when the player asks for a hit 
// Gives the player a new card and checks whether he busts
void hit(){
    playerScore += getNewCard(playerCards);
    if(playerScore > 21){
        bool bust = true;
        for (Card& card: playerCards){
            if(card.score == 11){
                card.score = 1;
                playerScore -= 10;
                bust = false;
                break;
            }
        }
        if(bust){
            mvprintw(height*0.5, width*0.5-2, "Bust!");
            playing = false;
        }
    }

    if(playerScore == 21){
        mvprintw(height*0.5, width*0.5-5, "Blackjack!");
        playing = false;
    }
}

// Gives a new card to either the player or dealer.
int getNewCard(std::vector<Card>& cards){
    int pickedCard = rand() % currentDeck.size();
    cards.push_back(currentDeck[pickedCard]);
    currentDeck.erase(currentDeck.begin() + pickedCard);
    return cards.back().score;
}

// Called whether the screen changes size
// Gets the new size and then refreshes the screen
void sizeChangehandler(int sig){
    clear();
    endwin();
    initscr();

    int newHeight, newWidth;
    getmaxyx(stdscr, newHeight, newWidth);
    height = newHeight;
    width = newWidth;
    updateScreen();
}

// Handles the screen output
void updateScreen(){
    // Get the positions of where cards should be put.
    int heightMid = round(height*0.5);
    int widthMid = round(width*0.5);
    int dealerDisplayScore = 0;
    int dealerCardsPos = widthMid-ceil(dealerCards.size()*1.5);
    if(dealerCardsPos < 0) dealerCardsPos = 0;
    int playerCardsPos = widthMid-ceil(playerCards.size()*1.5);
    if(playerCardsPos < 0) playerCardsPos = 0;

    // Show the dealer cards and score
    move(2, dealerCardsPos);
    addch(*"|");
    if(playing) printw("XX|");
    for(int i = playing; i<dealerCards.size(); i++){
        dealerDisplayScore += dealerCards[i].score;
        showCard(dealerCards[i]);
    }
    showScore(dealerDisplayScore, 1);

    // Show the player cards and score
    move(height - 3, playerCardsPos);
    addch(*"|");
    for (Card& card: playerCards) showCard(card);
    showScore(playerScore, height - 2);

    // Asks for user input
    if(playing) mvprintw(height - 2, 0, "(S)tay or (H)it");
    else mvprintw(height - 2, 0, "Play again? (Y)es or (N)o");
    move(height - 1, 0);

    refresh();
}

// Shows the score of either the dealer or the player
void showScore(int score, int posY){
    std::string display = std::to_string(score) + "/21";
    char* charArray = new char[display.length() + 1];
    strcpy(charArray, display.c_str());
    mvprintw(posY, width - 1 - display.length(), "%s", charArray);
    free(charArray);
}

// Displays a single card
void showCard(Card card){
    if(card.red){
        attron(COLOR_PAIR(RED_CARD));
    }
    char* charArray = new char[card.value.length() + 1];
    strcpy(charArray, card.value.c_str());
    printw("%s", charArray);
    addwstr(card.face);
    free(charArray);
    attroff(COLOR_PAIR(RED_CARD));
    addch(*"|");
}