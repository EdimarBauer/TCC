
/*
* Author: Edimar Jacob Bauer
* Email: edimarjb@gmail.com
* Date: October 1, 2017
*
* Update: February 20, 2018
* Update: May 15, 2018
*
* bf is the balance_factor. When bf is 0 the node is equilibrate, when bf is -1 the left is heaviest, when bf is 1 the right 
* is heaviest, when bf is -2 is necessary to do rotation to right and when bf is 2 is necessary to do rotation to left
*/

#include <iostream>
#include <set>
#include <cmath>
using namespace std;
typedef long long ll;

const int T = (1 << 24)+1;
const int MAX = 1 << 30;
int N = (1 << 10);
set<int> mapa;
set<int>::iterator vit[T];
int total, last;

struct Tree{
    int key;
    char bf;
    struct Tree *left, *right;
};

int rando();
long long altura(Tree *tree, int alt);
void clear(Tree *tree);

Tree* newNode(int x);
Tree* search(Tree *tree, int x);

void rotate_left(Tree *&tree);
void rotate_right(Tree *&tree);
bool balance(Tree *&tree);
bool insert(Tree *&tree, int x);

bool replace(Tree *&tree, Tree **raiz);
bool remove(Tree *&tree, int x);

void check_is_correct(Tree *tree);
int check(Tree *tree);
void print(Tree *tree, int i);

int main(){

    Tree* tree = NULL;
    int k = log2(N), x;
    clock_t tInicio, tFim;
    double tempo, tempoRemocao;

    while(N < (1 << 25) ){
        total = 0; tempo = 0; tempoRemocao = 0;
        clear(tree); mapa.clear();
        tree = NULL;

        for (int i = 1; i <= N; ){
            do{
                x = rand() % MAX;
            }while(mapa.count(x) > 0);
            
            for (int j = 0; j < 1000 && mapa.count(x) == 0 && x < MAX && i <= N; j++){
                mapa.insert(x);
                vit[i-1] = mapa.insert(x).first;

                tInicio = clock();
                insert(tree, x);
                tFim = clock();

                x++; i++;
                tempo += ( (double)(tFim - tInicio) / (CLOCKS_PER_SEC / 1000.0));
            }
        }

        ll alt = altura(tree, 0);
        double altMedia = ((double)alt / total);

        int sz = N;
        while (sz){
            x = rand() % sz;

            tInicio = clock();
            remove(tree, *(vit[x]) );
            tFim = clock();

            tempoRemocao += ( (double)(tFim - tInicio) / (CLOCKS_PER_SEC / 1000.0));   

            vit[x] = vit[--sz];
        }

        printf("%d  tempo: %.0lf  alt_media: %.4lf   remocao: %.0lf\n", k++, tempo, altMedia, tempoRemocao);

        N <<= 1;
    }
}


//#########################################################################################################
//#########################################################################################################


long long altura(Tree *tree, int alt){

    if (!tree) return 0;

    long long nos = alt;
    nos += altura(tree->left, alt+1);
    nos += altura(tree->right, alt+1);
    return nos;
}

void clear(Tree *tree){

    if (tree == NULL) return;

    clear(tree->left);
    clear(tree->right);
    free(tree);
}


Tree* newNode(int x){

    Tree *tree = new Tree;
    tree->key = x;
    tree->bf = 0;
    tree->left = tree->right = NULL;
    total++;

    return tree;
}

Tree* search(Tree *tree, int x){

    while(tree != NULL){
        if (tree->key == x) return tree;

        if (x < tree->key) tree = tree->left;
        else tree = tree->right;
    }
    return NULL;
}





