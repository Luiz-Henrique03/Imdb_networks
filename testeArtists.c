#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int ID;
    char* name;
    char** movies;
    int numMovies;
} Artist;

typedef struct RBNode {
    Artist* artist;
    int color;
    struct RBNode* left;
    struct RBNode* right;
    struct RBNode* parent;
} RBNode;

typedef struct {
    RBNode* root;
    RBNode* nil;
} RBTree;

RBNode* createNode(Artist* artist, RBNode* left, RBNode* right, RBNode* parent) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    node->artist = artist;
    node->left = left;
    node->right = right;
    node->parent = parent;
    node->color = 0; // Initialize the color to black
    return node;
}

RBTree* createRBTree() {
    RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
    tree->nil = createNode(NULL, NULL, NULL, NULL);
    tree->nil->color = 0;
    tree->root = tree->nil;
    return tree;
}

RBNode* findNode(RBNode* node, int ID) {
    if (node == NULL || node == NULL || node->artist->ID == ID)
        return node;
    if (ID < node->artist->ID)
        return findNode(node->left, ID);
    else
        return findNode(node->right, ID);
}

void leftRotate(RBTree* tree, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != tree->nil)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == tree->nil)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rightRotate(RBTree* tree, RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != tree->nil)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == tree->nil)
        tree->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void insertFixup(RBTree* tree, RBNode* z) {
    while (z->parent->color == 1) {
        if (z->parent == z->parent->parent->left) {
            RBNode* y = z->parent->parent->right;
            if (y->color == 1) {
                z->parent->color = 0;
                y->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(tree, z);
                }
                z->parent->color = 0;
                z->parent->parent->color = 1;
                rightRotate(tree, z->parent->parent);
            }
        }
        else {
            RBNode* y = z->parent->parent->left;
            if (y->color == 1) {
                z->parent->color = 0;
                y->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(tree, z);
                }
                z->parent->color = 0;
                z->parent->parent->color = 1;
                leftRotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}

void insertArtist(RBTree* tree, Artist* artist) {
    RBNode* y = tree->nil;
    RBNode* x = tree->root;
    while (x != tree->nil) {
        y = x;
        if (artist->ID < x->artist->ID)
            x = x->left;
        else
            x = x->right;
    }
    RBNode* z = createNode(artist, tree->nil, tree->nil, y);
    if (y == tree->nil)
        tree->root = z;
    else if (artist->ID < y->artist->ID)
        y->left = z;
    else
        y->right = z;
    z->color = 1;
    insertFixup(tree, z);
}

RBNode* treeMinimum(RBNode* node, RBNode* nil) {
    while (node->left != nil)
        node = node->left;
    return node;
}

void transplant(RBTree* tree, RBNode* u, RBNode* v) {
    if (u->parent == tree->nil)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

void deleteFixup(RBTree* tree, RBNode* x) {
    while (x != tree->root && x->color == 0) {
        if (x == x->parent->left) {
            RBNode* w = x->parent->right;
            if (w->color == 1) {
                w->color = 0;
                x->parent->color = 1;
                leftRotate(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == 0 && w->right->color == 0) {
                w->color = 1;
                x = x->parent;
            }
            else {
                if (w->right->color == 0) {
                    w->left->color = 0;
                    w->color = 1;
                    rightRotate(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                leftRotate(tree, x->parent);
                x = tree->root;
            }
        }
        else {
            RBNode* w = x->parent->left;
            if (w->color == 1) {
                w->color = 0;
                x->parent->color = 1;
                rightRotate(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == 0 && w->left->color == 0) {
                w->color = 1;
                x = x->parent;
            }
            else {
                if (w->left->color == 0) {
                    w->right->color = 0;
                    w->color = 1;
                    leftRotate(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                rightRotate(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = 0;
}


void deleteArtist(RBTree* tree, int ID) {
    RBNode* z = findNode(tree->root, ID);
    if (z == tree->nil)
        return;
    RBNode* y = z;
    RBNode* x;
    int yOriginalColor = y->color;
    if (z->left == tree->nil) {
        x = z->right;
        transplant(tree, z, z->right);
    }
    else if (z->right == tree->nil) {
        x = z->left;
        transplant(tree, z, z->left);
    }
    else {
        y = treeMinimum(z->right, tree->nil);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z)
            x->parent = y;
        else {
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    free(z->artist->name);
    for (int i = 0; i < z->artist->numMovies; i++)
        free(z->artist->movies[i]);
    free(z->artist->movies);
    free(z->artist);
    free(z);
    if (yOriginalColor == 0)
        deleteFixup(tree, x);
}

void printArtist(Artist* artist) {
    printf("ID: %d\n", artist->ID);
    printf("Name: %s\n", artist->name);
    printf("Movies:\n");
    for (int i = 0; i < artist->numMovies; i++)
        printf("- %s\n", artist->movies[i]);
    printf("\n");
}

void inorderTraversal(RBNode* node, RBNode* nil) {
    if (node != nil) {
        inorderTraversal(node->left, nil);
        printArtist(node->artist);
        inorderTraversal(node->right, nil);
    }
}

int main() {
    RBTree* tree = createRBTree();

    // Create and insert artists
    Artist* artist1 = (Artist*)malloc(sizeof(Artist));
    artist1->ID = 1;
    artist1->name = strdup("Leonardo DiCaprio");
    artist1->numMovies = 4;
    artist1->movies = (char**)malloc(artist1->numMovies * sizeof(char*));
    artist1->movies[0] = strdup("Titanic");
    artist1->movies[1] = strdup("Inception");
    artist1->movies[2] = strdup("The Revenant");
    artist1->movies[3] = strdup("The Departed");
    insertArtist(tree, artist1);

    Artist* artist2 = (Artist*)malloc(sizeof(Artist));
    artist2->ID = 2;
    artist2->name = strdup("Tom Hanks");
    artist2->numMovies = 5;
    artist2->movies = (char**)malloc(artist2->numMovies * sizeof(char*));
    artist2->movies[0] = strdup("Forrest Gump");
    artist2->movies[1] = strdup("Cast Away");
    artist2->movies[2] = strdup("Saving Private Ryan");
    artist2->movies[3] = strdup("The Green Mile");
    artist2->movies[4] = strdup("Apollo 13");
    insertArtist(tree, artist2);

    Artist* artist3 = (Artist*)malloc(sizeof(Artist));
    artist3->ID = 3;
    artist3->name = strdup("Meryl Streep");
    artist3->numMovies = 3;
    artist3->movies = (char**)malloc(artist3->numMovies * sizeof(char*));
    artist3->movies[0] = strdup("The Devil Wears Prada");
    artist3->movies[1] = strdup("Mamma Mia!");
    artist3->movies[2] = strdup("Doubt");
    insertArtist(tree, artist3);

    // Print all artists
    printf("All Artists:\n");
    inorderTraversal(tree->root, tree->nil);

    // Delete an artist
    int artistID = 2;
    printf("Deleting artist with ID %d\n", artistID);
    deleteArtist(tree, artistID);

    // Print all artists after deletion
    printf("All Artists after deletion:\n");
    inorderTraversal(tree->root, tree->nil);

    // Clean up memory
    free(tree->nil);
    free(tree);

    return 0;
}
