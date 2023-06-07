#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char* name;
    int numMovies;
    char** movies;
} Artist;

typedef struct {
    int id;
    char* name;
    int numConnections;
    int* connections;
} Movie;

typedef struct {
    Artist** artists;
    int numArtists;
    Movie** movies;
    int numMovies;
} Graph;

typedef struct node {
    Artist* artist;
    struct node* left;
    struct node* right;
} Node;

Graph* graph = NULL;
Node* root = NULL;

Node* createNode(Artist* artist) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->artist = artist;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void insertNode(Node* root, Node* newNode) {
    if (newNode->artist->id < root->artist->id) {
        if (root->left == NULL)
            root->left = newNode;
        else
            insertNode(root->left, newNode);
    }
    else if (newNode->artist->id > root->artist->id) {
        if (root->right == NULL)
            root->right = newNode;
        else
            insertNode(root->right, newNode);
    }
}

void readArtistsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    graph = (Graph*)malloc(sizeof(Graph));
    graph->artists = NULL;
    graph->numArtists = 0;

    printf("Lendo arquivo...\n");

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, "\t");
        if (token == NULL) {
            return;
        }

        Artist* artist = (Artist*)malloc(sizeof(Artist));
        if (artist == NULL) {
            return;
        }
        artist->id = atoi(token);

        token = strtok(NULL, "\t");
        artist->name = strdup(token);

        artist->numMovies = 0;
        artist->movies = NULL;

        Node* artistNode = createNode(artist);
        if (root == NULL)
            root = artistNode;
        else
            insertNode(root, artistNode);

        graph->numArtists++;
        graph->artists = (Artist**)realloc(graph->artists, sizeof(Artist*) * graph->numArtists);
        graph->artists[graph->numArtists - 1] = artist;

        token = strtok(NULL, "\t"); // Discard the rest of the tokens
    }

    fclose(file);
}

void readMoviesFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    graph->movies = NULL;
    graph->numMovies = 0;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, "\t");

        Movie* movie = (Movie*)malloc(sizeof(Movie));
        movie->id = atoi(token);

        token = strtok(NULL, "\t");
        movie->name = strdup(token);

        token = strtok(NULL, "\t");

        // Skip the next 7 tokens, as they are not needed for now
        for (int i = 0; i < 7; i++)
            token = strtok(NULL, "\t");

        token = strtok(NULL, "\t");

        // Known For Titles
        if (strcmp(token, "\\N") != 0) {
            char* artistIdToken = strtok(token, ",");
            while (artistIdToken != NULL) {
                int artistId = atoi(artistIdToken);
                for (int i = 0; i < graph->numArtists; i++) {
                    if (graph->artists[i]->id == artistId) {
                        graph->artists[i]->numMovies++;
                        graph->artists[i]->movies = (char**)realloc(graph->artists[i]->movies, sizeof(char*) * graph->artists[i]->numMovies);
                        graph->artists[i]->movies[graph->artists[i]->numMovies - 1] = strdup(movie->name);
                        break;
                    }
                }
                artistIdToken = strtok(NULL, ",");
            }
        }

        graph->numMovies++;
        graph->movies = (Movie**)realloc(graph->movies, sizeof(Movie*) * graph->numMovies);
        graph->movies[graph->numMovies - 1] = movie;
    }

    fclose(file);
}

void displayMoviesByArtist(int artistId) {
    Artist* artist = NULL;

    // Search for the artist in the BST
    Node* current = root;
    while (current != NULL) {
        if (artistId < current->artist->id)
            current = current->left;
        else if (artistId > current->artist->id)
            current = current->right;
        else {
            artist = current->artist;
            break;
        }
    }

    if (artist == NULL) {
        printf("Artist with ID %d not found.\n", artistId);
        return;
    }

    printf("Movies for Artist %d - %s:\n", artist->id, artist->name);
    for (int i = 0; i < artist->numMovies; i++) {
        printf("- %s\n", artist->movies[i]);
    }
}

void cleanup() {
    if (graph != NULL) {
        if (graph->artists != NULL) {
            for (int i = 0; i < graph->numArtists; i++) {
                Artist* artist = graph->artists[i];
                free(artist->name);
                if (artist->movies != NULL) {
                    for (int j = 0; j < artist->numMovies; j++) {
                        free(artist->movies[j]);
                    }
                    free(artist->movies);
                }
                free(artist);
            }
            free(graph->artists);
        }
        if (graph->movies != NULL) {
            for (int i = 0; i < graph->numMovies; i++) {
                Movie* movie = graph->movies[i];
                free(movie->name);
                free(movie);
            }
            free(graph->movies);
        }
        free(graph);
    }

    // Free the BST
    while (root != NULL) {
        Node* current = root;
        root = root->left;
        free(current);
    }
}

int main() {
    const char* artistsFile = "artists.tsv";
    const char* moviesFile = "movies.tsv";

    readArtistsFromFile(artistsFile);
    readMoviesFromFile(moviesFile);

    int artistId = 123; // Replace with the desired artist ID
    displayMoviesByArtist(artistId);

    cleanup();

    return 0;
}

