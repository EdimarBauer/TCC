/*
* Author: Edimar Jacob Bauer
* Email: edimarjb@gmail.com
* Date: October 31, 2018
*/
#include <bits/stdc++.h>
using namespace std;
typedef unsigned int ui;
typedef long long ll;

const int T = (1 << 24)+1;
const int MAX = 1 << 30;
int N = (1 << 3);
set<int> mapa;
set<int>::iterator vit[T];

int Ref, Universe;
int last, total;

struct Tree{
    int key;
    char miss;
    struct Tree *left, *right;
};

void clear(Tree *tree);
ll altura(Tree *tree, int alt);

Tree* newNode(int x);
Tree* makeHead(Tree* child, int x);
void createMiddleNode(Tree *tree, int x, int cont);
bool updateRef(Tree *tree, int x, int &ref, int &jump);
char updateMiss(int x, int ref, int jump);
void insert(Tree *&root, int x);

Tree* search(Tree *tree, int x);
Tree* replace(Tree **tree);
void remove(Tree *&root, int x);

void print(Tree *tree, int alt);
void check_is_correct(Tree *tree);
void check(Tree *tree);


int main(){

    Tree* tree = NULL;
    int k = log2(N), x;
    clock_t tInicio, tFim;
    double tempo, tempoRemocao;

    while(N < (1 << 5)){
        total = 0; tempo = 0; tempoRemocao = 0;
        clear(tree); mapa.clear();
        tree = NULL;

        for (int i = 0; i < N; i++){
            do{
                x = rand() % (N << 1);
            }while(mapa.count(x) > 0);
            vit[i] = mapa.insert(x).first;

            //printf("\nInserindo %d...\n", x);

            tInicio = clock();
            insert(tree, x);
            tFim = clock();

            //print(tree, 0);
            //check_is_correct(tree);

            tempo += ( (double)(tFim - tInicio) / (CLOCKS_PER_SEC / 1000.0));
        }

        ll alt = altura(tree, 0);
        double altMedia = ((double)alt / total);

        int sz = N, cont = N;
        while (cont--){
            
            //printf("\nRemovendo %d...\n", *(vit[x]));
            int inserir = 4;
            if (inserir < 3){
                do{
                    x = rand() % (N << 1);
                }while(mapa.count(x) > 0);
                vit[sz++] = mapa.insert(x).first;

                //printf("Inserindo %d\n", x);
                tInicio = clock();
                insert(tree, x);
                tFim = clock();
            }else{
                x = rand() % sz;
                //printf("Removendo %d\n", x);
                tInicio = clock();
                remove(tree, *(vit[x]) );
                tFim = clock();
                vit[x] = vit[--sz];
            }
            tempoRemocao += ( (double)(tFim - tInicio) / (CLOCKS_PER_SEC / 1000.0));   
        }

        printf("%d  tempo: %.0lf  alt_media: %.4lf  Remocao %.0lf\n", k++, tempo, altMedia, tempoRemocao);

        N <<= 1;
    }
}



//#####################################################################################################
//#####################################################################################################

void clear(Tree *tree){

    if (tree == NULL) return;
    clear(tree->left);
    clear(tree->right);
    free(tree);
}

long long altura(Tree *tree, int alt){

    if (!tree) return 0;

    long long nos = alt;
    nos += altura(tree->left, alt+1);
    nos += altura(tree->right, alt+1);
    return nos;
}

Tree* newNode(int x){

    Tree *tree = new Tree;
    tree->key = x;
    tree->left = tree->right = NULL;
    tree->miss = 0;
    total++;
    return tree;
}

Tree* makeHead(Tree *child, int x){
    Tree* head = newNode(x);
    int cont = -1;
    if (child->key < x) {     //pai da direita
        head->left = child;
        do{
            Ref += Universe;
            Universe <<= 1;
            cont++;
        }while( (Ref + Universe) <= x);
    }else {         //pai da esquerda
        head->right = child;
        do{
            Ref -= Universe;
            Universe <<= 1;
            cont++;
        }while(Ref - Universe >= x);
    }
    child->miss = cont;
    return head;
}

void createMiddleNode(Tree *tree, int x, int cont){

    Tree *child;
    if (x < tree->key){
        child = tree->left;
        tree->left = newNode(x);
        if (child->key < x) tree->left->left = child;
        else tree->left->right = child;
        tree->left->miss = child->miss - cont - 1;
    }else{
        child = tree->right;
        tree->right = newNode(x);
        if (child->key < x) tree->right->left = child;
        else tree->right->right = child;
        tree->right->miss = child->miss - cont - 1;
    }
    child->miss = cont;
}

