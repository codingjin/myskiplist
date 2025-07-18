#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>

template<typename K, typename V>
class Node {
public:
    Node();
    Node(const K& k, const V& v, const int node_level);
    ~Node();
    K get_key() const;
    V get_value() const;
    void set_key(const K& k);
    void set_value(const V& v);
    int node_level;
    Node<K, V>** forward;

private:
    K key;
    V value;
};

template<typename K, typename V>
Node<K, V>::Node()
    : key{}, value{}, node_level{0}, forward{nullptr}
{}

template<typename K, typename V>
Node<K, V>::Node(const K& k, const V& v, const int node_level)
    : key{k}, value{v}, node_level{node_level}
{
    forward = new Node<K, V>*[node_level + 1];
    memset(forward, 0, sizeof(Node<K, V>*) * (node_level + 1));
}

template<typename K, typename V>
Node<K, V>::~Node()
{
    delete[] forward;
}

template<typename K, typename V>
K Node<K, V>::get_key() const
{
    return key;
}

template<typename K, typename V>
V Node<K, V>::get_value() const
{
    return value;
}

template<typename K, typename V>
void Node<K, V>::set_key(const K& k)
{
    key = k;
}

template<typename K, typename V>
void Node<K, V>::set_value(const V& v)
{
    value = v;
}


template<typename K, typename V>
class SkipList {
public:
    SkipList(const int maxlevel);
    ~SkipList();
    int get_random_level() const;
    Node<K, V>* create_node(const K& key, const V& value, const int node_level);
    bool search(const K& key) const;
    bool insert(const K& key, const V& value);
    bool del(const K& key);
private:
    int _max_level;
    int _skiplist_level;
    int _element_count;
    Node<K, V>* _header;
};

template<typename K, typename V>
SkipList<K, V>::SkipList(const int maxlevel)
    : _max_level{maxlevel}, _skiplist_level{0}, _element_count{0}
{
    if (maxlevel <= 0) {
        std::cerr << "Invalid maxlevel for SkipList!" << std::endl;
        exit(1);
    }
    K key{};
    V value{};
    _header = new Node<K, V>{key, value, _max_level};
}

template<typename K, typename V>
SkipList<K, V>::~SkipList()
{
    delete _header;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level() const
{
    int klevel = 0;
    while (rand() % 2) {
        ++klevel;
        if (klevel >= _max_level)   break;
    }
    return klevel;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K& key, const V& value, const int node_level)
{
    Node<K, V>* node = new Node<K, V>{key, value, node_level};
    return node;
}

template<typename K, typename V>
bool SkipList<K, V>::search(const K& key) const
{
    Node<K, V>* current = _header;
    for (int i = _skiplist_level; i >= 0; --i) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        if (current->forward[i] && current->forward[i]->get_key() == key)  return true;
    }
    return false;
}

template<typename K, typename V>
bool SkipList<K, V>::insert(const K& key, const V& value)
{
    Node<K, V>* update[_skiplist_level + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (_skiplist_level + 1));
    Node<K, V>* current = _header;

    for (int i = _skiplist_level; i >= 0; --i) {
        while (current->forward[i] && current->forward[i]->get_key() < key)
            current = current->forward[i];
        
        if (current->forward[i] && current->forward[i]->get_key() == key)
            return false;
        
        update[i] = current;
    }

    int node_level = get_random_level();
    Node<K, V>* node = new Node<K, V>{key, value, node_level};
    if (node_level > _skiplist_level) {
        for (int i = node_level; i > _skiplist_level; --i) {
            _header->forward[i] = node;
        }
        for (int i = _skiplist_level; i >= 0; --i) {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }
        _skiplist_level = node_level;
    } else {
        for (int i = node_level; i >= 0; --i) {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }
    }
    ++_element_count;
    return true;
}

template<typename K, typename V>
bool SkipList<K, V>::del(const K& key)
{
    Node<K, V>* current = _header;
    Node<K, V>* target = nullptr;
    for (int i = _skiplist_level; i >= 0; --i) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        if (current->forward[i] && current->forward[i]->get_key() == key) {
            target = current->forward[i];
            break;
        }
    }
    if (target == nullptr)  return false;

    int target_level = target->node_level;
    Node<K, V>* update[target_level + 1];
    current = _header;
    for (int i = target_level; i >= 0; --i) {
        while (current->forward[i]->get_key() != key) {
            current = current->forward[i];
        }
        current->forward[i] = target->forward[i];
    }
    delete target;
    --_element_count;
    return true;
}




int main() {
    int N;
    int K;
    int M;
    SkipList<int, int> *skip_list = new SkipList<int, int>(16);
    std::cin >> N >> K >> M;

    // insert
    for (int i = 0; i < N; i++) {
        int key;
        int value;
        std::cin >> key >> value;
        if (skip_list->insert(key, value)) {
            std::cout << "Insert Success" << std::endl;
        } else {
            std::cout << "Insert Failed" << std::endl;
        }
    }

    // del
    for (int i = 0; i < K; i++) {
        int k;
        std::cin >> k;
        skip_list->del(k);
    }


    // search
    for (int i = 0; i < M; i++) {
        int key;
        std::cin >> key;
        if (skip_list->search(key)) {
            std::cout << "Search Success" << std::endl;
        } else {
            std::cout << "Search Failed" << std::endl;
        }
    }
    return 0;
}
