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
    using DataPtr = std::shared_ptr<float[]>;

    struct Message {
        unsigned int id = 0;
        DataPtr data = nullptr;
        double beta = 0;
    };

    size_t m_maxParallel = 8;
    std::unique_ptr<graph> m_graph;
    std::unique_ptr<function_node<Message, Message>> m_computeNode;
    std::unique_ptr<limiter_node<Message>> m_limit;
    std::unique_ptr<sequencer_node<Message>> m_ordering;
    std::unique_ptr<function_node<Message>> m_globalComputeNode;

    using decision_node = multifunction_node<Message, tbb::flow::tuple<continue_msg, Message> >;
    std::unique_ptr<decision_node> m_decisionNode;

    std::random_device m_randomDevice {};
    std::mt19937 m_generator {m_randomDevice()};
    std::normal_distribution<> m_distribution {0, 1};

    mutable std::shared_mutex m_mutex;

    double processAsync(const size_t id, const DataPtr &data);
};

#endif // PARALLELGRAPH_H