bool updateRef(Tree *tree, int x, int &ref, int &jump){

    int cont, key;
    if (x < ref){
        cont = tree->left->miss;
        key = tree->left->key;
    }else{
        cont = tree->right->miss;
        key = tree->right->key;
    }
    do{
        if (x < ref) ref -= jump;
        else ref += jump;
        jump >>= 1;
        if (cont-- == 0) return false;
    }while((x < ref && key < ref) || (x > ref && key > ref));
    
    createMiddleNode(tree, x, cont);
    return true;
}

char updateMiss(int x, int ref, int jump){

    char cont = 0;
    while(ref != x){
        cont++;
        if (x < ref) ref -= jump;
        else ref += jump;
        jump >>= 1;
    }
    return cont;
}

void insert(Tree *&root, int x){

    if (root == NULL){
        root = newNode(x);
        Ref = x; Universe = 1;
    }else if (x >= (Ref + Universe) || x <= (Ref - Universe) ){
        root = makeHead(root, x);
    }else{
        Tree *tree = root;
        int cont, ref = Ref, jump = Universe >> 1;
        while(tree->key != x){
            if (x == ref) swap(x, tree->key);
            if (x < ref){
                if (x > tree->key) swap(tree->key, x);
                if (tree->left == NULL){
                    tree->left = newNode(x);
                    tree->left->miss = updateMiss(x, ref - jump, jump >> 1);
                    return;
                }
                if (updateRef(tree, x, ref, jump)) return;
                tree = tree->left;
            }else{
                if (x < tree->key) swap(tree->key, x);  //aplicação da propagação para deixar sempre o maior a direita
                if (tree->right == NULL){
                    tree->right = newNode(x);
                    tree->right->miss = updateMiss(x, ref + jump, jump >> 1);
                    return;
                }
                if (updateRef(tree, x, ref, jump)) return;
                tree = tree->right;
            }
        }
    }
}





Tree* search(Tree *tree, int x){

    while(tree != NULL){
        if (tree->key == x) return tree;

        if (x < tree->key) tree = tree->left;
        else tree = tree->right;
    }

    return NULL;
}


Tree* replace(Tree **tree){

    while((*tree)->right) 
        tree = &((*tree)->right);
    
    Tree *aux = *tree;
    *tree = (*tree)->left;
    if (*tree)
        (*tree)->miss += aux->miss + 1; 

    return aux;
}

void remove(Tree *&root, int x){

    if (root == NULL) return;
    Tree **tree = &root;

    while(*tree != NULL){
        if ((*tree)->key == x){
            Tree *del = *tree;
            if ((*tree)->left && (*tree)->right){
                *tree = replace(&((*tree)->left));
                (*tree)->miss = del->miss;
                (*tree)->left = del->left;
                (*tree)->right = del->right;
            }else{
                if (del->left) *tree = del->left;
                else *tree = del->right;

                if (*tree){
                    if (del == root){
                        int key = (*tree)->key;
                        do{
                            Universe >>= 1;
                            if (key < x) Ref += Universe;
                            else Ref -= Universe;
                        }while((*tree)->miss--);
                        (*tree)->miss = 0;
                    }else (*tree)->miss += del->miss + 1;
                }
            }
            free(del);
            total--;
            return;
        }
        if (x < (*tree)->key) tree = &((*tree)->left);
        else tree = &((*tree)->right);
    }
}





void print(Tree *tree, int alt){

    if (tree == NULL) return;

    print(tree->left, alt+1);
    cout << tree->key << "   alt: " << alt << "    miss: " << (int)tree->miss << endl;
    print(tree->right, alt+1);
}

void check_is_correct(Tree *tree){

    last = -1;
    int to = total;
    //printf("Universe: %d  Ref: %d\n", Universe, Ref);
    check(tree);
    if (total != 0){
        printf("Diference of %d elements\n", total);
        exit(0);
    }
    total = to;
}

void check(Tree *tree){

    if (tree == NULL) return;

    check(tree->left);
    if (last >= tree->key){
        printf("Err between nodes %u and %u\n", last, tree->key);
        printf("Universe: %d  Ref: %d\n", Universe, Ref);
        exit(0);
    }
    last = tree->key;
    total--;
    check(tree->right);
}

