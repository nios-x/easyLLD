    class Node {
        public:
        int key;
        int val;
        Node* next;
        Node* prev;
        Node(int key, int val){
            this->key = key;
            this->val = val;
            this->next = NULL;
            this->prev = NULL;
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
        Node* left = new Node(-1, -1);
        Node* right = new Node(-1, -1);
        LRUCache(int capacity) {
            this->capacity = capacity;
            this->current = 0;
            left->next = right;
            right->prev = left;
        }
        Node* addNode(int key, int val){
            Node* node = new Node(key, val);
            Node* pre = left->next;
            left->next = node;
            node->next = pre;
            pre->prev = node;
            node->prev = left;   
            return node;
        }
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
                delNode(e);
                mp[key] = addNode(key, val);
                return val;
            }
            return -1;
        }
        void put(int key, int value) {
            if(mp.find(key)!=mp.end()){
                Node* e = mp[key];
                delNode(e);
                mp[key] = addNode(key, value);
                return;
            }
            if(current==capacity){
                mp.erase(right->prev->key);
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