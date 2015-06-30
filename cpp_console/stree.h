#ifndef STREE_H
#define STREE_H

#include <map>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <cstdarg>

namespace pgr {

template<typename T>
std::vector<T> array_as_vector(int count, ...)
{
	std::vector<T> vec;
	va_list args;
	va_start(args, count);
	for (int i = 0; i < count; i++) {
		T t = va_arg(args, T);
		vec.push_back(t);
	}
	va_end(args);
	return vec;
}

template<typename key_type, typename data_type>
class stree;

template<typename key_type>
class stree_node_pos
{
public:
    stree_node_pos() :
        _parent(false, key_type()),
        _left(false, key_type()),
        _right(false, key_type()) {}
    stree_node_pos& parent(bool b, key_type k) {
        _parent.first = b;
        _parent.second = k;
        return *this;
    }
    stree_node_pos& left(bool b, key_type k) {
        _left.first = b;
        _left.second = k;
        return *this;
    }
    stree_node_pos& right(bool b, key_type k) {
        _right.first = b;
        _right.second = k;
        return *this;
    }
    bool has_parent() const { return _parent.first;  }
    bool has_left()   const { return _left.first;    }
    bool has_right()  const { return _right.first;   }
    key_type parent() const { return _parent.second; }
    key_type left()   const { return _left.second;   }
    key_type right()  const { return _right.second;  }
private:
    std::pair<bool, key_type> _parent;
    std::pair<bool, key_type> _left;
    std::pair<bool, key_type> _right;
};

template<typename key_type, typename data_type>
class stree_helper {
    friend class stree<key_type, data_type>;
    typedef stree_node_pos<key_type> stree_node_pos;
    typedef std::pair<key_type, data_type> node_type;
public:
    void add_node(key_type k, data_type d) {
        _node_data[k] = d;
    }

    void add_nodes(const std::vector<node_type>& vec) {
        typename std::vector<node_type>::const_iterator it;
        for (it = vec.begin(); it != vec.end(); it++) {
            add_node(it->first, it->second);
        }
    }

    void add_pos_info(key_type p, const std::vector<key_type>& vec)
    {
        if (vec.empty())
            return;
        if (_node_pos.count(p) == 0) {
            _node_pos[p] = stree_node_pos();
        }
        typename std::vector<key_type>::const_iterator it;
        for (it = vec.begin(); it != vec.end(); it++) {
            stree_node_pos pos;
            pos.parent(true, p);
            if (it > vec.begin())
                pos.left(true, *(it - 1));
            if (it < vec.end() - 1)
                pos.right(true, *(it + 1));
            _node_pos[*it] = pos;
        }
    }
private:
    std::map<key_type, stree_node_pos> _node_pos;
    std::map<key_type, data_type> _node_data;
};

template<typename key_type, typename data_type>
class stree {
public:
    struct stree_node {
        key_type key;
        data_type data;
        stree_node* parent;
        stree_node* left_child;
        stree_node* right_sibling;
        stree_node() :
            parent(0), left_child(0), right_sibling(0) {}
        stree_node(key_type k) : key(k),
            parent(0), left_child(0), right_sibling(0) {}
        stree_node(key_type k, data_type d) : key(k), data(d),
            parent(0), left_child(0), right_sibling(0) {}
    };

    typedef stree_node node_type;

    ~stree()
    {
        typename std::map<key_type, node_type*>::iterator it = _nodes.begin();
        while (it != _nodes.end()) {
            delete it->second;
            it++;
        }
        root = 0;
    }


    void add_node(stree_node_pos<key_type> pos, key_type k, data_type d)
    {
        node_type* n = set_node(k, d);
        node_type* p = pos.has_parent() ? get_node(pos.parent()) : 0;
        node_type* l = pos.has_left()   ? get_node(pos.left())   : 0;
        node_type* r = pos.has_right()  ? get_node(pos.right())  : 0;

        add_child(p, l, r, n);
    }

