//
// Created by Aleksey Dorofeev on 14/02/2020.
//

#pragma once

#include <iostream>
#include <algorithm>
#include <type_traits>
#include <vector>

template <class CONT>
class Sequence {
public:
    using T = typename CONT::value_type;

    Sequence(CONT const& c, size_t MAX_SIZE) {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
        std::copy(std::begin(c), std::end(c), std::back_inserter(v));
        for(size_t i = v.size(); i < MAX_SIZE; i++)
            v.push_back(0);
    }
    inline std::vector<T> const& vec() const noexcept {
        return v;
    }
    template <class CONT2>
    std::vector<T> const& operator +=(CONT2 const& c) {
        return v;
    }

private:
    std::vector<T> v;
};

struct SequenceHelper {
    template <class CONT>
    static Sequence<CONT> CreateSequence(CONT const& c, size_t MAX_SIZE) {
        return Sequence<CONT>(c, MAX_SIZE);
    }
};

template <typename T, typename SEQ1, typename SEQ2>
std::vector<T> SumSequence(SEQ1 const& seq1, SEQ2 const& seq2) {
    std::vector<T> res;
    std::transform(seq1.begin(), seq1.end(), seq2.begin(), std::back_inserter(res),
                   [](typename SEQ1::value_type const& a, typename SEQ2::value_type const& b) -> T {
                       return a + b;
                   }
    );
    return res;
}

template <class CONT1, class CONT2>
auto SumCont(CONT1 const& cont1, CONT2 const& cont2) {
    size_t MAX_SIZE = std::max(cont1.size(), cont2.size());
    auto seq1 = SequenceHelper::CreateSequence(cont1, MAX_SIZE);//todo: do not make a copy
    auto seq2 = SequenceHelper::CreateSequence(cont2, MAX_SIZE);//todo: do not make a copy
    return SumSequence<typename CONT1::value_type>(seq1.vec(), seq2.vec());
}
