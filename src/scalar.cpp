#include "scalar.h"
#include <fstream>
#include <tuple>

namespace autograd
{
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
        std::vector<int> nodes;
        std::vector<std::tuple<int, int>> edges;

        std::vector<Scalar> stack = {*this};
        while (stack.size())
        {
            Scalar node = stack.back();
            stack.pop_back();

            if (node._left.has_value())
            {
                stack.push_back(*node._left.value());
                edges.push_back({node._data, node._left.value()->_data});
            }

            if (node._right.has_value())
            {
                stack.push_back(*node._right.value());
                edges.push_back({node._data, node._right.value()->_data});
            }

            nodes.push_back(node._data);
        }

        std::ofstream file(filename);

        file << "{\n    \"nodes\": [\n";
        for (int i = 0; i < nodes.size() - 1; i++)
            file << "        { \"id\": " << nodes[i] << ", \"label\": \"" << nodes[i] << "\" },\n";
        file << "        { \"id\": " << nodes.back() << ", \"label\": \"" << nodes.back() << "\" }\n    ],\n";

        file << "    \"edges\": [\n";
        for (int i = 0; i < edges.size() - 1; i++)
            file << "        { \"from\": " << std::get<0>(edges[i]) << ", \"to\": " << std::get<1>(edges[i]) << " },\n";
        file << "        { \"from\": " << std::get<0>(edges.back()) << ", \"to\": " << std::get<1>(edges.back()) << " }\n    ]\n}";

        file.close();
    }
}
