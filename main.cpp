#include <iostream>
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <iterator>

template <typename T>
struct Node{
    T value;
    Node* next;
    Node* prev;
    Node(const T& value) : value(value), next(nullptr), prev(nullptr){}
    Node(const T& value, Node *next) : value(value), next(next), prev(nullptr){}
};

template <typename T>
class lnklist {
private:
    size_t size1;
    Node<T> *head;
    Node<T> *tail;
public:
    lnklist() : head(nullptr), tail(nullptr), size1(0){}
    lnklist(size_t n,const T& value) : head(nullptr), tail(nullptr), size1(0){
        while(n--){
            push_back(value);
        }
    }
    lnklist(std::initializer_list<T> list) : head(nullptr), tail(nullptr), size1(0){
        for(const T& value : list){
            push_back(value);
        }
    }
    lnklist(const lnklist& list) : head(nullptr), tail(nullptr), size1(0){
        for(Node<T>* i = list.head; i; i = i->next){
            push_back(i->value);
        }
    }
    lnklist(lnklist&& lst) : head(lst.head), tail(lst.tail), size1(lst.size1){
        lst.head = nullptr;
        lst.tail = nullptr;
        lst.size1 = 0;
    }
    lnklist& operator=(const lnklist& list){
        if(this != &list){
            clear();
            for(Node<T>* i = list.head; i; i = i->next){
                push_back(i->value);
            }
        }
        return *this;
    }
    lnklist& operator=(lnklist&& lst){
        if(this != &lst){
            clear();
            head = lst.head;
            tail = lst.tail;
            size1 = lst.size1;
            lst.head = nullptr;
            lst.tail = nullptr;
            lst.size1 = 0;
        }
        return *this;
    }
    ~lnklist(){
        clear();
    }

    class Iterator{
    private:
        Node<T>* it;
    public:
        explicit Iterator(Node<T>* node) : it(node){}
        Iterator() : it(nullptr){}

        friend class lnklist<T>;

        Iterator& operator++() {
            if (it) it = it->next;
            return *this;
        }

        Iterator& operator--(){
            it = it->prev;
            return *this;
        }
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Iterator operator--(int){
            Iterator temp = *this;
            it = it->prev;
            return temp;
        }

        bool operator==(const Iterator& t) const { return it == t.it; }
        bool operator!=(const Iterator& t) const { return !(*this == t); }

        T& operator*(){
            return it->value;
        }
    };

    Iterator begin() const {
        return Iterator(head);
    }
    Iterator end() const {
        return Iterator(nullptr);
    }

    void push_back(const T& value){
        Node<T>* newNode = new Node<T>(value);
        if(!tail){
            head = tail = newNode;
        }
        else{
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        ++size1;
    }
    void pop_back() {
        if (!tail) {
            return;
        }
        Node<T>* temp = tail;
        tail = tail->prev;
        if (!tail) {
            head = nullptr;
        } else {
            tail->next = nullptr;
        }
        delete temp;
        --size1;
    }
    T front(){
        return head->value;
    }
    T back(){
        return tail->value;
    }
    bool empty() const{
        return size1 == 0;
    }
    size_t size() const{
        return size1;
    }
    void clear() {
        while (head != nullptr) {
            pop_back();
        }
    }

    void push_front(const T& value) {
        Node<T>* newNode = new Node<T>(value);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        ++size1;
    }
    void pop_front(){
        if(!head) {
            return;
        }
        else{
            Node<T>* temp = head;
            head = head->next;
            if(!head){
                tail = nullptr;
            }
            else{
                head->prev = nullptr;
            }
            delete temp;
            --size1;
        }
    }
    void resize(size_t n, const T& value){
        while(size1 > n){
            pop_back();
        }
        while (size1 < n){
            push_back(value);
        }
    }
    Iterator erase(Iterator it) {
        Node<T>* temp = it.it;
        if (temp->prev) {
            temp->prev->next = temp->next;
        } else {
            head = temp->next;
        }
        if (temp->next) {
            temp->next->prev = temp->prev;
        } else {
            tail = temp->prev;
        }

        Node<T>* next_node = temp->next;
        delete temp;
        --size1;
        if (size1 == 0) {
            tail = nullptr;
        }

        return Iterator(next_node);
    }


    Iterator erase(Iterator first, Iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return first;
    }

    Iterator insert(Iterator pos, const T& value) {
        if (pos == begin()) {
            push_front(value);
            return begin();
        }
        if (pos == end()) {
            push_back(value);
            return Iterator(tail);
        }

        Node<T>* newNode = new Node<T>(value);
        Node<T>* temp = pos.it;

        newNode->next = temp;
        newNode->prev = temp->prev;
        temp->prev->next = newNode;
        temp->prev = newNode;
        ++size1;

        return Iterator(newNode);
    }
    Iterator insert(Iterator pos, size_t len,const T& value){
        while(len--){
            insert(pos, value);
        }
        return pos;
    }

    Iterator insert(Iterator pos, std::initializer_list<T> list) {
        for (const auto& el : list) {
            pos = insert(pos, el);
            ++pos;
        }
        return pos;
    }

    void reverse() {
        Node<T>* current = head;
        Node<T>* temp = nullptr;
        while (current) {
            temp = current->prev;
            current->prev = current->next;
            current->next = temp;
            current = current->prev;
        }
        if (temp) {
            tail = head;
            head = temp->prev;
        }
    }

    void splice(Iterator pos, lnklist& other, Iterator first, Iterator last) {
        if (first == last) {
            return;
        }

        Node<T>* firstNode = first.it;
        Node<T>* lastNode = last.it ? last.it->prev : nullptr;
        if (!firstNode || !lastNode) {
            return;
        }
        if (firstNode->prev) {
            firstNode->prev->next = last.it;
        } else {
            other.head = last.it;
        }

        if (lastNode->next) {
            lastNode->next->prev = first.it->prev;
        } else {
            other.tail = first.it->prev;
        }

        if (pos.it) {
            lastNode->next = pos.it;
            pos.it->prev = lastNode;
        } else {
            if (tail) {
                tail->next = firstNode;
                firstNode->prev = tail;
            } else {
                head = firstNode;
            }
            tail = lastNode;
        }

        size1 += other.size1;
        other.size1 = 0;

        other.head = other.tail = nullptr;
    }

    void merge(lnklist& other) {
        if (this == &other) return;

        Node<T>* current1 = head;
        Node<T>* current2 = other.head;

        while (current1 && current2) {
            if (current2->value < current1->value) {
                Node<T>* temp = current2;
                current2 = current2->next;
                insert(Iterator(current1), temp->value);
            } else {
                current1 = current1->next;
            }
        }

        while (current2) {
            Node<T>* temp = current2;
            current2 = current2->next;
            insert(Iterator(current1), temp->value);
        }
        other.clear();
    }

};

void test_push_back() {
    lnklist<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    // Проверяем размер и элементы
    assert(list.size() == 3);
    assert(list.front() == 1);
    assert(list.back() == 3);

    std::cout << "test_push_back прошел проверку" << std::endl;
}

void test_push_front() {
    lnklist<int> list;
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);

