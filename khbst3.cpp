/*
* Author: Edimar Jacob Bauer
* Email: edimarjb@gmail.com
* Date: October 11, 2018
*/

/*
* Key Hidden Binary Search Tree 3
* 
* Estrutura dinâmica como a KHBST 2
* Porém, ao invés de calcular o valor de referência com a função lca(), que possui complexidade O(log(B)),
* calcula-se o valor de referência através de operações bitwise constantes
*
* Vantagens com relação a KHBST 2
* Melhora a complexidade de inserção para O(B) apenas
* 
* Desvantagens com relação a KHBST 2
* Maior dificuldade de entendimento e implementação do código
*
*
* OBS.: 
* A dificuldade maior de implementação só existe porque NÃO EXISTE OPERAÇÃO BITWISE PARA O PROPÓSITO, 
* logo, é feita uma "gambiarra"
*
* Caso existisse tal operação bitwise, a KHBST 3 seria o melhor algoritmo para tornar a HBST "dinâmica"
* sem dúvidas, pois reduziria uso de memória, número de linhas de código e constantes de tempo de inserção
*
*/

#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int T = (1 << 24)+1;
int N = (1 << 3);
int last, total;
set<int> mapa;
set<int>::iterator vit[T];

struct Tree{
    int key;
    struct Tree *left, *right;
};

long long altura(Tree *tree, int alt);
void clear(Tree *tree);

Tree* newNode(int x);
void swapNode(Tree *&tree, Tree *&node);
void defineRef(Tree *tree, int x, int &scope, int &left, int &right);
void insert(Tree *&tree, int x);
void insertNode(Tree *&tree, Tree *node);

Tree* search(Tree *tree, int x);

void propagaNode(Tree **tree);
Tree* replace(Tree **tree, Tree *ant);
void remove(Tree *&root, int x);

void print(Tree *tree, int height);
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

void defineRef(Tree *tree, int x, int &scope, int &left, int &right){

    if (tree->left) {
        left = tree->left->key;
        right = (tree->right) ? tree->right->key : tree->key;
    }else{
        left = tree->key;
        right = (tree->right && (left + (left & -left) < tree->right->key) ) ? tree->right->key : tree->key;
    }
    scope = (left == right) ? ( x < left && x + (x & -x) > left ? 0 : (left & -left) << 1) : left xor right;
    left ^= x; right ^= x;
}

void swapNode(Tree *&tree, Tree *&node){

    node->left = tree->left;
    node->right = tree->right;
    Tree *aux = tree;
    tree = node;
    node = aux;
    node->left = node->right = NULL;
}

void insertNode(Tree *&tree, Tree *node){

    if (tree == NULL) tree = node;
    else if (tree->key != node->key){
        int left, right, scope;
        defineRef(tree, node->key, scope, left, right);

        if (left >= scope && right >= scope || (node->key & -node->key) > scope){
            //insere pai
            if (node->key < tree->key) node->right = tree;
            else node->left = tree;
            tree = node;
        }else{
            if (left != right && scope < ((node->key & -node->key) << 1) ){
                //propagação estendida
                swapNode(tree, node);
                left = right;
            }
            if ( (left != right && left < right) or (left == right && node->key < tree->key) ){
                if (node->key > tree->key) swapNode(tree, node);
                insertNode(tree->left, node);
            }else{
                if (node->key < tree->key) swapNode(tree, node);
                insertNode(tree->right, node);
            }
        }
    }else{
        free(node);
        total--;
    }
}

void insert(Tree *&tree, int x){
    insertNode(tree, newNode(x));
}



Tree* search(Tree *tree, int x){

    while(tree != NULL){
        if (tree->key == x) return tree;

        if (x < tree->key) tree = tree->left;
        else tree = tree->right;
    }

    return NULL;
}




void propagaNode(Tree **tree){

    Tree *node = *tree;
    if (node->left){
        *tree = node->left;
        node->left = NULL;
    }else{
        *tree = node->right;
        node->right = NULL;
    }
    insertNode(*tree, node);
}

Tree* replace(Tree **tree, Tree **ant){

    bool b = 0;
    while((*tree)->right){
        b = 1;
        ant = tree;
        tree = &((*tree)->right);
    }
    Tree *node = *tree;
    *tree = (*tree)->left;
    if (b && !(*tree) )
        propagaNode(ant);

    return node;
}

void remove(Tree *&root, int x){

    Tree **tree = &root, **ant = NULL;
    while(*tree){
        if ((*tree)->key == x){
            Tree *del = *tree;
            if ((*tree)->left == NULL && (*tree)->right == NULL){
                *tree = NULL;
                if (ant) propagaNode(ant);
            }else{
                if ((*tree)->left && (*tree)->right){
                    Tree *node = replace(&((*tree)->left), tree);
                    node->left = (*tree)->left;
                    node->right = (*tree)->right;
                    *tree = node;
                    if ((*tree)->left == NULL)
                        propagaNode(tree);
                }else{
                    Tree *child = (*tree)->left ? (*tree)->left : (*tree)->right;
                    *tree = child;
                }
            }
            free(del);
            total--;
            return;
        }
        ant = tree;
        if (x < (*tree)->key) tree = &((*tree)->left);
        else tree = &((*tree)->right);
    }
}





void print(Tree *tree, int height){

    if (tree == NULL) return;

    print(tree->left, height+1);
    cout << tree->key << "   alt: " << height << endl;
    print(tree->right, height+1);
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

void check(Tree *tree){

    if (tree == NULL) return;

    check(tree->left);
    if (last >= tree->key){
        printf("Err between nodes %d and %d\n", last, tree->key);
        exit(0);
    }
    last = tree->key;
    total--;
    check(tree->right);
}

