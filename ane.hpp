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

// Funcao de hash segundo o artigo 
template<size_t Order>
struct ane_default_hash {
    size_t operator()(size_t level, size_t key) const {
        size_t r = 0;
        for (size_t i = 0; i <= key; ++i) {
            r = (key + i) % (Order + level + i);
            if (r < Order)
                break;
        }
        return r;
    }
};

template<class KeyType, class DataType, size_t Order,
    class HashFunction=ane_default_hash<Order> >
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

        bool is_leaf() const {
            for (size_t i = 0; i < Order; ++i)
                if (children[i])
                    return false;
            return true;
        }

    private:
        void _init() {
            for (size_t i = 0; i < Order; ++i)
                children[i] = NULL;
        }
    };

    ane() {
        m_root = NULL;
    }

protected:
    struct node_reference {
        NodePtr parent_node;
        size_t hash_value;

        node_reference() : parent_node(NULL), hash_value(0) {
        }

        node_reference(NodePtr parent, size_t hash)
         : parent_node(parent), hash_value(hash) {
        }

        NodePtr node() const {
            return parent_node->children[hash_value];
        }

        bool valid_parent() const {
            return (bool)parent_node;
        }

        bool valid_node() const {
            return parent_node ?
                (bool)parent_node->children[hash_value] : false;
        }
    };

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
        for (size_t level = 0;; ++level) {
            const HashFunction hash;
            const size_t hash_val = hash(level, key);
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

    // Remove the node associated with the given key.
    // Returns true if the node was removed, or false if no such node exist.
    bool remove(const KeyType &key) {
        node_reference nr;
        if (!_find(key, nr))
            return false;
        NodePtr node = nr.node();
        if (node->is_leaf()) {
            nr.parent_node->children[nr.hash_value] = NULL;
            delete node;
        } else {
            size_t d_level = 0;
            node_reference d_node;
            _find_deepest(nr, key, 0, d_level, d_node);
            node->data = d_node.node()->data;
            node->key = d_node.node()->key;
            delete d_node.parent_node->children[d_node.hash_value];
            d_node.parent_node->children[d_node.hash_value] = NULL;
        }
        return true;
        
    }

    // Finds the node for the given key. Returns null if no such node exist.
    NodePtr find(const KeyType &key) const {
        node_reference nr;
        return _find(key, nr) ? (nr.valid_parent() ? nr.node() : m_root) : NULL;
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

    void _find_deepest(node_reference &base, const KeyType &key,
        size_t level, size_t &d_level, node_reference &d_node)
    {
        bool has_child = false;
        for (size_t i = 0; i < Order; ++i) {
            node_reference child(base.node(), i);
            if (!child.valid_node())
                continue;
            _find_deepest(child, key, level+1, d_level, d_node);
            has_child = true;
        }
        if (!has_child && level > d_level) {
            d_node = base;
            d_level = level;
        }
    }

    bool _find(const KeyType &key, node_reference &nref) const {
        if (!m_root)
            return false;
        NodePtr _node = m_root;
        if (_node->key == key)
            return true;
        for (size_t level = 0;; ++level) {
            const HashFunction hash;
            const size_t hash_val = hash(level, key);
            NodePtr c;
            if (!(c = _node->children[hash_val]))
                return false;
            if (c->key == key) {
                nref = node_reference(_node, hash_val);
                return c;
            }
            _node = c;
        }
        return false;
    }


    template<class T>
    bool _traverse(T &t, NodePtr n) const {
        t.visit(n);
        for (size_t i = 0; i < Order; ++i)
            if (n->children[i])
                if (!_traverse(t, n->children[i]))
                    return false;
        return true;
    }

};

#endif

