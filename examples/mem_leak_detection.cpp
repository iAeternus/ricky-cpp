/**
 * @brief 内存泄露检测
 * @author Ricky
 * @date 2025/8/14
 * @version 1.0
 */
#include "bi_chain.hpp"
#include "chain.hpp"
#include "dyn_array.hpp"
#include "link_list_queue.hpp"
#include "rbtree_map.hpp"
#include "str.hpp"
#include "tracing_alloc.hpp"
#include "vec.hpp"

using namespace my;

/**
 * @brief 是否开启详细打印模式
 * 1 = 开启
 * 0 = 关闭
 */
#define VERBOSE 1

/**
 * @brief 追踪对象
 * 1 = CString
 * 2 = util::String
 * 3 = util::Vec
 * 4 = util::Chain
 * 5 = util::BiChain
 * 6 = util::HashMap
 * 7 = util::RBTreeMap
 * 8 = util::DynArray
 * 9 = util::Array
 * 10 = util::Queue
 */
#define TRACE_OBJECT 6

void trace_cstring() {
    using TraceCString = BasicCString<mem::TracingAllocator<char>>;
#if VERBOSE == 1
    mem::TracingAllocator<char>::set_verbose(true);
#endif

    // 构造
    TraceCString c1(10);
    TraceCString c2("abc");

    // 拷贝
    TraceCString c3(c1);
    TraceCString c4 = c2;

    // 移动
    TraceCString c5(std::move(c3));
    TraceCString c6 = std::move(c4);
}

