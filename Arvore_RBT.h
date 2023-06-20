#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define MAX_TITLE_LENGTH 100
#define MAX_GENRES_LENGTH 100

typedef struct {
    char* titleId;
    char* titleType;
    char* primaryTitle;
    char* originalTitle;
    int isAdult;
    int startYear;
    int endYear;
    int runtimeMinutes;
    char* genres;
    int* artists; // Array of artist IDs associated with the movie
    int numArtists; // Number of artists associated with the movie
} Movie;

typedef struct {
    int ID;
    char* name;
    char** movies;
    int numMovies;
} Artist;

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    char* key;
    Color color;
    struct RBNode* parent;
    struct RBNode* left;
    struct RBNode* right;
    Movie* movie;
    Artist* artist;
} RBNode;

typedef struct {
    RBNode* root;
} RBTree;

RBNode* createNode(char* key, Movie* movie, Artist* artist) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    node->key = strdup(key);
    node->color = RED;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    node->movie = movie;
    node->artist = artist;
    return node;
}

RBTree* createRBTree() {
    RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
    tree->root = NULL;
    return tree;
}

RBNode* grandparent(RBNode* node) {
    if (node == NULL || node->parent == NULL)
        return NULL;
    return node->parent->parent;
}

RBNode* uncle(RBNode* node) {
    RBNode* g = grandparent(node);
    if (g == NULL)
        return NULL;
    if (node->parent == g->left)
        return g->right;
    else
        return g->left;
}

void rotate_left(RBTree* tree, RBNode* node) {
    RBNode* pivot = node->right;
    node->right = pivot->left;
    if (pivot->left != NULL)
        pivot->left->parent = node;
    pivot->parent = node->parent;
    if (node->parent == NULL)
        tree->root = pivot;
    else if (node == node->parent->left)
        node->parent->left = pivot;
    else
        node->parent->right = pivot;
    pivot->left = node;
    node->parent = pivot;
}

void rotate_right(RBTree* tree, RBNode* node) {
    RBNode* pivot = node->left;
    node->left = pivot->right;
    if (pivot->right != NULL)
        pivot->right->parent = node;
    pivot->parent = node->parent;
    if (node->parent == NULL)
        tree->root = pivot;
    else if (node == node->parent->left)
        node->parent->left = pivot;
    else
        node->parent->right = pivot;
    pivot->right = node;
    node->parent = pivot;
}

void insert_fixup(RBTree* tree, RBNode* node) {
    while (node != tree->root && node->parent->color == RED) {
        RBNode* u = uncle(node);
        RBNode* g = grandparent(node);
        if (u != NULL && u->color == RED) {
            node->parent->color = BLACK;
            u->color = BLACK;
            g->color = RED;
            node = g;
        }
        else {
            if (node->parent == g->left) {
                if (node == node->parent->right) {
                    node = node->parent;
                    rotate_left(tree, node);
                }
                node->parent->color = BLACK;
                g->color = RED;
                rotate_right(tree, g);
            }
            else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotate_right(tree, node);
                }
                node->parent->color = BLACK;
                g->color = RED;
                rotate_left(tree, g);
            }
        }
    }
    tree->root->color = BLACK;
}

void insert_movie(RBTree* tree, char* key, Movie* movie) {
    RBNode* node = createNode(key, movie, NULL);
    if (tree->root == NULL) {
        tree->root = node;
        node->color = BLACK;
    }
    else {
        RBNode* current = tree->root;
        RBNode* parent = NULL;
        while (current != NULL) {
            parent = current;
            int cmp = strcmp(key, current->key);
            if (cmp == 0) {
                free(node);
                return; // Movie with the same key already exists
            }
            else if (cmp < 0)
                current = current->left;
            else
                current = current->right;
        }
        node->parent = parent;
        int cmp = strcmp(key, parent->key);
        if (cmp < 0)
            parent->left = node;
        else
            parent->right = node;
        insert_fixup(tree, node);
    }
}



