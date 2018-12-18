#include "simplesequencegraph.h"

#include <iostream>

SimpleSequenceGraph::SimpleSequenceGraph()
    : m_graph(new graph)
{
    // Just waste some cpu cycles - simulate decompressing columns
    auto f = [] (Message msg) -> Message {
        for (int i = 0; i < 200000000; ++i) {
            msg.data++;
            msg.data--;
            msg.data++;
        }
        return msg;
    };
    // Due to parallelism (limited only by number of threads here), the node can
    // push messages to its successors in any order
    m_process.reset(new function_node<Message, Message>(*m_graph, unlimited, f));

    // The sequencer node enforces the correct ordering based upon the message id
    m_ordering.reset(new sequencer_node<Message>(*m_graph, [] (const Message& msg) -> int {
        return msg.id;
    }));

    auto g = [] (const Message& msg) {
        std::cout << "Message recieved with id: " << msg.id << std::endl;
    };
    // The writer is enforced to behave in serial manner - simulate the processing of a column
    m_writer.reset(new function_node<Message>(*m_graph, serial, g));

    make_edge(*m_process, *m_ordering);
    make_edge(*m_ordering, *m_writer);
}

void SimpleSequenceGraph::exec()
{
    // Push some messages into the top of the graph to be processed - representing the column indices
    for (int i = 0; i < 100; ++i) {
        Message msg = { i, 0 };
        m_process->try_put(msg);
    }

    // Wait for the graph to complete
    m_graph->wait_for_all();
}
