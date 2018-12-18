#include "limitgraph.h"

#include <iostream>

LimitGraph::LimitGraph(size_t maxParallel)
    : m_maxParallel(maxParallel)
    , m_graph(new graph)
{
    // Just waste some cpu cycles and memory - simulate decompressing columns
    auto f = [] (Message msg) -> Message {
        std::cout << "Process node " << msg.id << std::endl;

        const size_t size = 20000000;
        msg.data = new float[size];
        for (auto i = 0U; i < size; ++i) {
            msg.data[i] = static_cast<float>(msg.id);
            msg.data[i]++;
            msg.data[i]--;
        }
        return msg;
    };
    // Do some work on up to maxParallel threads at once
    m_process.reset(new function_node<Message, Message>(*m_graph, m_maxParallel, f));

    // Do not allow predecessors to carry on blindly until later parts of
    // the graph have finished and freed up some resources.
    m_limit.reset(new limiter_node<Message>(*m_graph, m_maxParallel));

    auto g = [] (Message msg) -> continue_msg {
        std::cout << "Message recieved with id: " << msg.id << std::endl;
        delete[] msg.data;
        msg.data = nullptr;
        return continue_msg();
    };
    // The writer is enforced to behave in serial manner - simulate the processing of a column
    m_writer.reset(new function_node<Message>(*m_graph, serial, g));

    make_edge(*m_process, *m_limit);
    make_edge(*m_limit, *m_writer);

    // Feedback that we can now decompress another column
    make_edge(*m_writer, m_limit->decrement);
}

void LimitGraph::exec()
{
    // Push some messages into the top of the graph to be processed - representing the column indices
    for (int i = 0; i < 100; ++i) {
        Message msg = { i, nullptr };
        m_process->try_put(msg);
    }

    // Wait for the graph to complete
    m_graph->wait_for_all();
}
