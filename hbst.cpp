#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int T = (1<<24) + 1;
const int HIGH = (1<<30);
int N = (1 << 10);
set<int> mapa;
set<int>::iterator vit[T];
priority_queue<int> fila;
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
Tree* replace(Tree **tree);
void remove(Tree *&root, int x);

void check_is_correct(Tree *tree);
void check(Tree *tree, int low, int high);
void print(Tree *tree);

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
                x = rand() % HIGH;
            }while(mapa.count(x) > 0);
            
            for (int j = 0; j < 1000 && mapa.count(x) == 0 && x < HIGH && i <= N; j++){
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


        printf("%d  tempo: %.0lf  alt_media: %.4lf  remocao: %.0lf\n", k++, tempo, altMedia, tempoRemocao);

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

    int lower = 0, upper = HIGH, ref;
    while(tree != NULL){
        if (x == tree->key) return tree;

        ref = (lower + upper) >> 1;
        if (x < ref) {
            tree = tree->left;
            upper = ref;
        }else {
            tree = tree->right;
            lower = ref;
        }
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
        if (x < middle){
            if (tree->left == NULL){
                tree->left = newNode(x);
                return;
            }
            tree = tree->left;
            high = middle;
        }else{
            if (tree->right == NULL){
                tree->right = newNode(x);
                return;
            }
            tree = tree->right;
            low = middle;
        }
    }
}

Tree* replace(Tree **tree){
    
    do{
        if ((*tree)->left) tree = &((*tree)->left);
        else if ((*tree)->right) tree = &((*tree)->right);
        else break;
    }while(1);

    Tree *aux = *tree;
    *tree = NULL;
    return aux;
}

void remove(Tree *&root, int x){

    Tree **tree = &root;
    int lower = 0, upper = HIGH, ref;
    while(*tree != NULL){
        if (x == (*tree)->key){
            Tree *del = *tree;
            if (!(*tree)->left && !(*tree)->right)
                *tree = NULL;
            else{
                *tree = replace(tree);
                (*tree)->left = del->left;
                (*tree)->right = del->right;
            }

            free(del);
            total--;
            break;
        }else{
            ref = (lower + upper) >> 1;
            if (x < ref) {
                tree = &((*tree)->left);
                upper = ref;
            }else {
                tree = &((*tree)->right);
                lower = ref;
            }
        }
    }
}




void print(Tree *tree){

    if (tree == NULL) return;

    fila.push(tree->key);
    print(tree->left);
    print(tree->right);

    //printf("%d\n", fila.top());
    fila.pop();
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

