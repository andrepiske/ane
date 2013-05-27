/**
* -- BEGIN LICENSE --
* The MIT License (MIT)
* 
* Copyright (c) 2013 André Diego Piske
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* -- END LICENSE --
* Implementação da ANE- Árvore N-ária de Espalhamento conforme o artigo
*   Silva, A. G., Fiorese, A., da Silva, R. E., dos Santos, G. B. (2006)
*   "ANE – Árvore N-ária de Espalhamento Naturalmente Balanceada".
*   Universidade do Estado de Santa Catarina, Joinville - Santa Catarina.
* 
* Implementado por André Diego Piske.
* versão 1. Maio de 2013.
* 
**/
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ane.hpp"

// Funcao de hash segundo o artigo 
template<int Order>
struct MyAneHash {
    int operator()(int level, int key) const {
        int r = 0;
        for (int i = 0; i <= key; ++i) {
            r = (key + i) % (Order + level + i);
            if (r < Order)
                break;
        }
        return r;
    }
};

/* Arvore ANE:
 ane<
    tipo da chave,
    tipo do valor,
    ordem da arvore (se igual a 1 entao torna-se uma lista encadeada),
    funcao de hash
    >
*/
typedef ane<int, std::string, 2, MyAneHash<2> > MyAne;

struct Tv {
    bool visit(MyAne::NodePtr node) const {
        printf("Visit node '%s'\n", node->data.c_str());
        return true;
    }
};

/* Expected output:
-- OUTPUT BEGIN --
Visit node 'Vier'
Visit node 'Neuntausend'
Visit node 'Two dreihundert'
Visit node 'Ein'
Node 15 not found
Node 2300 found: Two dreihundert
Node 1 found: Ein
Node 9000 found: Neuntausend
-- OUTPUT END --
*/
int main(int argc, char *argv[])
{
    MyAne u;
    u.put(4, "Vier");
    u.put(1, "Ein");
    u.put(9000, "Neuntausend");
    u.put(9000, "Das ist kaputt");
    u.put(2300, "Nein!");
    u.put_or_replace(2300, "Two dreihundert");

    u.traverse<const Tv>(Tv());

    const int find_keys[] = { 15, 2300, 1, 9000 };
    for (size_t i = 0; i < sizeof(find_keys)/sizeof(find_keys[0]); ++i) {
        MyAne::NodePtr node = u.find(find_keys[i]);
        printf(node ? "Node %d found: %s\n" : "Node %d not found\n",
            find_keys[i], node ? node->data.c_str() : NULL);
    }

    return 0;
}

