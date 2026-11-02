    // LeetCode 146. LRU Cache
    // Hash map (key -> node) gives O(1) lookup.
    // Doubly linked list keeps the order of use: most recent next to left, least recent next to right.
    // get() and put() are both O(1).
    class Node {
        public:
        int key;
        int val;
        Node* next;
        Node* prev;
        Node(int key, int val){
            this->key = key;
            this->val = val;
            this->next = nullptr;
            this->prev = nullptr;
        }
        Node(int key, int val, Node* prev, Node* next){
            this->key = key;
            this->val = val;
            this->prev = prev;
            this->next = next;
        }    
    };
    class LRUCache {
    public:
        int capacity;
        int current;
        unordered_map<int,Node*> mp;
        Node* left = new Node(-1, -1);    // dummy head: avoids null checks when adding/removing
        Node* right = new Node(-1, -1);   // dummy tail
        LRUCache(int capacity) {
            this->capacity = capacity;
            this->current = 0;
            left->next = right;
            right->prev = left;
        }
        ~LRUCache() {                     // free every node, including the two dummies
            Node* cur = left;
            while(cur){
                Node* nxt = cur->next;
                delete cur;
                cur = nxt;
            }
        }
        // Inserts a new node right after left (marks it as most recently used).
        Node* addNode(int key, int val){
            Node* node = new Node(key, val);
            Node* pre = left->next;
            left->next = node;
            node->next = pre;
            pre->prev = node;
            node->prev = left;   
            return node;
        }
        // Unlinks a node from the list and frees it.
        void delNode(Node* node){
            Node* pre = node->prev;
            Node* post = node->next;
            pre->next = post;
            post->prev = pre;
            delete node;
        }
        int get(int key) {
            if(mp.find(key)!=mp.end()){
                Node* e = mp[key];
                int val = e->val;
                delNode(e);                     // move to front: remove ...
                mp[key] = addNode(key, val);    // ... and add again as most recent
                return val;
            }
            return -1;
        }
        void put(int key, int value) {
            if(mp.find(key)!=mp.end()){         // key exists: update value and move to front
                Node* e = mp[key];
                delNode(e);
                mp[key] = addNode(key, value);
                return;
            }
            if(current==capacity){              // full: evict the least recently used (right->prev)
                mp.erase(right->prev->key);     // erase from map first, delNode frees the node
                delNode(right->prev);
            }else current++;
            mp[key] = addNode(key, value);
        }
    };

    /**
    * Your LRUCache object will be instantiated and called as such:
    * LRUCache* obj = new LRUCache(capacity);
    * int param_1 = obj->get(key);
    * obj->put(key,value);
    */