    void build(const stree_helper<key_type, data_type>& helper)
    {
        typedef typename std::map<key_type, data_type>::const_iterator data_iterator_type;
        typedef typename std::map<key_type, stree_node_pos<key_type> >::const_iterator pos_iterator_type;

        data_iterator_type it = helper._node_data.begin();
        while (it != helper._node_data.end()) {
            pos_iterator_type pos = helper._node_pos.find(it->first);
            if (pos != helper._node_pos.end()) {
                add_node(pos->second, it->first, it->second);
            }
            it++;
        }
    }

    void print()
    {
        typedef std::vector< std::pair<int, node_type*> > result_type;
        result_type vec;
		std::cout << "***depth traversal***" << std::endl;
        if (depth_traversal(vec) > 0) {
            typename result_type::const_iterator it = vec.begin();
            while (it != vec.end()) {
				std::cout << std::string(it->first * 2, ' ') << it->second->key
                     << ": " << it->second->data << std::endl;
                it++;
            }
        }

        vec.clear();
		std::cout << "***breadth traversal***" << std::endl;
        if (breadth_traversal(vec) > 0) {
            int depth = 0;
            typename result_type::const_iterator it = vec.begin();
            while (it != vec.end()) {
                if (it->first > depth) {
					std::cout << std::endl;
                    depth = it->first;
                }
                else if (it != vec.begin()) {
					std::cout << " ";
                }
				std::cout << it->second->key << ": " << it->second->data;
                it++;
            }
        }
    }

private:
    node_type* get_node(key_type k)
    {
        if (_nodes.count(k) == 0) {
            _nodes[k] = new node_type(k);
        }
        return _nodes[k];
    }

    node_type* set_node(key_type k, data_type d)
    {
        node_type* n = get_node(k);
        n->data = d;
        return n;
    }

    void add_child(node_type* p, node_type* l, node_type* r, node_type* n)
    {
        if (p)
        {
            if (l && r) {
                l->right_sibling = n;
                n->parent = p;
                n->right_sibling = r;
            }
            else if (l) {
                l->right_sibling = n;
                n->parent = p;
            }
            else if (r) {
                n->right_sibling = r;
                n->parent = p;
                p->left_child = n;
            }
            else {
                node_type* rc = rightmost_child(p);
                if (rc) {
                    rc->right_sibling = n;
                    n->parent = p;
                }
                else {
                    p->left_child = n;
                    n->parent = p;
                }
            }
        }
        else
        {
            root = n;
        }
    }

    node_type* rightmost_child(node_type* p)
    {
        if (p) {
            node_type* q = p->left_child;
            while (q && q->right_sibling) {
                q = q->right_sibling;
            }
            return q;
        }
        return 0;
    }

    int depth_traversal(std::vector< std::pair<int, node_type*> >& result)
    {
        if (!root)
            return 0;
        typedef std::pair<int, node_type*> pair_type;
        std::vector<pair_type> stack;
        std::vector<pair_type> vec;  //output from left to right
        stack.push_back(pair_type(0, root));
        while (!stack.empty()) {
            pair_type n = stack.back();
            stack.pop_back();
            result.push_back(n);
            node_type* child = n.second->left_child;
            while (child) {
                vec.push_back(pair_type(n.first + 1, child));
                child = child->right_sibling;
            }
            while (!vec.empty()) {
                stack.push_back(vec.back());
                vec.pop_back();
            }
        }
        return result.size();
    }

    int breadth_traversal(std::vector< std::pair<int, node_type*> >& result)
    {
        if (!root)
            return 0;
        typedef std::pair<int, node_type*> pair_type;
        std::queue<pair_type> que;
        que.push(pair_type(0, root));
        while (!que.empty()) {
            pair_type n = que.front();
            que.pop();
            result.push_back(n);
            node_type* child = n.second->left_child;
            while (child) {
                que.push(pair_type(n.first + 1, child));
                child = child->right_sibling;
            }
        }
        return result.size();
    }

    node_type* root;
    std::map<key_type, node_type*> _nodes;
};

} //namespace pgr

#endif // STREE_H

