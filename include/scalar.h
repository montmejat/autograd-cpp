#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace autograd
{
    class Scalar
    {
        using OptionalScalar = std::optional<std::shared_ptr<autograd::Scalar>>;

        float _data;
        float _grad;
        OptionalScalar _left;
        OptionalScalar _right;
        bool visited = false;

        Scalar(float data, OptionalScalar left, OptionalScalar right)
            : _data(data), _grad(0.0), _left(left), _right(right) {};
        static void ordered_graph(Scalar &scalar, std::vector<Scalar> &stack);

    public:
        Scalar(float data) : _data(data), _grad(0.0) {};

        float data() { return _data; };
        float grad() { return _grad; };
        OptionalScalar left() { return _left; };
        OptionalScalar right() { return _right; };

        Scalar operator+(const Scalar &other) const;
        Scalar operator+(const float other) const;
        friend Scalar operator+(const float left, const Scalar &right);

        friend std::ostream &operator<<(std::ostream &os, Scalar &scalar);

        void json(const std::string &filename = "graph.json");
    };
}
