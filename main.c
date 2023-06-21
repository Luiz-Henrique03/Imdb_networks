#include "Arvore_RBT.h"


typedef struct {
    int* adjMatrix;
    int numVertices;
} Graph;

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
            continue;
        }

        lineLength = strlen(line);

        *artists = realloc(*artists, (*numArtists + 1) * sizeof(Artist));
        if (*artists == NULL) {
            printf("Erro ao alocar memória.\n");
            return;
        }

        Artist* currentArtist = &((*artists)[*numArtists]);

        currentArtist->name = malloc((lineLength + 1) * sizeof(char));
        if (currentArtist->name == NULL) {
            printf("Erro ao alocar memória.\n");
            return;
        }

        currentArtist->numMovies = 0;
        currentArtist->movies = NULL;

        char* token = strtok(line, "\t");

        int fieldIndex = 0;
        while (token != NULL) {
            if (fieldIndex == 0) {
                sscanf(token, "nm%*06d", &(currentArtist->ID));
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

void readMovieFile(Movie** movies, int* numMovies, Artist* artists, int numArtists, char* path) {
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
            continue;
        }

        lineLength = strlen(line);

        *movies = realloc(*movies, (*numMovies + 1) * sizeof(Movie));
        if (*movies == NULL) {
            printf("Erro ao alocar memória.\n");
            return;
        }

        Movie* currentMovie = &((*movies)[*numMovies]);

        currentMovie->titleId = malloc((lineLength + 1) * sizeof(char));
        currentMovie->titleType = malloc((lineLength + 1) * sizeof(char));
        currentMovie->primaryTitle = malloc((lineLength + 1) * sizeof(char));
        currentMovie->originalTitle = malloc((lineLength + 1) * sizeof(char));
        currentMovie->genres = malloc((lineLength + 1) * sizeof(char));

        if (currentMovie->titleId == NULL || currentMovie->titleType == NULL ||
            currentMovie->primaryTitle == NULL || currentMovie->originalTitle == NULL ||
            currentMovie->genres == NULL) {
            printf("Erro ao alocar memória.\n");
            return;
        }

        currentMovie->artists = malloc(numArtists * sizeof(int));
        if (currentMovie->artists == NULL) {
            printf("Erro ao alocar memória.\n");
            return;
        }

        char* token = strtok(line, "\t");

        int fieldIndex = 0;
        while (token != NULL) {
            switch (fieldIndex) {
            case 0:
                strcpy(currentMovie->titleId, token);
                break;
            case 1:
                strcpy(currentMovie->titleType, token);
                break;
            case 2:
                strcpy(currentMovie->primaryTitle, token);
                break;
            case 3:
                strcpy(currentMovie->originalTitle, token);
                break;
            case 4:
                sscanf(token, "%d", &(currentMovie->isAdult));
                break;
            case 5:
                sscanf(token, "%d", &(currentMovie->startYear));
                break;
            case 6:
                sscanf(token, "%d", &(currentMovie->endYear));
                break;
            case 7:
                sscanf(token, "%d", &(currentMovie->runtimeMinutes));
                break;
            case 8:
                strcpy(currentMovie->genres, token);
                break;
            case 9: // Modified case for artist IDs
               // Tokenize artist IDs
                char* artistToken = strtok(token, ",");
                while (artistToken != NULL) {
                    // Convert artist ID to integer
                    int artistID = atoi(artistToken);

                    // Find the artist with the corresponding ID
                    for (int i = 0; i < numArtists; i++) {
                        if (artists[i].ID == artistID) {
                            // Add the movie index to the artist's movie list
                            artists[i].movies = realloc(artists[i].movies, (artists[i].numMovies + 1) * sizeof(char*));
                            artists[i].movies[artists[i].numMovies] = currentMovie->titleId;
                            artists[i].numMovies++;
                            break;
                        }
                    }

                    artistToken = strtok(NULL, ",");
                }
                break;
            default:
                int artistID;
                sscanf(token, "nm%*06d", &artistID);

                for (int i = 0; i < numArtists; i++) {
                    if (artists[i].ID == artistID) {
                        currentMovie->artists[currentMovie->numArtists] = i;
                        currentMovie->numArtists++;
                        break;
                    }
                }
                break;
            }

            fieldIndex++;
            token = strtok(NULL, "\t");
        }

        (*numMovies)++;
    }

    fclose(arquivo);
}

void addEdge(Graph* graph, Movie* movies, int movieIndex1, int movieIndex2) {
    if (graph == NULL || movieIndex1 < 0 || movieIndex2 < 0 ||
        movieIndex1 >= graph->numVertices || movieIndex2 >= graph->numVertices) {
        return;
    }

    graph->adjMatrix[movieIndex1 * graph->numVertices + movieIndex2] = 1;
    graph->adjMatrix[movieIndex2 * graph->numVertices + movieIndex1] = 1;
}

Graph createGraph(Movie* movies, int numMovies) {
    Graph graph;
    graph.numVertices = numMovies;
    graph.adjMatrix = calloc(numMovies * numMovies, sizeof(int));
    if (graph.adjMatrix == NULL) {
        printf("Erro ao alocar memória.\n");
        return graph;
    }

    for (int i = 0; i < numMovies; i++) {
        for (int j = i + 1; j < numMovies; j++) {
            Movie movie1 = movies[i];
            Movie movie2 = movies[j];

            if (movie1.startYear == movie2.startYear && strcmp(movie1.genres, movie2.genres) == 0) {
                addEdge(&graph, movies, i, j);
            }
        }
    }

    return graph;
}

void printGraph(Graph* graph, Movie* movies) {
    if (graph == NULL || movies == NULL) {
        return;
    }

    FILE* file = fopen("graph.dot", "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo graph.dot\n");
        return;
    }

    fprintf(file, "graph G {\n");

    for (int i = 0; i < graph->numVertices; i++) {
        Movie movie = movies[i];

        fprintf(file, "\t%d [label=\"%s (%d)\"];\n", i, movie.originalTitle, movie.startYear);

        for (int j = 0; j < graph->numVertices; j++) {
            if (graph->adjMatrix[i * graph->numVertices + j] == 1) {
                Movie relatedMovie = movies[j];
                fprintf(file, "\t%d -- %d;\n", i, j);
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);

    printf("O arquivo graph.dot foi gerado com sucesso.\n");
}

int main() {
    Artist* artists = NULL;
    int numArtists = 0;
    Movie* movies = NULL;
    RBTree* tree = createRBTree();
    int numMovies = 0;

    readArtistFile(&artists, &numArtists, "artists.tsv");
    readMovieFile(&movies, &numMovies, artists, numArtists, "movies.tsv");

    for (int i = 0; i < numMovies; i++) {
        Movie movie = movies[i];
        insert_movie(tree, movie.titleId, movies);
    }



    Graph graph = createGraph(movies, numMovies);

    printGraph(&graph, movies);

    // Liberar memória alocada
    for (int i = 0; i < numArtists; i++) {
        free(artists[i].name);
        for (int j = 0; j < artists[i].numMovies; j++) {
            free(artists[i].movies[j]);
        }
        free(artists[i].movies);
    }
    free(artists);

    for (int i = 0; i < numMovies; i++) {
        Movie movie = movies[i];
        free(movie.titleId);
        free(movie.titleType);
        free(movie.primaryTitle);
        free(movie.originalTitle);
        free(movie.genres);
        free(movie.artists);
    }
    free(movies);

    free(graph.adjMatrix);

    return 0;
}