#include "Arvore_RBT.h"
#include <string.h>

int main() {
    // Example usage
    RBTree* tree = createRBTree();

    Movie* movie1 = (Movie*)malloc(sizeof(Movie));
    movie1->titleId = strdup("tt0000001");
    movie1->titleType = strdup("movie");
    movie1->primaryTitle = strdup("Carmencita");
    movie1->originalTitle = strdup("Carmencita");
    movie1->isAdult = 0;
    movie1->startYear = 1894;
    movie1->endYear = 0;
    movie1->runtimeMinutes = 1;
    movie1->genres = strdup("Documentary,Short");
    movie1->artists = NULL;
    movie1->numArtists = 0;

    insert_movie(tree, "tt0000001", movie1);

    Movie* movie2 = (Movie*)malloc(sizeof(Movie));
    movie2->titleId = strdup("tt0000002");
    movie2->titleType = strdup("movie");
    movie2->primaryTitle = strdup("Le clown et ses chiens");
    movie2->originalTitle = strdup("Le clown et ses chiens");
    movie2->isAdult = 0;
    movie2->startYear = 1892;
    movie2->endYear = 0;
    movie2->runtimeMinutes = 5;
    movie2->genres = strdup("Animation,Short");
    movie2->artists = NULL;
    movie2->numArtists = 0;

    insert_movie(tree, "tt0000002", movie2);

    /*
   Artist* artist1 = (Artist*)malloc(sizeof(Artist));
    artist1->ID = 1;
    artist1->name = strdup("Charles-Émile Reynaud");
    artist1->movies = (char**)malloc(sizeof(char*));
    artist1->movies[0] = strdup("tt0000002");
    artist1->numMovies = 1;
   
    insert_artist(tree, "Charles-Émile Reynaud", artist1);
     */
    Movie* foundMovie = search_movie(tree, "tt0000001");
    if (foundMovie != NULL) {
        printf("Found Movie: %s\n", foundMovie->primaryTitle);
    }
    /*
    Artist* foundArtist = search_artist(tree, "Charles-Émile Reynaud");
    if (foundArtist != NULL) {
        printf("Found Artist: %s\n", foundArtist->name);
        printf("Movies by Artist:\n");
        for (int i = 0; i < foundArtist->numMovies; i++) {
            printf("%s\n", foundArtist->movies[i]);
        }
    }
    
    delete_movie(tree, "tt0000001");

    printf("After deletion:\n");
    foundMovie = search_movie(tree, "tt0000001");
    if (foundMovie == NULL) {
        printf("Movie with ID tt0000001 not found.\n");
    }

    return 0;
}
