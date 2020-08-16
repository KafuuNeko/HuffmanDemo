#pragma one
#include <map>
#include <vector>
#include <memory>
#include <ostream>
#include <istream>
#include <sstream>
#include <algorithm>

namespace huffman
{

typedef unsigned char DataType;
typedef unsigned int AmountType;
typedef std::map<DataType, std::vector<bool>> TableType;

//赫夫曼树节点结构
struct TreeNode
{
    DataType data;
    struct TreeNode *left, *right;
};

//队列节点结构
struct QueueNode
{
    TreeNode *tree_node;
    AmountType weight;
    struct QueueNode *next;
};

//赫夫曼树
class Tree {
    std::shared_ptr<TreeNode*> root_;
    void __MakeTable(TableType &table, std::vector<bool> &code, TreeNode *node);
public:
    Tree(std::istream &in);
    TableType MakeTable();
};

//队列
class Queue
{
    size_t size_;
    QueueNode *first_;

public:
    Queue(): size_(0), first_(nullptr) {}
    void AddNode(TreeNode *tree_node, AmountType weight);
    TreeNode *CreateTree();
    TreeNode *PopTop();

    ~Queue();

    //删除拷贝构造以及赋值操作
    Queue(const Queue &) = delete;
    Queue &operator=(const Queue &) = delete;
};

/* 以下是编码部分 */
inline bool compare_table_second(const std::vector<bool> &l, const std::vector<bool> &r)
{
    if (l.size() != r.size())
    {
        return false;
    }

    for (size_t i = 0; i < l.size(); i++)
    {
        if(l[i] != r[i]) return false;
    }
    
    return true;
}

inline std::vector<bool> encoding(const TableType &table, const DataType *data, size_t size)
{
    std::vector<bool> result;

    for (size_t i = 0; i < size; ++i)
    {
        auto iter = table.find(data[i]);
        if (iter != table.end()) 
        {
            std::copy(iter->second.begin(), iter->second.end(), std::back_inserter(result));  
        }
    }
    return result;
}


inline std::pair<TableType, std::vector<bool>> encoding_string(const std::string &data)
{
    std::stringstream data_ss(data.c_str());
    auto huffman_table = Tree(data_ss).MakeTable();
    return {std::move(huffman_table), std::move(encoding(huffman_table, reinterpret_cast<const DataType*>(data.c_str()), data.length()))};
}

inline std::string decode_string(const TableType &table, const std::vector<bool> &code)
{
    std::stringstream ss;

    std::vector<bool> buffer;
    for (bool bit : code)
    {
        buffer.push_back(bit);
        for (auto &tp : table)
        {
            if(compare_table_second(tp.second, buffer))
            {
                ss << tp.first;
                buffer.clear();
            }
        }
    }
    
    return ss.str();
}

inline void encoding_binary(std::istream &is, std::ostream &os)
{
    Tree huffman_tree(is);
    auto huffman_table = huffman_tree.MakeTable();
    
    char in_data;
    unsigned char out_data;
    unsigned char n = 0;
    while(is.get(in_data))
    {
        //查询哈夫曼编码表
        auto iter = huffman_table.find(in_data);
        if (iter != huffman_table.end())
        {
            for (auto codebit : iter->second)
            {
                //不断置位，当置位数达到八位就输出数据，并复位数据
                out_data = (out_data << 1) + (codebit?1:0);
                if (++n == 8)
                {
                    os.put(static_cast<char>(out_data));
                    n = 0;
                    out_data = 0;
                }
            }
        }
    }

    if (out_data)
    {
        //左移剩余位
        out_data <<= 8 - n;
        os.put(static_cast<char>(out_data));
    }

    os.flush();

}

}