void rotate_left(Tree *&tree){

    Tree *aux = tree->right;
    tree->right = aux->left;
    aux->left = tree;

    if (tree->bf == 2){
        //simple rotate
        if (aux->bf == 2){
            //occur after first rotation of a double rotation
            tree->bf = -1;
            aux->bf = 0;
        }else if (aux->bf == 1){
            tree->bf = 0;
            aux->bf = 0;
        }else{
            tree->bf = 1;
            aux->bf = -1;
        }
    }else{
        //double rotate
        if (aux->bf == -1){
            tree->bf = 0;
            aux->bf = -2;
        }else if (aux->bf == 1){
            tree->bf = -1;
            aux->bf = -1;
        }else{  //aux->bf == 0
            tree->bf = 0;
            aux->bf = -1;
        }
    }
    tree = aux;
}

void rotate_right(Tree *&tree){

    Tree *aux = tree->left;
    tree->left = aux->right;
    aux->right = tree;

    if (tree->bf == -2){
        //simple rotate
        if (aux->bf == -2){
            //occur after first rotation of a double rotation
            tree->bf = 1;
            aux->bf = 0;
        }else if (aux->bf == -1){
            tree->bf = 0;
            aux->bf = 0;
        }else{
            tree->bf = -1;
            aux->bf = 1;
        }
    }else{
        //double rotate
        if (aux->bf == 1){
            tree->bf = 0;
            aux->bf = 2;
        }else if (aux->bf == -1){
            tree->bf = 1;
            aux->bf = 1;
        }else{  //aux->bf == 0
            tree->bf = 0;
            aux->bf = 1;
        }
    }
    tree = aux;
}

bool balance(Tree *&tree){

    if (tree->bf == 2){
        if (tree->right->bf < 0)
            rotate_right(tree->right);
        rotate_left(tree);
    }else if (tree->bf == -2){
        if (tree->left->bf > 0)
            rotate_left(tree->left);
        rotate_right(tree);
    }else if (tree->bf != 0)
        return 1;

    return 0;
}

bool insert(Tree *&tree, int x){

    if (tree == NULL){
        tree = newNode(x);
        return 1;
    }

    int b;
    if (x < tree->key) b = -insert(tree->left, x);
    else if (x > tree->key) b = insert(tree->right, x);
    else return 0;

    if (b){
        tree->bf += b;
        return balance(tree);
    }
    return 0;
}





bool replace(Tree *&tree, Tree **raiz){

    int b = 0;
    if (tree->left == NULL){
        *raiz = tree;
        tree = tree->right;
        return 1;
    }else b = -replace(tree->left, raiz);

    if (b){
        tree->bf -= b;
        balance(tree);
        if (tree->bf == 0) return 1;
    }
    return 0;
}

bool remove(Tree *&tree, int x){

    if (tree == NULL) return 0;

    int b = 0;
    if (tree->key == x){
        Tree *aux = tree;
        total--;
        if (tree->left && tree->right) {
            b = replace(tree->right, &tree);
            tree->left = aux->left;
            tree->right = aux->right;
            tree->bf = aux->bf;
            free(aux);
        }else{
            if (tree->left == NULL) tree = tree->right;
            else tree = tree->left;
            free(aux);
            return 1;
        }
    }else if (x < tree->key) b = -remove(tree->left, x);
    else b = remove(tree->right, x);

    if (b){
        tree->bf -= b;
        balance(tree);
        if (tree->bf == 0) return 1;
    }
    return 0;
}





void print(Tree *tree, int i){

    if (tree == NULL) return;

    print(tree->left, i+1);
    printf("key %d  balance: %d  alt: %d\n", tree->key, tree->bf, i);
    print(tree->right, i+1);
}

void check_is_correct(Tree *tree){

    last = -1;
    int to = total;
    check(tree);
    if (total != 0){
        printf("Diference of %d elements\n", total);
        exit(0);
    }
    total = to;
}

int check(Tree *tree){

    if (tree == NULL) return 0;

    int l, r;
    l = check(tree->left);
    total--;
    if (last >= tree->key){
        printf("Err between nodes %d and %d\n", last, tree->key);
        exit(0);
    }
    last = tree->key;
    r = check(tree->right);

    if (abs(l-r) > 1 || (r-l) != tree->bf){
        printf("Err of balance in parent %d\n", tree->key);
        exit(0);
    }

    return max(l, r) + 1;
}

 