/**
 * @brief `ricky-cpp` 项目 api 集成
 * @author Ricky
 * @date 2025/5/19
 * @version 1.0
 */
#ifndef RICKY_LIB_HPP
#define RICKY_LIB_HPP

/**
 * @brief 是否启用api整合包，以及是否启用扩展模块
 * @note 1-启用 0-不启用
 * @note 扩展模块：除util之外的模块，util属于核心模块始终包含
 */
#define RICKY_CPP_API 0
#define RICKY_CPP_AI 1
#define RICKY_CPP_ASYNC 1
#define RICKY_CPP_CORO 1
#define RICKY_CPP_FS 1
#define RICKY_CPP_GRAPH 1
#define RICKY_CPP_IO 1
#define RICKY_CPP_JSON 1
#define RICKY_CPP_MATH 1
#define RICKY_CPP_TEST 1

#if RICKY_CPP_API == 1

/**
 * @brief util
 */
#include "Array.hpp"
#include "BiChain.hpp"
#include "Chain.hpp"
#include "Creator.hpp"
#include "DateTime.hpp"
#include "Deleter.hpp"
#include "Dict.hpp"
#include "DisjointSet.hpp"
#include "Duration.hpp"
#include "DynArray.hpp"
#include "HuffmanTree.hpp"
#include "PriorityQueue.hpp"
#include "Queue.hpp"
#include "Random.hpp"
#include "SortedDict.hpp"
#include "Stack.hpp"
#include "StrategyDrivenIterator.hpp"
#include "Stream.hpp"
#include "String.hpp"
#include "StringBuilder.hpp"
#include "Timer.hpp"
#include "Tree.hpp"
#include "Vec.hpp"

/**
 * @brief ai
 */
#if RICKY_CPP_AI == 1

#endif // RICKY_CPP_AI

/**
 * @brief async
 */
#if RICKY_CPP_ASYNC == 1

#include "ThreadPool.hpp"

#endif // RICKY_CPP_ASYNC

/**
 * @brief coro
 */
#if RICKY_CPP_CORO == 1

#include "Generator.hpp"
#include "Promise.hpp"

#endif // RICKY_CPP_CORO

/**
 * @brief fs
 */
#if RICKY_CPP_FS == 1

#include "filesystem.hpp"

#endif // RICKY_CPP_FS

/**
 * @brief graph
 */
#if RICKY_CPP_GRAPH == 1

#include "Graph.hpp"
#include "graph_algorithm.hpp"

#endif // RICKY_CPP_GRAPH

/**
 * @brief io
 */
#if RICKY_CPP_IO == 1

#include "Log.hpp"
#include "Printer.hpp"

#endif // RICKY_CPP_IO

/**
 * @brief json
 */
#if RICKY_CPP_JSON == 1

#include "JsonParser.hpp"

#endif // RICKY_CPP_JSON

/**
 * @brief math
 */
#if RICKY_CPP_MATH == 1

#include "BigDecimal.hpp"
#include "BigInteger.hpp"
#include "Complex.hpp"
#include "Expr.hpp"
#include "geometry_2d.hpp"
#include "Matrix.hpp"
#include "Polygon.hpp"

#endif // RICKY_CPP_MATH

/**
 * @brief test
 */
#if RICKY_CPP_TEST == 1

#include "Assertions.hpp"
#include "UnitTest.hpp"

#endif // RICKY_CPP_TEST

#endif // RICKY_CPP_API

#endif // RICKY_LIB_HPP