// TODO 存在内存泄漏问题！段错误！
void trace_string() {
    using TraceString = util::BasicString<util::EncodingType::UTF8, mem::TracingAllocator<char>>;
    using Utf16TraceString = util::BasicString<util::EncodingType::UTF16, mem::TracingAllocator<char>>;
#if VERBOSE == 1
    mem::TracingAllocator<util::CodePoint<>>::set_verbose(true);
#endif

    util::Vec<char> chs = {'a', 'b', 'c', 'd', 'e', 'f'};

    // 构造
    TraceString c1("你好世界ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    TraceString c2("你好世界abcdef"_cs);
    Utf16TraceString c3("你好世界abcdef"_cs);
    TraceString c4("我");
    TraceString c5(10, 'a');
    TraceString c6(chs.begin(), chs.end());

    // 拷贝
    TraceString c7(c5);
    TraceString c8 = c6;

    // 移动
    TraceString c9(std::move(c7));
    TraceString c10 = std::move(c8);

    // +
    TraceString c11;
    for (i32 i = 0; i < 1024; ++i) {
        c11 += c2;
    }

    // clear
    c4.clear();

    // swap
    c1.swap(c2);
    c2.swap(c1);
}

void trace_vec() {
    using TraceVec = util::Vec<i32, mem::TracingAllocator<i32>>;
#if VERBOSE == 1
    mem::TracingAllocator<i32>::set_verbose(true);
#endif

    // 构造
    TraceVec v(10, 99);
    TraceVec v2 = {1, 2, 3};

    // 拷贝
    TraceVec v3(v);
    TraceVec v4 = v2;

    // 移动
    TraceVec v5(std::move(v3));
    TraceVec v6 = std::move(v4);

    // append
    for (i32 i = 0; i < 1024; ++i) {
        v2.append(i);
    }

    // pop
    for (i32 i = 0; i < 512; ++i) {
        v2.pop();
    }

    // swap
    v.swap(v2);
    v2.swap(v);
}

void trace_chain() {
    using TraceChainList = util::ChainList<i32, mem::TracingAllocator<util::ChainNode<i32>>>;
#if VERBOSE == 1
    mem::TracingAllocator<util::ChainNode<i32>>::set_verbose(true);
#endif

    // 构造
    TraceChainList c1;

    // append
    for (i32 i = 0; i < 1024; ++i) {
        c1.append(i);
    }

    // clear
    c1.clear();
}

void trace_bi_chain() {
    using TraceBiChainList = util::BiChainList<i32, mem::TracingAllocator<util::BiChainNode<i32>>>;
#if VERBOSE == 1
    mem::TracingAllocator<util::BiChainNode<i32>>::set_verbose(true);
#endif

    // 构造
    TraceBiChainList bc1;

    // append
    for (i32 i = 0; i < 1024; ++i) {
        bc1.append(i);
    }

    // clear
    bc1.clear();

    // prepend
    for (i32 i = 0; i < 1024; ++i) {
        bc1.prepend(i);
    }
}

// template <>
// struct std::formatter<BasicCString<mem::TracingAllocator<char>>> : std::formatter<const char*> {
//     fn format(const BasicCString<mem::TracingAllocator<char>>& value, auto& ctx) const {
//         return std::formatter<const char*>::format(value.data(), ctx);
//     }
// };

void trace_hash_map() {
    // using TraceCString = BasicCString<mem::TracingAllocator<char>>;
    using TraceHashMap = util::HashMap<std::string, i32, mem::TracingAllocator<std::string>>; // TODO 用TraceCString会段错误
#if VERBOSE == 1
    mem::TracingAllocator<std::string>::set_verbose(true);
#endif

    // 构造
    TraceHashMap m1;
    TraceHashMap m2 = {{"aaa", 1}, {"bbb", 2}, {"ccc", 3}};

    // 拷贝
    TraceHashMap d3(m1);
    TraceHashMap d4 = m2;

    // 移动
    TraceHashMap d5(std::move(d3));
    TraceHashMap d6 = std::move(d4);

    // insert
    for (i32 i = 0; i < 1024; ++i) {
        m1.insert(std::to_string(i), i);
    }

    // update
    TraceHashMap d7, d8;
    d7.update(m2);
    d8.update(std::move(d7));

    // remove
    m2.remove("aaa");

    // clear
    m1.clear();
}

void trace_sorted_dict() {
    using TraceRBTreeMap = util::RBTreeMap<i32, i32, std::less<i32>, mem::TracingAllocator<util::RBTreeNode<i32, i32>>>;
#if VERBOSE == 1
    mem::TracingAllocator<util::RBTreeNode<i32, i32>>::set_verbose(true);
#endif

    // 构造
    TraceRBTreeMap t1;
    TraceRBTreeMap t2 = {{1, 1}, {2, 2}, {3, 3}};

    // 拷贝
    TraceRBTreeMap t3(t1);
    TraceRBTreeMap t4 = t2;

    // 移动
    TraceRBTreeMap t5(std::move(t3));
    TraceRBTreeMap t6 = std::move(t4);

    // insert
    for (i32 i = 0; i < 1024; ++i) {
        t1.insert(i, i);
    }

    // remove
    for (i32 i = 0; i < 512; ++i) {
        t1.remove(i);
    }

    // clear
    t1.clear();
}

void trace_dyn_array() {
    // using TraceDynArray = util::DynArray<i32, >;
}

void trace_array() {
    using TraceArray = util::Array<i32, mem::TracingAllocator<i32>>;
#if VERBOSE == 1
    mem::TracingAllocator<i32>::set_verbose(true);
#endif

    // 构造
    TraceArray a1(10, 99);
    TraceArray a2 = {1, 2, 3};

    // 拷贝
    TraceArray a3(a1);
    TraceArray a4 = a2;

    // 移动
    TraceArray a5(std::move(a3));
    TraceArray a6 = std::move(a4);

    // resize
    a1.resize(5);
    a2.resize(10);

    // write
    for (i32 i = 0; i < 5; ++i) {
        a1[i] = i;
    }
    for (i32 i = 0; i < 10; ++i) {
        a2[i] = i;
    }
}

void trace_queue() {
    using TraceQueue = util::ChainQueue<util::ChainNode<i32>, mem::TracingAllocator<util::ChainNode<i32>>>;
#if VERBOSE == 1
    mem::TracingAllocator<i32>::set_verbose(true);
#endif

    // 构造
    TraceQueue q1;

    // push
    for (i32 i = 0; i < 1024; ++i) {
        q1.push(i);
    }

    // pop
    for (i32 i = 0; i < 512; ++i) {
        q1.pop();
    }

    // clear
    q1.clear();
}

int main() {
#if TRACE_OBJECT == 1
    trace_cstring();
#elif TRACE_OBJECT == 2
    trace_string();
#elif TRACE_OBJECT == 3
    trace_vec();
#elif TRACE_OBJECT == 4
    trace_chain();
#elif TRACE_OBJECT == 5
    trace_bi_chain();
#elif TRACE_OBJECT == 6
    trace_hash_map();
#elif TRACE_OBJECT == 7
    trace_sorted_dict();
#elif TRACE_OBJECT == 8
    trace_dyn_array();
#elif TRACE_OBJECT == 9
    trace_array();
#elif TRACE_OBJECT == 10
    trace_queue();
#endif
    return 0;
}
