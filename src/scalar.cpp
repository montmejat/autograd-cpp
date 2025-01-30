#include "scalar.h"
#include <fstream>
#include <tuple>

namespace autograd
{
    int Scalar::_next_id = 0;

    Scalar Scalar::operator+(const Scalar &other) const
    {
        return Scalar(_data + other._data, std::make_shared<Scalar>(*this), std::make_shared<Scalar>(other));
    }

    Scalar Scalar::operator+(const float other) const
    {
        return Scalar(_data + other, std::make_shared<Scalar>(*this), std::make_shared<Scalar>(Scalar(other)));
    }

    Scalar operator+(const float left, const Scalar &right)
    {
        return Scalar(left + right._data, std::make_shared<Scalar>(Scalar(left)), std::make_shared<Scalar>(right));
    }

    std::ostream &operator<<(std::ostream &ostream, Scalar &scalar)
    {
        std::vector<Scalar> stack;
        Scalar::ordered_graph(scalar, stack);

        for (auto &s : stack)
            ostream << s.data() << " ";

        return ostream;
    }

    void Scalar::ordered_graph(Scalar &scalar, std::vector<Scalar> &stack)
    {
        if (!scalar.visited)
        {
            scalar.visited = true;

            if (scalar._left.has_value())
                ordered_graph(*scalar._left.value(), stack);
            if (scalar._right.has_value())
                ordered_graph(*scalar._right.value(), stack);

            stack.push_back(scalar);
        }
    }

    void Scalar::json(const std::string &filename)
    {
        std::vector<std::tuple<int, int, int>> nodes;
        std::vector<std::tuple<int, int>> edges;

        std::vector<std::tuple<std::shared_ptr<Scalar>, int>> stack = {{std::make_shared<Scalar>(*this), 0}};
        while (stack.size())
        {
            std::shared_ptr<Scalar> node;
            int depth;

            std::tie(node, depth) = stack.back();
            stack.pop_back();

            if (node->_left.has_value())
            {
                stack.push_back({node->_left.value(), depth + 1});
                edges.push_back({node->id, node->_left.value()->id});
            }

            if (node->_right.has_value())
            {
                stack.push_back({node->_right.value(), depth + 1});
                edges.push_back({node->id, node->_right.value()->id});
            }

            nodes.push_back({node->id, node->_data, depth});
        }

        std::ofstream file(filename);

        file << "{\n    \"nodes\": [\n";
        for (int i = 0; i < nodes.size(); i++)
        {
            int id;
            int data;
            int depth;
            std::tie(id, data, depth) = nodes[i];

            file << "        { \"id\": " << id << ", "
                 << "\"label\": \"" << data << "\", "
                 << "\"depth\": " << depth << " }"
                 << (i == nodes.size() - 1 ? "\n" : ",\n");
        }

        file << "    ],\n    \"edges\": [\n";
        for (int i = 0; i < edges.size(); i++)
        {
            int source;
            int target;
            std::tie(source, target) = edges[i];

            file << "        { \"source\": " << source << ", "
                 << "\"target\": " << target << " }"
                 << (i == edges.size() - 1 ? "\n" : ",\n");
        }

        file << "    ]\n}\n";
        file.close();
    }
}
