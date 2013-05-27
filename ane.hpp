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
#ifndef __ARVORE_NE_H__
#define __ARVORE_NE_H__

template<class KeyType, class DataType, int Order, class HashFunction>
class ane
{
public:
    struct node;
    typedef node Node;
    typedef node *NodePtr;

    struct node {
        node() {
            _init();
        }

        node(const KeyType &_k, const DataType &_d)
            : key(_k), data(_d) {
            _init();
        }

        KeyType key;
        DataType data;
        ane::NodePtr children[Order];

    private:
        void _init() {
            for (int i = 0; i < Order; ++i)
                children[i] = NULL;
        }
    };

    ane() {
        m_root = NULL;
    }

public:
    typedef ane<KeyType, DataType, Order, HashFunction> ThisType;

    ane(const ThisType &c) : m_root(c.m_root) {
    }

    template<bool replace>
    bool _put(const KeyType &key, const DataType &data) {
        if (!m_root) {
            m_root = new node(key, data);
            return true;
        }
        NodePtr _node = m_root;
        for (int level = 0;; ++level) {
            const HashFunction hash;
            const int hash_val = hash(level, key);
            NodePtr *c = &_node->children[hash_val];
            if (!*c) {
                *c = new node(key, data);
                return true;
            }
            if ((*c)->key == key) {
                if (!replace)
                    break;
                (*c)->data = data;
                return true;
            }
            _node = *c;
        }
        return false;
    }

    // Finds the node for the given key. Returns null if no such node exist.
    NodePtr find(const KeyType &key) const {
        if (!m_root)
            return NULL;
        NodePtr _node = m_root;
        for (int level = 0;; ++level) {
            const HashFunction hash;
            const int hash_val = hash(level, key);
            NodePtr c;
            if (!(c = _node->children[hash_val]))
                return NULL;
            if (c->key == key)
                return c;
            _node = c;
        }
        return NULL;
    }

    // Tree traversal.
    template<typename T>
    void traverse(T &t) const {
        if (!m_root)
            return;
        _traverse(t, m_root);
    }

    // Inserts a pair <key, data> in the tree.
    // Returns true if data has been inserted. False if key already in tree.
    bool put(const KeyType &key, const DataType &data) {
        return _put<false>(key, data);
    }

    // Inserts a pair <key, data> in the tree. If key already exists,
    // replace its data.
    void put_or_replace(const KeyType &key, const DataType &data) {
        _put<true>(key, data);
    }

protected:
    NodePtr m_root;

    template<class T>
    bool _traverse(T &t, NodePtr n) const {
        t.visit(n);
        for (int i = 0; i < Order; ++i)
            if (n->children[i])
                if (!_traverse(t, n->children[i]))
                    return false;
        return true;
    }

};

#endif

