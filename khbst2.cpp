/*
* Author: Edimar Jacob Bauer
* Email: edimarjb@gmail.com
* Date: October 11, 2018
*/

/*
* Key Hidden Binary Search Tree 2
* 
* Estrutura dinâmica como a KHBST
* Porém, ao invés de guardar o valor de referência, calcula-se o valor de referência na hora
*
* Vantagens com relação a KHBST 
* Usa menos memória, pois não armazena o valor de referência de cada nó
* 
* Desvantagens com relação a KHBST
* Complexidade de tempo de inserção maior, 
* a KHBST possui complexidade O(B + log(B))
* a KHBST 2 possui complexidade O(B * log(B))
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
int lca(int x, int y);
int defineRef(Tree *tree);
void insert(Tree *&root, int x);

Tree* search(Tree *tree, int x);

void propagaNodeRight(Tree **tree);
void propagaNodeLeft(Tree **tree);
int replace(Tree **tree, Tree *ant);
void remove(Tree *&root, int x);

void print(Tree *tree, int height);
void check_is_correct(Tree *tree);
void check(Tree *tree, int alt);


int main(){

    Tree* tree = NULL;
    int k = log2(N), x;
    clock_t tInicio, tFim;
    double tempo, tempoRemocao;;

    while(N < (1 << 25)){
        total = 0; tempo = 0, tempoRemocao = 0;
        clear(tree); mapa.clear();
        tree = NULL;

        for (int i = 0; i < N; i++){
            //procura chave ainda não inserida
            do{
                x = rand() % (1 << 30);
            }while(mapa.count(x) > 0);
            vit[i] = mapa.insert(x).first;  //salva o endereço desta chave para usar na remoção

            tInicio = clock();
            insert(tree, x);
            tFim = clock();

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

int lca(int x, int y){

    //x deve ser menor que y sempre
    //if (x > y) swap(x, y);
    int dy = y & -y;
    int dx = x & -x;
    if (dy > dx){       //ser maior significa estar mais alto na árvore, ou seja, mais próximo do pai comum
        while(x <= (y - dy)){
            y -= dy;
            dy = y & -y;
        }
        return y;
    }else{
        while(y >= (x + dx)){
            x += dx;
            dx = x & -x;
        }
        return x;
    }
}

int defineRef(Tree *tree){
    if (tree->left)
        if (tree->right) return lca(tree->left->key, tree->right->key);
        else return lca(tree->left->key, tree->key);
    if (tree->right)
        return lca(tree->key, tree->right->key);
    return tree->key;
}

void insert(Tree *&root, int x){

    if (root == NULL) root = newNode(x);
    else{
        Tree **parent = &root;
        Tree *tree = root;
        int limit, ref;
        while(tree->key != x){
            //verifica se precisa criar nó pai
            ref = defineRef(tree);
            limit = ref & -ref;
            if ((ref + limit) <= x){ //pai direito
                *parent = newNode(x);
                (*parent)->left = tree;
                return;
            }else if (x <= (ref - limit) ){   //pai esquerdo
                *parent = newNode(x);
                (*parent)->right = tree;
                return;
            }

            if (x == ref) swap(x, tree->key);   //propagação estendida
            if (x < ref){
                if (x > tree->key) swap(x, tree->key);  //propagação
                if (tree->left == NULL){
                    tree->left = newNode(x);
                    return;
                }
                parent = &(tree->left);
                tree = tree->left;
            }else{
                if (x < tree->key) swap(x, tree->key);  //propagação
                if (tree->right == NULL){
                    tree->right = newNode(x);
                    return;
                }
                parent = &(tree->right);
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




void propagaNodeRight(Tree **tree){

    Tree *child = *tree;
    *tree = (*tree)->right;
    int ref;
    do{
        tree = &((*tree)->left);
        if (*tree == NULL) break;
        ref = defineRef(*tree);
    }while(child->key > (ref - (ref & -ref)) );
    child->right = *tree;
    *tree = child;
}

void propagaNodeLeft(Tree **tree){

    Tree *child = *tree;
    *tree = (*tree)->left;
    int ref;
    do{
        tree = &((*tree)->right);
        if (*tree == NULL) break;
        ref = defineRef(*tree);
    }while(child->key < (ref + (ref & -ref)) );
    child->left = *tree;
    *tree = child;
}

void propagaNode(Tree **tree){

    if ((*tree)->right){
        int ref = defineRef((*tree)->right);
        if ((*tree)->key > (ref - (ref & -ref) ) )
            propagaNodeRight(tree);
    }else if ((*tree)->left){
        int ref = defineRef((*tree)->left);
        if ((*tree)->key < (ref + (ref & -ref) ) )
            propagaNodeLeft(tree);
    }
}

int replace(Tree **tree, Tree **ant){

    bool b = 0;
    while((*tree)->right){
        b = 1;
        ant = tree;
        tree = &((*tree)->right);
    }
    int x = (*tree)->key;
    Tree *left = (*tree)->left;
    free(*tree);
    *tree = left;
    if (b && !left)
        propagaNode(ant);

    return x;
}

void remove(Tree *&root, int x){

    //printf("Removing %d\n", x);
    Tree **tree = &root, **ant = NULL;
    while(*tree){
        if ((*tree)->key == x){
            if ((*tree)->left == NULL && (*tree)->right == NULL){
                free(*tree);
                *tree = NULL;
                if (ant) propagaNode(ant);
            }else{
                if ((*tree)->left && (*tree)->right){
                    (*tree)->key = replace(&((*tree)->left), tree);
                    if ((*tree)->left == NULL)
                        propagaNode(tree);
                }else{
                    Tree *child = (*tree)->left ? (*tree)->left : (*tree)->right;
                    free(*tree);
                    *tree = child;
                }
            }
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
    cout << tree->key << "   alt: " << height << "   ref: " << defineRef(tree) << endl;
    print(tree->right, height+1);
}

void check_is_correct(Tree *tree){

    last = -1;
    int to = total;
    check(tree, INT_MAX);
    if (total != 0){
        printf("Diference of %d elements\n", total);
        exit(0);
    }
    total = to;
}

void check(Tree *tree, int alt){

    if (tree == NULL) return;

    int ref = defineRef(tree);
    int limit = ref & -ref;

    if (alt <= limit){
        printf("Err in node %d   alt: %d  limit: %d\n", tree->key, alt, limit);
        exit(0);
    }

    check(tree->left, limit);
    if (last >= tree->key){
        printf("Err between nodes %d and %d\n", last, tree->key);
        exit(0);
    }
    last = tree->key;
    total--;
    check(tree->right, limit);
}

