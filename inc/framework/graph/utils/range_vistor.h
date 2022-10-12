/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: range_vistor
 */

#ifndef GE_RANGE_VISTOR_H
#define GE_RANGE_VISTOR_H

#include <vector>

namespace ge {
template <class E, class O>
class RangeVistor {
public:
    using Iterator = typename std::vector<E>::iterator;
    using ConstIterator = typename std::vector<E>::const_iterator;

public:
    RangeVistor(O owner, const std::vector<E>& vs) : owner_(owner), elements_(vs)
    {
    }

    ~RangeVistor()
    {
    }

    Iterator begin()
    {
        return elements_.begin();
    }

    Iterator end()
    {
        return elements_.end();
    }

    ConstIterator begin() const
    {
        return elements_.begin();
    }

    ConstIterator end() const
    {
        return elements_.end();
    }

    std::size_t size() const
    {
        return elements_.size();
    }

    bool empty() const
    {
        return elements_.empty();
    }

    E& at(std::size_t index)
    {
        return elements_.at(index);
    }

    const E& at(std::size_t index) const
    {
        return elements_.at(index);
    }

private:
    O owner_;
    std::vector<E> elements_;
};
} // namespace ge
#endif // GE_RANGE_VISTOR_H
