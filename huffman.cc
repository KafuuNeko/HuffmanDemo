#include "huffman.hpp"

using namespace huffman;

constexpr unsigned int kDataSize = static_cast<DataType>(-1) + 1;

/**
 * 释放一个哈夫曼树节点及其此节点的所有子孙
*/
void FreeTree(TreeNode *node)
{
    if (node != nullptr)
    {
        FreeTree(node->left);
        FreeTree(node->right);
        delete node;
    }
}

/**
 * 哈夫曼树构造函数
 * @param input 用于构造哈夫曼树的数据
 * @param size 用于构造哈夫曼树的数据的大小
*/
Tree::Tree(std::istream &in):
    root_(new TreeNode*, [](TreeNode **tree_node) { 
        FreeTree(*tree_node); 
        delete tree_node;
    })
{
    AmountType *statistics = new AmountType[kDataSize];
    std::fill(statistics, statistics + kDataSize, 0);
    
    //统计数据出现的次数
    in.seekg(in.beg);
    char in_data;
    while (in.get(in_data)) ++statistics[(DataType)in_data];
    //恢复流的状态
    in.seekg(in.beg);
    in.clear(std::istream::eofbit);
    
    Queue huffman_queue;

    for (DataType i = 0; ; ++i)
    {
        if (statistics[i] != 0) 
        {
            huffman_queue.AddNode(new TreeNode {i, nullptr, nullptr}, statistics[i]);
        }

        if (i == kDataSize - 1) break;
        
    }

    delete[] statistics;

    *this->root_ = huffman_queue.CreateTree();
}

/**
 * 根据权重，顺序添加，自动排序
 * @param tree_node 要添加的哈夫曼树节点
 * @param weight 权重，为此数据出现的次数
*/
void Queue::AddNode(TreeNode *tree_node, AmountType weight)
{
    QueueNode *new_node = new QueueNode{tree_node, weight, nullptr};

    if (this->size_ == 0)
    {
        //如果队列为空，则first指向新节点
        this->first_ = new_node;
        this->size_ = 1;
    }
    else
    {
        if (this->first_->weight > weight)
        {
            //队列第一个节点大于新节点，则直接用新节点作为第一个节点
            new_node->next = this->first_;
            this->first_ = new_node;
            ++(this->size_);
        }
        else
        {
            auto it = this->first_;
            for (; it->next != nullptr; it = it->next)
            {
                if (it->next->weight > weight)
                {
                    new_node->next = it->next;
                    it->next = new_node;
                    ++(this->size_);
                    return;
                }
            }
            it->next = new_node;
            ++(this->size_);
        }
          
    }
}

/**
 * 弹出位于队列顶端的节点(最小节点)
 * 弹出后此节点将被释放
 * @return 若成功弹出，则返回顶端的哈夫曼树节点，失败返回空指针
*/
TreeNode *Queue::PopTop()
{
    TreeNode *result = nullptr;
    
    if (this->size_ != 0)
    {
        QueueNode *queue_node = this->first_;
        result = queue_node->tree_node;
        this->first_ = queue_node->next;
        --(this->size_);
        delete queue_node;
    }

    return result;
}

/**
 * 通过此队列创建哈夫曼树
 * 当创建完成后，队列被清空
*/
TreeNode *Queue::CreateTree()
{
    while (this->size_ > 1)
    {
        AmountType new_weight = 0;

        TreeNode *new_tree_node = new TreeNode{0, nullptr, nullptr};

        new_weight += this->first_->weight;
        new_tree_node->left = this->PopTop();

        new_weight += this->first_->weight;
        new_tree_node->right = this->PopTop();
        
        this->AddNode(new_tree_node, new_weight);
    }

    if (this->first_ == nullptr)
    {
        return nullptr;
    }

    if (this->first_->tree_node->left == nullptr)
    {
        this->first_->tree_node = new TreeNode{'\0', this->first_->tree_node, nullptr};
    }
    
    auto result = this->first_->tree_node;
    
    //创建完成，清空队列
    delete this->first_;
    this->size_ = 0;
    this->first_ = nullptr;

    return result;
}

/**
 * 队列析构函数
*/
Queue::~Queue()
{
    auto it = this->first_;
    while (it != nullptr)
    {
        auto next = it->next;
        FreeTree(it->tree_node);
        delete it;
        it = next;
    }
}

/**
 * 构造哈夫曼编码表递归函数
*/
void Tree::__MakeTable(TableType &table, std::vector<bool> &code, TreeNode *node)
{
    if (node != nullptr)
    {
        if (node->left == nullptr)
        {
            table[node->data] = code;
        }
        else
        {
            code.push_back(false);

            this->__MakeTable(table, code, node->left);
            *(code.end()-1) = true;
            this->__MakeTable(table, code, node->right);
            
            code.erase(code.end()-1);
        }
    }
}

/**
 * 构造哈夫曼编码表
*/
TableType Tree::MakeTable()
{
    TableType table;    
    std::vector<bool> code_buf;
    __MakeTable(table, code_buf, *this->root_);
    return table;
}

