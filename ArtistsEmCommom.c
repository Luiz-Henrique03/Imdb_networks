#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000

struct Artist {
    int ID;
    char* nome;
    char** movies;
    int numMovies;
};

typedef struct Artist Artist;

struct Graph {
    int numVertices;
    int** adjMatrix;
};

typedef struct Graph Graph;

void initializeGraph(Graph* graph, int numVertices) {
    graph->numVertices = numVertices;
    graph->adjMatrix = (int**)malloc(numVertices * sizeof(int*));
    for (int i = 0; i < numVertices; i++) {
        graph->adjMatrix[i] = (int*)malloc(numVertices * sizeof(int));
        memset(graph->adjMatrix[i], 0, numVertices * sizeof(int));
    }
}

void addEdge(Graph* graph, int vertex1, int vertex2) {
    graph->adjMatrix[vertex1][vertex2] = 1;
    graph->adjMatrix[vertex2][vertex1] = 1;
}

void printGraph(Graph* graph, Artist* artists, int numArtists) {
    for (int i = 0; i < numArtists; i++) {
        printf("Artista: %s\nFilmes em comum com outros artistas:\n", artists[i].nome);
        for (int j = 0; j < numArtists; j++) {
            if (graph->adjMatrix[i][j] == 1 && i != j) {
                printf("- %s\n", artists[j].nome);
            }
        }
        printf("\n");
    }
}

void readArtistFile(Artist** artists, int* numArtists, char* path) {
    FILE* arquivo;
    char line[MAX_LINE_LENGTH];
    int lineLength;
    int skipHeader = 1;

    arquivo = fopen(path, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    int cont = 1000;

    while (fgets(line, MAX_LINE_LENGTH, arquivo) != NULL && cont--) {
        if (skipHeader) {
            skipHeader = 0;
            continue; // Pula para a próxima iteração para ignorar o cabeçalho
        }

        lineLength = strlen(line);

        *artists = realloc(*artists, (*numArtists + 1) * sizeof(Artist));
        if (*artists == NULL) {
            printf("Erro ao alocar memória.\n");
            return;
        }

        Artist* currentArtist = &((*artists)[*numArtists]);

        currentArtist->nome = malloc((lineLength + 1) * sizeof(char));
        if (currentArtist->nome == NULL) {
            printf("Erro ao alocar memória.\n");
            return;
        }

        currentArtist->numMovies = 0;
        currentArtist->movies = NULL;

        char* token = strtok(line, "\t"); // Tabulação como separador

        int fieldIndex = 0;
        while (token != NULL) {
            if (fieldIndex == 0) {
                sscanf(token, "nm%*06d", &(currentArtist->ID));
            }
            else if (fieldIndex == 1) {
                strcpy(currentArtist->nome, token);
            }
            else if (fieldIndex == 5) {
                char* movieToken = strtok(token, ",");
                while (movieToken != NULL) {
                    currentArtist->movies = realloc(currentArtist->movies, (currentArtist->numMovies + 1) * sizeof(char*));
                    if (currentArtist->movies == NULL) {
                        printf("Erro ao alocar memória.\n");
                        return;
                    }

                    currentArtist->movies[currentArtist->numMovies] = malloc(strlen(movieToken) * sizeof(char));
                    if (currentArtist->movies[currentArtist->numMovies] == NULL) {
                        printf("Erro ao alocar memória.\n");
                        return;
                    }

                    strcpy(currentArtist->movies[currentArtist->numMovies], movieToken);
                    currentArtist->numMovies++;

                    movieToken = strtok(NULL, ",");
                }
            }

            fieldIndex++;
            token = strtok(NULL, "\t");
        }

        (*numArtists)++;
    }

    fclose(arquivo);
}

void createGraph(Graph* graph, Artist* artists, int numArtists) {
    initializeGraph(graph, numArtists);

    for (int i = 0; i < numArtists; i++) {
        for (int j = i + 1; j < numArtists; j++) {
            Artist artist1 = artists[i];
            Artist artist2 = artists[j];

            for (int k = 0; k < artist1.numMovies; k++) {
                for (int l = 0; l < artist2.numMovies; l++) {
                    if (strcmp(artist1.movies[k], artist2.movies[l]) == 0) {
                        addEdge(graph, i, j);
                        break;
                    }
                }
            }
        }
    }
}

void freeMemory(Artist* artists, int numArtists, Graph* graph) {
    for (int i = 0; i < numArtists; i++) {
        Artist artist = artists[i];

        free(artist.nome);

        for (int j = 0; j < artist.numMovies; j++) {
            free(artist.movies[j]);
        }

        free(artist.movies);
    }

    for (int i = 0; i < graph->numVertices; i++) {
        free(graph->adjMatrix[i]);
    }

    free(graph->adjMatrix);

    free(artists);
}

int main() {
    Artist* artists = NULL;
    int numArtists = 0;
    Graph graph;

    readArtistFile(&artists, &numArtists, "artists.tsv");
    createGraph(&graph, artists, numArtists);
    printGraph(&graph, artists, numArtists);
    freeMemory(artists, numArtists, &graph);

    return 0;
}
