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

/* Arvore ANE:
 ane<
    tipo da chave,
    tipo do valor,
    ordem da arvore (se igual a 1 entao torna-se uma lista encadeada),
    funcao de hash
    >
*/

template<size_t Order>
struct MyStringHash {
    ane_default_hash<Order> h;

    // from: http://www.cse.yorku.ca/~oz/hash.html
    static size_t string_hash(const std::string &key) {
        size_t s = 5381;
        const size_t len = key.size();
        const char *ptr = key.c_str();
        for (size_t i = 0; i < len; ++i, ++ptr)
            s = ((s << 5) + s) + (size_t)*ptr;
        return s;
    }

    size_t operator()(size_t level, const std::string &key) const {
        return h(level, string_hash(key));
    }
};

typedef ane<int, std::string, 2> MyAne;
typedef ane<std::string, std::string, 2, MyStringHash<2> > MyAneString;

struct Tv {
    bool visit(MyAne::NodePtr node) const {
        printf("Visit node '%s'\n", node->data.c_str());
        return true;
    }
};

struct TvString {
    bool visit(MyAneString::NodePtr node) const {
        printf("Visit node '%s'\n", node->data.c_str());
        return true;
    }
};

/* Expected output:
-- OUTPUT BEGIN --
Visit node 'Vier'
Visit node 'Two dreihundert'
Visit node 'Klein bottle'
Node 1 found: Klein bottle
Node 15 not found
Node 2300 found: Two dreihundert
Node 4 found: Vier
Node 9000 not found
Visit node 'Vier'
Visit node 'Two dreihundert'
Visit node 'Klein bottle'
Node um found: Klein bottle
Node quinze not found
Node dois mil e trezentos found: Two dreihundert
Node quatro found: Vier
Node nove mil not found
-- OUTPUT END --
*/

void string_ane();
void int_ane();

int main(int argc, char *argv[])
{
    int_ane();
    string_ane();
    return 0;
}

void int_ane() {
    MyAne u;
    u.put(4, "Vier");
    u.put(1, "Ein");
    u.put(9000, "Neuntausend");
    u.put(9000, "Das ist kaputt");
    u.put(2300, "Nein!");
    u.put_or_replace(2300, "Two dreihundert");

    u.remove(1);
    u.remove(9000);
    u.put(1, "Klein bottle");

    u.traverse<const Tv>(Tv());

    const int find_keys[] = { 1, 15, 2300, 4, 9000 };
    for (size_t i = 0; i < sizeof(find_keys)/sizeof(find_keys[0]); ++i) {
        MyAne::NodePtr node = u.find(find_keys[i]);
        printf(node ? "Node %d found: %s\n" : "Node %d not found\n",
            find_keys[i], node ? node->data.c_str() : NULL);
    }
}

void string_ane() {
    MyAneString u;
    u.put("quatro", "Vier");
    u.put("um", "Ein");
    u.put("nove mil", "Neuntausend");
    u.put("nove mil", "Das ist kaputt");
    u.put("dois mil e trezentos", "Nein!");
    u.put_or_replace("dois mil e trezentos", "Two dreihundert");

    u.remove("um");
    u.remove("nove mil");
    u.put("um", "Klein bottle");

    u.traverse<const TvString>(TvString());

    const char* find_keys[] = { "um", "quinze", "dois mil e trezentos", "quatro", "nove mil" };
    for (size_t i = 0; i < sizeof(find_keys)/sizeof(find_keys[0]); ++i) {
        MyAneString::NodePtr node = u.find(find_keys[i]);
        printf(node ? "Node %s found: %s\n" : "Node %s not found\n",
            find_keys[i], node ? node->data.c_str() : NULL);
    }
}

