#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int T = (1<<24) + 1;
const int HIGH = (1<<30);
int N = (1 << 10);
set<int> mapa;
set<int>::iterator vit[T];
int total, last;

struct Tree{
    int key;
    Tree *left, *right;
};

long long altura(Tree *tree, int alt);
void clear(Tree *tree);
Tree* newNode(int x);
Tree* search(Tree *tree, int x);
void insert(Tree *&root, int x);
Tree* biggerLeft(Tree **tree);
Tree* smallerRight(Tree **tree);
void remove(Tree *&root, int x);

void check_is_correct(Tree *tree);
void check(Tree *tree, int low, int high);
void print(Tree *tree);

int main(){

    Tree* tree = NULL;
    int k = log2(N), x;
    clock_t tInicio, tFim;
    double tempo, tempoRemocao;

    while(N < (1 << 25)){
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
    tree->left = tree->right = NULL;
    total++;
    return tree;
}

Tree* search(Tree* tree, int x){

    while(tree != NULL){
        if (x == tree->key) return tree;

        if (x < tree->key) tree = tree->left;
        else tree = tree->right;
    }

    return NULL;
}





void insert(Tree *&root, int x){

    if (root == NULL){
        root = newNode(x);
        return;
    }
    Tree *tree = root;
    int middle, low = 0, high = HIGH;

    while(tree->key != x){
        middle = (low + high) >> 1;
        if (x == middle) swap(x, tree->key);
        if (x < middle){
            if (x > tree->key) swap(x, tree->key);
            if (tree->left == NULL){
                tree->left = newNode(x);
                return;
            }
            tree = tree->left;
            high = middle;
        }else{
            if (x < tree->key) swap(x, tree->key);
            if (tree->right == NULL){
                tree->right = newNode(x);
                return;
            }
            tree = tree->right;
            low = middle;
        }
    }
}

Tree* biggerLeft(Tree **tree){
    
    while((*tree)->right) tree = &((*tree)->right);

    Tree *aux = *tree;
    if ((*tree)->left) {
        *tree = biggerLeft(&((*tree)->left));
        (*tree)->left = aux->left;
    }else *tree = NULL;

    return aux;
}

Tree* smallerRight(Tree **tree){
    
    while((*tree)->left) tree = &((*tree)->left);

    Tree *aux = *tree;
    if ((*tree)->right) {
        *tree = smallerRight(&((*tree)->right));
        (*tree)->right = aux->right;
    }else *tree = NULL;

    return aux;
}

void remove(Tree *&root, int x){

    Tree **tree = &root;
    while(*tree != NULL){
        if (x == (*tree)->key){
            Tree *del = *tree;
            if (!(*tree)->left && !(*tree)->right)
                *tree = NULL;
            else{
                if ((*tree)->left) *tree = biggerLeft(&((*tree)->left));
                else if ((*tree)->right) *tree = smallerRight(&((*tree)->right));
                (*tree)->left = del->left;
                (*tree)->right = del->right;
            }

            free(del);
            total--;
            break;
        }else{
            if (x < (*tree)->key) tree = &((*tree)->left);
            else tree = &((*tree)->right);
        }
    }
}




void print(Tree *tree){

    if (tree == NULL) return;

    print(tree->left);
    //printf("%d  alt: %d\n", tree->key, alt);
    print(tree->right);
}

void check_is_correct(Tree *tree){

    last = INT_MIN;
    int to = total;
    check(tree, 0, HIGH);
    if (total != 0){
        printf("Diference of %d elements\n", total);
        exit(0);
    }
    total = to;
}

void check(Tree *tree, int low, int high){

    if (tree == NULL) return;

    int middle = (low + high) / 2;
    check(tree->left, low, middle);
    if (tree->key <= low || tree->key >= high){
        printf("Err. %d out of your interval. Low: %d   High: %d\n", tree->key, low, high);
        exit(0);
    }
    if (tree->key <= last){
        printf("Err. Element out of order between %d and %d\n", last, tree->key);
        exit(0);
    }
    last = tree->key;
    total--;
    check(tree->right, middle, high);
}

