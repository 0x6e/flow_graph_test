#ifndef SIMPLEGRAPH_H
#define SIMPLEGRAPH_H

#include "tbb/flow_graph.h"
#include <memory>

using namespace tbb::flow;

class SimpleGraph
{
public:
    SimpleGraph();

    void exec();

private:
    struct Message {
        int id;
        int data;
    };

    std::unique_ptr<graph> m_graph;
    std::unique_ptr<function_node<Message, Message>> m_process;
    std::unique_ptr<function_node<Message>> m_writer;
};

#endif // SIMPLEGRAPH_H
