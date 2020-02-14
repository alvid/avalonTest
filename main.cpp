#include <iostream>
#include <sstream>
#include <vector>
#include <list>

//#include "is_stl_container.hpp"

// specialize a type for all of needed containers
namespace is_stl_container_impl {
    template<typename T>
    struct is_stl_container : std::false_type {
    };
    template <typename T, std::size_t N>
    struct is_stl_container<std::array <T,N>> : std::true_type {
    };
    template<typename... Args>
    struct is_stl_container<std::vector<Args...>> : std::true_type {
    };
    template<typename ...Args>
    struct is_stl_container<std::initializer_list<Args...>> : std::true_type {
    };
    //tbd...
}

// type trait to utilize the implementation type traits as well as decay the type
template <typename T>
struct is_stl_container {
    static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};

//////////////////////////////////////////////////////////////////////////////

//#include "sequence.hpp"

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

//////////////////////////////////////////////////////////////////////////////

template<typename CONT, typename Enable = void>
struct PrintAnyContHelper {
};

template <typename CONT>
struct PrintAnyContHelper<CONT, std::enable_if_t<std::is_array_v<CONT>>> {
    static std::ostream& PrintAnyCont(std::ostream &where, CONT const& cont) {
        for(size_t i=0; i<sizeof(cont)/sizeof(cont[0]); i++)
            where << cont[i] << ", ";
        return where;
    }
};

template <typename CONT>
struct PrintAnyContHelper<CONT, std::enable_if_t<std::is_same_v<std::decay_t<CONT>, std::string>
                                                 || is_stl_container_impl::is_stl_container<CONT>::value>> {
    static std::ostream& PrintAnyCont(std::ostream& where, CONT const& cont) {
        std::for_each(std::begin(cont), std::end(cont), [&where](typename CONT::value_type const& item){
            where << item << ", ";
        });
        return where;
    }
};

template <typename CONT>
void PrintAnyCont(CONT const& cont) {
    std::ostringstream where;
    PrintAnyContHelper<CONT>::PrintAnyCont(where, cont);
    std::cout << where.str() << std::endl;
}

//////////////////////////////////////////////////////////////////////////////

template <class T>
bool any(T const& v) {
    if(std::is_default_constructible_v<T>) {
        return T() == v;
    }
    return false;
}

template <class T, typename ...Args>
bool any(T const& v, Args ... args) {
    if(any(v))
        return true;
    return any(args...);
}

//////////////////////////////////////////////////////////////////////////////

int main() {
    std::cout << std::boolalpha;

    std::cout << "*** 1 ***" << std::endl;
    //Задание 1. создать функцию для вывода на печать элементов последовательностей, заданных ниже
    {
        std::vector<double> vd = {1.1, 2.2, 3.3};
        PrintAnyCont(vd);

        std::string s("abc");
        PrintAnyCont(s);

        int ar[] = {1, 2, 3};
        PrintAnyCont(ar);

        std::initializer_list<int> il{3, 4, 5};
        PrintAnyCont(il);
    }

    std::cout << "*** 2 ***" << std::endl;
    //Задание 2. напишите функцию, которая будет формировать и возвращать вектор, каждый
    // элемент которого является суммой элементов двух последовательностей
    // РАЗНОЙ длины и с элементами РАЗНОГО типа.
    //Подсказка 1: так как последовательности могут быть разной длины, логично сделать
    // размер результирующего вектора максимальным из двух
    //Подсказка 2: подумайте о возможности использования алгоритма transform(), в
    // котором трансформирующее действие требуется задать лямбда-функцией
    {
        std::vector<int> iv1 = {1, 2, 3, 4};
        std::vector<int> iv2 = {1, 2, 3, 4, 5};
        std::list<int> il = {1, 2, 3, 4, 5};
        std::list<double> dl = {1.1, 2.2, 3.3, 4.4, 5.5};

        auto res1 = SumCont(iv1, iv2);
        PrintAnyCont(res1);

        auto res2 = SumCont(il, dl);
        PrintAnyCont(res2);
    }

    std::cout << "*** 3 ***" << std::endl;
    //Задание 3.
    //Напишите функцию, которая
    //• принимает любое количество параметров любого типа
    //• и проверяет - совпадает ли хотя бы один параметр со значением (данного типа) по
    //умолчанию. Подсказка: дефолтовый конструктор может быть запрещен.
    {
        struct A {
            int _a;
            A(int a=0) : _a(a) {}
            bool operator == (A const& a) {
                return _a == a._a;
            }
        };

        bool b1 = any( 2.2, 3, A(1), 0 ); //true
        std::cout << b1 << std::endl;
        bool b2 = any( 2.2, 3, A() , A(1)); //true
        std::cout << b2 << std::endl;
        bool b3 = any( 2.2, 3, A(1), 5); //false
        std::cout << b3 << std::endl;
    }
}
