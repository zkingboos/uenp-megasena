#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_GAMES 3
#define MAX_NUMBERS 15
#define MAX_RANDOM_GAMES 7
#define MIN_NUMBERS 6
#define DRAW_SIZE 6
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0m"

typedef struct {
    int numbers[MAX_NUMBERS];
    int quantity;
    bool random;
} Game;

typedef struct {
    Game games[MAX_GAMES + MAX_RANDOM_GAMES];
    int gameCount;
    int draw[DRAW_SIZE];
} Ticket;

void orderNumbers(int numbers[], int quantity) {
    for (int i = 0; i < quantity - 1; i++) {
        for (int j = 0; j < quantity - 1 - i; j++) {
            if (numbers[j] > numbers[j + 1]) {
                int temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
}

void orderGameNumbers(Game *game) {
    orderNumbers(game->numbers, game->quantity);
}

bool hasNumberMatch(int draw[], int currentNumber) {
    for (int i = 0; i < DRAW_SIZE; ++i) {
        if (draw[i] == currentNumber) {
            return true;
        }
    }

    return false;
}

bool areGamesEqual(Game first, Game second) {
    if (first.quantity != second.quantity) {
        return false;
    }

    for (int index = 0; index < first.quantity; ++index) {
        if (first.numbers[index] != second.numbers[index]) {
            return false;
        }
    }

    return true;
}

bool createNewGame(Ticket *ticket, Game newGame) {
    for (int index = 0; index < ticket->gameCount; ++index) {
        if (areGamesEqual(ticket->games[index], newGame)) {
            printf("Game with exact same numbers already exists. Please specify a different game!\n");
            return false;
        }
    }

    ticket->games[ticket->gameCount] = newGame;
    ticket->gameCount++;
    return true;
}

void generateNewRandomGame(Ticket *ticket) {
    Game newGame = {
            .quantity = MIN_NUMBERS,
            .random = true
    };

    bool registeredNumbers[60] = {};
    do {
        for (int index = 0; index < MIN_NUMBERS; ++index) {
            int number;
            do {
                number = rand() % 60 + 1;
            } while (registeredNumbers[number] == true);

            registeredNumbers[number] = true;
            newGame.numbers[index] = number;
        }
        orderGameNumbers(&newGame);
    } while (!createNewGame(ticket, newGame));
}

int countMatches(Ticket ticket, Game game) {
    int *draw = ticket.draw;

    int matches = 0;
    int xIndex = 0;
    int yIndex = 0;

    while (xIndex < game.quantity && yIndex < DRAW_SIZE) {
        if (game.numbers[xIndex] == draw[yIndex]) {
            matches++;
            xIndex++;
            yIndex++;
        } else if (game.numbers[xIndex] < draw[yIndex]) {
            xIndex++;
        } else {
            yIndex++;
        }
    }

    return matches;
}

Game getUserGame(int numberQuantity, int gameNumber) {
    Game newUserGame = {.quantity = numberQuantity, .random = false};

    int index = 0;
    bool registeredNumbers[60] = {};
    do {
        int number;
        printf("Game(%d) #%d: Enter %d numbers for you game (1-60): ", gameNumber, index + 1, numberQuantity);
        scanf("%d", &number);

        if (number < 1 || number > 60) {
            printf("The number needs to be 1-60. Try again.\n");
            continue;
        }

        if (registeredNumbers[number] == true) {
            printf("This number was used before in the same game, please specify a new number.\n");
            continue;
        }

        registeredNumbers[number] = true;
        newUserGame.numbers[index] = number;
        index++;
    } while (index < numberQuantity);

    orderGameNumbers(&newUserGame);
    return newUserGame;
}

void generateRandomDraw(Ticket *ticket) {
    bool registeredNumbers[60] = {};
    for (int index = 0; index < DRAW_SIZE; ++index) {
        int number;
        do {
            number = rand() % 60 + 1;
        } while (registeredNumbers[number] == true);

        registeredNumbers[number] = true;
        ticket->draw[index] = number;
    }

    orderNumbers(ticket->draw, DRAW_SIZE);
}

int main() {
    srand(time(NULL));

    int gameCount;
    printf("Enter the number of games? (1-3): ");
    scanf("%d", &gameCount);

    if (gameCount < 1 || gameCount > MAX_GAMES) {
        printf("Invalid number of games.\n");
        return 1;
    }

    Ticket ticket = {
            .gameCount = 0
    };

    int numberQuantity;
    printf("Enter the number of numbers per game? (6-15): ");
    scanf("%d", &numberQuantity);

    if (numberQuantity < MIN_NUMBERS || numberQuantity > MAX_NUMBERS) {
        printf("Invalid number of numbers per game.\n");
        return 1;
    }

    for (int index = 0; index < gameCount; ++index) {
        printf("\n\n\t\tGame %d\n\n\n", index + 1);
        bool hasCreated = false;

        do {
            Game userGame = getUserGame(numberQuantity, index + 1);
            hasCreated = createNewGame(&ticket, userGame);
        } while (hasCreated == false);
    }

    int randomGameCount;
    printf("Enter the number of random games? (0-7): ");
    scanf("%d", &randomGameCount);

    if (randomGameCount < 0 || randomGameCount > MAX_RANDOM_GAMES) {
        printf("Invalid number of random games.\n");
        return 1;
    }

    printf("\tGenerating random games...\n");
    int currentGameCount = ticket.gameCount;
    for (int index = 0; index < randomGameCount; ++index) {
        printf("\t\tGenerating numbers for random game: %d\n", currentGameCount + index + 1);
        generateNewRandomGame(&ticket);
    }

    printf("\tGenerating draw...\n");
    generateRandomDraw(&ticket);

    printf("\tRegistered total of games: %d\n", ticket.gameCount);

    int totalMatches = 0;
    for (int i = 0; i < ticket.gameCount; ++i) {
        Game userGame = ticket.games[i];
        int matches = countMatches(ticket, userGame);

        if (userGame.random) {
            printf("\t\t[Game %d Random] You have %d matches. \n", i + 1, matches);
            printf("\t\t\tNumbers: ");
            for (int j = 0; j < userGame.quantity; ++j) {
                int number = userGame.numbers[j];
                if (hasNumberMatch(ticket.draw, number)) {
                    printf(COLOR_GREEN "%d " COLOR_RESET, number);
                } else {
                    printf("%d ", number);
                }

            }
            printf("\n");

        } else {
            printf("\t\t[Game %d] You have %d matches.\n", i + 1, matches);
            printf("\t\t\tNumbers: ");
            for (int j = 0; j < userGame.quantity; ++j) {
                int number = userGame.numbers[j];
                if (hasNumberMatch(ticket.draw, number)) {
                    printf(COLOR_GREEN "%d " COLOR_RESET, number);
                } else {
                    printf("%d ", number);
                }
            }
            printf("\n");
        }

        totalMatches += matches;
    }

    printf("\tDraw numbers: ");
    for (int i = 0; i < DRAW_SIZE; ++i) {
        printf("%d ", ticket.draw[i]);
    }
    printf("\n");

    printf("\tTotal matches: %d\n", totalMatches);
    if (totalMatches >= DRAW_SIZE) {
        printf("\t\tCongratulations! You won the big prize!\n");
    } else if (totalMatches >= 4) {
        printf("\t\tCongratulations! You won a prize!\n");
    } else {
        printf("\t\tSorry, you didn't win anything.\n");
    }
    return 0;
}