    // Проверяем размер и элементы
    assert(list.size() == 3);
    assert(list.front() == 3);
    assert(list.back() == 1);

    std::cout << "test_push_front прошел проверку" << std::endl;
}

void test_pop_back() {
    lnklist<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.pop_back();

    assert(list.size() == 2);
    assert(list.back() == 2);

    std::cout << "test_pop_back прошел проверку" << std::endl;
}

void test_pop_front() {
    lnklist<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.pop_front();

    assert(list.size() == 2);
    assert(list.front() == 2);

    std::cout << "test_pop_front прошел проверку" << std::endl;
}

void test_insert() {
    lnklist<int> list = {1, 2, 3};
    auto it = list.begin();
    ++it;
    list.insert(it, 4);
    assert(list.size() == 4);
    assert(list.front() == 1);
    assert(list.back() == 3);

    it = list.begin();
    ++it;
    assert(*it == 4);

    std::cout << "test_insert прошел проверку" << std::endl;
}

void test_erase() {
    lnklist<int> list = {1, 2, 3, 4, 5};
    auto it = list.begin();
    ++it;
    ++it;
    auto erased_it = it;
    ++it;
    list.erase(erased_it);
    assert(list.size() == 4);

    std::cout << "test_erase прошел проверку" << std::endl;
}


void test_clear() {
    lnklist<int> list = {1, 2, 3};
    list.clear();

    assert(list.size() == 0);
    assert(list.empty() == true);

    std::cout << "test_clear прошел проверку" << std::endl;
}

void test_resize() {
    lnklist<int> list = {1, 2, 3};
    list.resize(5, 0);

    assert(list.size() == 5);
    assert(list.back() == 0);

    list.resize(2, 0);
    assert(list.size() == 2);
    assert(list.back() == 2);

    std::cout << "test_resize прошел проверку" << std::endl;
}

void test_reverse() {
    lnklist<int> list = {1, 2, 3, 4};
    list.reverse();

    auto it = list.begin();
    assert(*it == 4);
    ++it;
    assert(*it == 3);
    ++it;
    assert(*it == 2);
    ++it;
    assert(*it == 1);

    std::cout << "test_reverse прошел проверку" << std::endl;
}

void test_merge() {
    lnklist<int> list1 = {1, 3, 5};
    lnklist<int> list2 = {2, 4, 6};
    list1.merge(list2);

    assert(list1.size() == 6);
    assert(list2.size() == 0);
    assert(list1.front() == 1);
    assert(list1.back() == 6);

    std::cout << "test_merge прошел проверку" << std::endl;
}


int main() {
    test_push_back();
    test_push_front();
    test_pop_back();
    test_pop_front();
    test_insert();
    test_erase();
    test_clear();
    test_resize();
    test_reverse();
    test_merge();

    std::cout << "Все тесты прошли проверку!" << std::endl;
    return 0;
}