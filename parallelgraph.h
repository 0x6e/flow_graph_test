#ifndef PARALLELGRAPH_H
#define PARALLELGRAPH_H

#include "tbb/flow_graph.h"
#include <memory>

using namespace tbb::flow;

class ParallelGraph
{
public:
    ParallelGraph(size_t maxParallel = 8);

    void exec();

private:
    struct Message {
        unsigned int id;
        float* data;
    };

    size_t m_maxParallel = 8;
    std::unique_ptr<graph> m_graph;
    std::unique_ptr<function_node<Message>> m_computeNode;
    std::unique_ptr<limiter_node<Message>> m_limit;
    std::unique_ptr<sequencer_node<Message>> m_ordering;
};

#endif // PARALLELGRAPH_H