RBNode* find_node(RBNode* node, char* key) {
    if (node == NULL || strcmp(node->key, key) == 0)
        return node;
    if (strcmp(key, node->key) < 0)
        return find_node(node->left, key);
    else
        return find_node(node->right, key);
}

Movie* search_movie(RBTree* tree, char* key) {
    RBNode* node = find_node(tree->root, key);
    if (node != NULL && node->movie != NULL)
        return node->movie;
    else
        return NULL;
}



void transplant(RBTree* tree, RBNode* u, RBNode* v) {
    if (u->parent == NULL)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v != NULL)
        v->parent = u->parent;
}

RBNode* minimum_node(RBNode* node) {
    while (node->left != NULL)
        node = node->left;
    return node;
}

void delete_fixup(RBTree* tree, RBNode* x) {
    while (x != tree->root && (x == NULL || x->color == BLACK)) {
        if (x == x->parent->left) {
            RBNode* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotate_left(tree, x->parent);
                w = x->parent->right;
            }
            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left != NULL)
                        w->left->color = BLACK;
                    w->color = RED;
                    rotate_right(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->right != NULL)
                    w->right->color = BLACK;
                rotate_left(tree, x->parent);
                x = tree->root;
            }
        }
        else {
            RBNode* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotate_right(tree, x->parent);
                w = x->parent->left;
            }
            if ((w->right == NULL || w->right->color == BLACK) &&
                (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right != NULL)
                        w->right->color = BLACK;
                    w->color = RED;
                    rotate_left(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->left != NULL)
                    w->left->color = BLACK;
                rotate_right(tree, x->parent);
                x = tree->root;
            }
        }
    }
    if (x != NULL)
        x->color = BLACK;
}

void delete_node(RBTree* tree, RBNode* z) {
    RBNode* y = z;
    RBNode* x = NULL;
    Color y_original_color = y->color;

    if (z->left == NULL) {
        x = z->right;
        transplant(tree, z, z->right);
    }
    else if (z->right == NULL) {
        x = z->left;
        transplant(tree, z, z->left);
    }
    else {
        y = minimum_node(z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent != z) {
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == BLACK)
        delete_fixup(tree, x);

    free(z->key);

    if (z->movie != NULL) {
        free(z->movie->titleId);
        free(z->movie->titleType);
        free(z->movie->primaryTitle);
        free(z->movie->originalTitle);
        free(z->movie->genres);
        free(z->movie->artists);
        free(z->movie);
    }
    else if (z->artist != NULL) {
        free(z->artist->name);
        for (int i = 0; i < z->artist->numMovies; i++)
            free(z->artist->movies[i]);
        free(z->artist->movies);
        free(z->artist);
    }

    free(z);
}


void delete_movie(RBTree* tree, char* key) {
   
    RBNode* node = find_node(tree->root, key);
    if (node != NULL)
        delete_node(tree, node);
}

void delete_artist(RBTree* tree, char* key) {
    RBNode* node = find_node(tree->root, key);
    if (node != NULL && node->artist != NULL)
        delete_node(tree, node);
}

void free_movies(RBNode* node) {
    if (node == NULL)
        return;
    free_movies(node->left);
    free_movies(node->right);
    if (node->movie != NULL) {
        free(node->movie->titleId);
        free(node->movie->titleType);
        free(node->movie->primaryTitle);
        free(node->movie->originalTitle);
        free(node->movie->genres);
        free(node->movie->artists);
        free(node->movie);
    }
    free(node->key);
    free(node);
}

void free_artists(RBNode* node) {
    if (node == NULL)
        return;
    free_artists(node->left);
    free_artists(node->right);
    if (node->artist != NULL) {
        free(node->artist->name);
        for (int i = 0; i < node->artist->numMovies; i++)
            free(node->artist->movies[i]);
        free(node->artist->movies);
        free(node->artist);
    }
    free(node->key);
    free(node);
}

void free_RBTree(RBTree* tree) {
    if (tree == NULL)
        return;
    free_movies(tree->root);
    free(tree);
}

