#include <memory>
#include <optional>

namespace autograd
{
    class Scalar
    {
        using OptionalScalar = std::optional<std::shared_ptr<autograd::Scalar>>;

        float _data;
        float _grad;
        OptionalScalar _left;
        OptionalScalar _right;

        Scalar(float data, OptionalScalar left, OptionalScalar right)
            : _data(data), _grad(0.0), _left(left), _right(right) {};

    public:
        Scalar(float data) : _data(data), _grad(0.0) {};

        float data() { return _data; };
        float grad() { return _grad; };
        OptionalScalar left() { return _left; };
        OptionalScalar right() { return _right; };

        Scalar operator+(const Scalar &other) const
        {
            return Scalar(_data + other._data, std::make_shared<Scalar>(*this), std::make_shared<Scalar>(other));
        };

        Scalar operator+(const float other) const
        {
            return Scalar(_data + other, std::make_shared<Scalar>(*this), std::nullopt);
        };

        friend Scalar operator+(const float left, const Scalar &right)
        {
            return Scalar(left + right._data, std::nullopt, std::make_shared<Scalar>(right));
        };
    };
}
