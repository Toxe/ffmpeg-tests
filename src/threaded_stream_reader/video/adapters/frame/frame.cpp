#include "frame.hpp"

double Frame::timestamp() const
{
    return timestamp_;
}

void Frame::set_timestamp(const double timestamp)
{
    timestamp_ = timestamp;
}
