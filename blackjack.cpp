#include <curses.h>
#include <signal.h>
#include <bits/stdc++.h>
#include <locale.h>
#include <vector>

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

struct Player{
    std::vector<Card> playerCards;
    int score;
    char* gameState;
    bool playing;
};

std::vector<Player> players;
bool playing = false;
int height, width;
int playerCount = 1;
int playerPlaying = 0;
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
void hit(int);
void clearInput(int&);
int getNewCard(std::vector<Card>&);
void sizeChangehandler(int);
void updateScreen();
void showScore(int, int, int);
void showCard(Card);
bool startsWith(const char*, const char*);

int main(int argc, char *argv[]){

    // Get player count
    for (int i = 1; i < argc-1; i++){
        if(startsWith(argv[i], "-p")){
            playerCount = atoi(argv[i+1]);
            if(playerCount < 0 || playerCount > 5){
                std::cout << "Enter a player count between 1 and 5 \n";
                return 1;
            }
            break;
        }
    }
    players.resize(playerCount, {{}, 0, const_cast<char*>(""), true});

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
    playerPlaying = 0;
    for(int i = 0; i < playerCount; i++){
        players[i].playerCards = {};   
        players[i].score = 0;
        players[i].gameState = const_cast<char*>("");
        players[i].playing = true;
        for(int j = 0; j<2; j++) hit(i);
    }
    dealerScore = 0;
    dealerCards = {};
    for(int i = 0; i<2; i++) dealerScore += getNewCard(dealerCards);

    updateScreen();
    int x = 0;

    // Main game loop that handles all inputs
    while(playerPlaying < playerCount){
        while(players[playerPlaying].playing){
            updateScreen();
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
                if(choice == *"s" || choice == *"S") players[playerPlaying].playing = false;
                else if(choice == *"h" || choice == *"H"){
                    hit(playerPlaying);
                }
            }
            else x++;
        }
        playerPlaying++;
    }

    endGame();
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
            for(int i = 0; i<playerCount; i++){
                if(!strcmp(players[i].gameState, "")) players[i].gameState = const_cast<char*>("The dealer busted!");
            }
        }
    }

    for(int i = 0; i<playerCount; i++){
        if(!strcmp(players[i].gameState, "")){
            if(players[i].score <= dealerScore){
                players[i].gameState = const_cast<char*>("The dealer wins!");
            }
            else players[i].gameState = const_cast<char*>("    You win!    ");
        }
    }

    updateScreen();
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
            else if(choice == *"n" || choice == *"N"){
                endwin();
                exit(0);
            }
        }
        else x++;
    }
}

// Called when the player asks for a hit 
// Gives the player a new card and checks whether he busts
void hit(int i){
    players[i].score += getNewCard(players[i].playerCards);
    if(players[i].score > 21){
        bool bust = true;
        for (Card& card: players[i].playerCards){
            if(card.score == 11){
                card.score = 1;
                players[i].score -= 10;
                bust = false;
                break;
            }
        }
        if(bust){
            players[i].gameState = const_cast<char*>("     Bust!      ");
            players[i].playing = false;
        }
    }

    if(players[i].score == 21){
        players[i].gameState = const_cast<char*>("   Blackjack!   ");
        players[i].playing = false;
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

// Gives a new card to either the players or dealer.
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
    int playerDistance = ceil(width/(playerCount+1));
    if(playerDistance < 1) playerDistance = 1;

    // Show the dealer cards and score
    move(2, dealerCardsPos);
    addch(*"|");
    if(playing) printw("XX|");
    for(int i = playing; i<dealerCards.size(); i++){
        dealerDisplayScore += dealerCards[i].score;
        showCard(dealerCards[i]);
    }
    showScore(dealerDisplayScore, 1, widthMid);

    // Show the players cards and scores
    for(int i = 0; i<playerCount; i++){
        move(height - 4, playerDistance*(i+1) - players[i].playerCards.size()*1.5f - 1);
        addch(*"|");
        for (Card& card: players[i].playerCards) showCard(card);
        showScore(players[i].score, height - 3, playerDistance*(i+1));
        mvprintw(height - 5, playerDistance*(i+1) - 8, "%s", players[i].gameState);
    }

    // Asks for user input
    if(playing) mvprintw(height - 2, 0, "Player %d (S)tay or (H)it", playerPlaying+1);
    else mvprintw(height - 2, 0, "Play again? (Y)es or (N)o");
    move(height - 1, 0);

    refresh();
}

// Shows the score of either the dealer or the player
void showScore(int score, int posY, int posX){
    std::string display = std::to_string(score) + "/21";
    char* charArray = new char[display.length() + 1];
    strcpy(charArray, display.c_str());
    mvprintw(posY, posX - display.length()*0.5f, "%s", charArray);
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

bool startsWith(const char *a, const char *b){
   return !strncmp(a, b, strlen(b));
}