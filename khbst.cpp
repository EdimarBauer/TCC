/*
* Author: Edimar Jacob Bauer
* Email: edimarjb@gmail.com
* Date: October 11, 2018
*/

/*
* Key Hidden Binary Search Tree
* 
* Estrutura dinâmica como a DHBST (Dinamic Hidden Binary Search tree), nós podem ser inseridos no meio
* Porém, ao invés de guardar a quantidade de nós faltantes acima (miss), guarda-se o valor de referência do nó
* 
* O valor de referência do nó é calculado com base nos valores chaves dos nós filhos através da função lca()
* logo após a inserção do mesmo
*
* Nó inserido no meio com apenas um nó filho, usa o próprio valor chave mais o valor chave do nó filho
* para satisfazer a função lca() 
*
* Nó inserido como nó folha usa seu próprio valor chave como valor de refência
*
* Vantagens com relação a DHBST 
* Não necessita calcular cada nível durante a operação de inserção
* Um pouco mais simples de programar
* 
* Desvantagens com relação a DHBST
* Usa a função lca() após cada inserção, exceto quando inserido como nó folha, cuja complexidade é O(log(B))
* Usa mais memória, pois a DHBST usa um char (1 byte) para guardar o miss, enquanto a KHBST guarda o
* valor de referência, que é do tamanho da chave
*
*
*/

#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int N = (1 << 10);
int last, total;
set<int> mapa;

struct Tree{
    int key, ref;
    struct Tree *left, *right;
};

long long altura(Tree *tree, int alt);
void clear(Tree *tree);

Tree* newNode(int x);
int lca(int x, int y);
Tree* makeHead(Tree* child, int x);
void createMiddleNode(Tree *tree, int x);
void insert(Tree *&root, int x);

Tree* search(Tree *tree, int x);
int replace(Tree **tree);
void remove(Tree *&root, int x);

void print(Tree *tree, int height);
void check_is_correct(Tree *tree);
void check(Tree *tree);


int main(){

    Tree* tree = NULL;
    int k = log2(N), x;
    clock_t tInicio, tFim;
    double tempo;

    while(N < (1 << 25) ){
        total = 0; tempo = 0;
        clear(tree); mapa.clear();
        tree = NULL;

        for (int i = 1; i <= N; i++){
            do{
                x = rand() % (1 << 30);
            }while(mapa.count(x) > 0);
            mapa.insert(x);

            tInicio = clock();
            insert(tree, x);
            tFim = clock();

            tempo += ( (double)(tFim - tInicio) / (CLOCKS_PER_SEC / 1000.0));
        }

        ll alt = altura(tree, 0);
        double altMedia = ((double)alt / total);

        printf("%d  tempo: %.0lf  alt_media: %.4lf    elementos: %d\n", k++, tempo, altMedia, total);

        N <<= 1;
    }
}



//#####################################################################################################
//#####################################################################################################


long long altura(Tree *tree, int alt){

    if (!tree) return 0;

    long long nos = alt;
    nos += altura(tree->left, alt+1);
    //printf("%d   alt: %d\n", tree->key, alt);
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
    tree->key = tree->ref = x;
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

void insert(Tree *&root, int x){

    //printf("Inserting %d\n", x);
    if (!x) return;
    if (root == NULL) root = newNode(x);
    else{
        Tree **parent = &root;
        Tree *tree = root;
        int limit;
        while(tree->key != x){
            limit = tree->ref & -tree->ref;
            if ((tree->ref + limit) <= x){          //cria nó pai direito
                *parent = newNode(x);
                (*parent)->left = tree;
                (*parent)->ref = lca(tree->key, x);
                return;
            }else if (x <= (tree->ref - limit) ){   //cria nó pai esquerdo
                *parent = newNode(x);
                (*parent)->right = tree;
                (*parent)->ref = lca(x, tree->key);
                return;
            }

            if (x == tree->ref) swap(x, tree->key);     //propagação estendida
            if (x < tree->ref){
                if (x > tree->key) swap(x, tree->key);      //propagação a esquerda
                if (tree->left == NULL){
                    tree->left = newNode(x);
                    return;
                }
                parent = &(tree->left);
                tree = tree->left;
            }else{
                if (x < tree->key) swap(x, tree->key);      //propagação a direita
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

int replace(Tree **tree){

    while((*tree)->right)
        tree = &((*tree)->right);

    int x = (*tree)->key;
    if ((*tree)->left)
        (*tree)->key = replace(&((*tree)->left));
    else{
        free(*tree);
        *tree = NULL;
    }
    return x;
}

void remove(Tree *&root, int x){

    //printf("Removing %d\n", x);
    Tree **tree = &root;
    while(*tree != NULL){
        if ((*tree)->key == x){
            if ((*tree)->left == NULL && (*tree)->right == NULL){
                free(*tree);
                *tree = NULL;
            }else{
                if ((*tree)->left && (*tree)->right)
                    (*tree)->key = replace(&((*tree)->left));
                else{
                    Tree *aux = (*tree)->left ? (*tree)->left : (*tree)->right;
                    free(*tree);
                    *tree = aux;
                }
            }
            total--;
            return;
        }
        if (x < (*tree)->key) tree = &((*tree)->left);
        else tree = &((*tree)->right);
    }
}





void print(Tree *tree, int height){

    if (tree == NULL) return;

    print(tree->left, height+1);
    cout << tree->key << " " << height << "   ref: " << tree->ref << endl;
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

