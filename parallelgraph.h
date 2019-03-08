#ifndef PARALLELGRAPH_H
#define PARALLELGRAPH_H

#include "tbb/flow_graph.h"
#include <memory>
#include <shared_mutex>
#include <random>

using namespace tbb::flow;

class ParallelGraph
{
public:
    ParallelGraph(size_t maxParallel = 8);

    void exec();

private:
    struct Message {
        unsigned int id;
    };

    size_t m_maxParallel = 8;
    std::unique_ptr<graph> m_graph;
    std::unique_ptr<function_node<Message>> m_computeNode;
    std::unique_ptr<limiter_node<Message>> m_limit;
    std::unique_ptr<sequencer_node<Message>> m_ordering;

    std::random_device m_randomDevice {};
    std::mt19937 m_generator {m_randomDevice()};
    std::normal_distribution<> m_distribution {0, 1};

    mutable std::shared_mutex m_mutex;

    void processAsync(const size_t id, float* data);
};

#endif // PARALLELGRAPH_H
