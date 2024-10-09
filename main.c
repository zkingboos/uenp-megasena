#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_GAMES 3
#define MAX_NUMBERS 15
#define MAX_RANDOM_GAMES 7
#define MIN_NUMBERS 6
#define DRAW_SIZE 6

typedef struct {
    int numbers[MAX_NUMBERS];
    int quantity;
    bool random;
} Game;

typedef struct {
    Game games[MAX_GAMES + MAX_RANDOM_GAMES];
    int gameCount;
} Ticket;

bool numberAlreadyExists(Game game, int number) {
    for (int index = 0; index < game.quantity; ++index) {
        if (game.numbers[index] == number) {
            return true;
        }
    }

    return false;
}

void orderGameNumbers(Game *game) {
    for (int i = 0; i < game->quantity - 1; i++) {
        for (int j = 0; j < game->quantity - 1 - i; j++) {
            if (game->numbers[j] > game->numbers[j + 1]) {
                int temp = game->numbers[j];
                game->numbers[j] = game->numbers[j + 1];
                game->numbers[j + 1] = temp;
            }
        }
    }
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

    do {
        for (int index = 0; index < MIN_NUMBERS; ++index) {
            newGame.numbers[index] = rand() % 60 + 1;
        }
        orderGameNumbers(&newGame);
    } while (!createNewGame(ticket, newGame));
}

int countMatches(Game game, int draw[], int drawSize) {
    int matches = 0;
    int xIndex = 0;
    int yIndex = 0;

    while (xIndex < game.quantity && yIndex < drawSize) {
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
    do {
        int number;
        printf("Game(%d) #%d: Enter %d numbers for you game (1-60): ", gameNumber, index + 1, numberQuantity);
        scanf("%d", &number);

        if (number < 1 || number > 60) {
            printf("The number needs to be 1-60. Try again.\n");
            continue;
        }

        if (numberAlreadyExists(newUserGame, number)) {
            printf("This number was used before in the same game, please specify a new number.\n");
            continue;
        }

        newUserGame.numbers[index] = number;
        index++;
    } while (index < numberQuantity);

    orderGameNumbers(&newUserGame);
    return newUserGame;
}

void generateRandomDraw(int *draw, int drawSize) {
    for (int index = 0; index < drawSize; ++index) {
        draw[index] = rand() % 60 + 1;
    }
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

    int draw[DRAW_SIZE] = {};
    printf("\tGenerating draw...\n");
    generateRandomDraw(draw, DRAW_SIZE);

    printf("\tRegistered total of games: %d\n", ticket.gameCount);

    int totalMatches = 0;
    for (int i = 0; i < ticket.gameCount; ++i) {
        Game userGame = ticket.games[i];
        int matches = countMatches(userGame, draw, DRAW_SIZE);

        if (userGame.random) {
            printf("\t\t[Game %d Random] You have %d matches.\n", i + 1, matches);
        } else {
            printf("\t\t[Game %d] You have %d matches.\n", i + 1, matches);
        }

        totalMatches += matches;
    }

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
