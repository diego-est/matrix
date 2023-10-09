/*  Static Size Container Array
*
* Wrapper class to a c-array of Tp_[n_]
*
* @tparam Tp_   -> element type
* @tparam n_    -> size of array
*/
template<typename Tp_, Size n_>
class Array
{
    /*  EnableCMem
    * check for the ability to 
    *   use `memcpy` and `memmove`
    *   from a type Vp_ to a type Tp_
    */
    template<typename Vp_>
    using EnableCMem = meta::BoolConstant<
        sizeof(Vp_) == sizeof(Tp_) &&
        std::is_pod<Tp_>::value &&
        std::is_pod<Vp_>::value
    >;

    /*  Store size of data array in type
    *       for zero overhead
    */
    using DataSize = meta::SizeConstant<sizeof(Tp_) * n_>;

    /*  Raw Data
    */
    Tp_ _data[n_];

public:

    //============================================
    //          ** Initialisation **
    //============================================

    /*  Maintain default initialisation
    *       from compilier
    *
    * default:
    *   ~ Default Ctor
    *   ~ Copy Ctor
    *   ~ Move Ctor
    *   ~ Destructor
    *   ~ Copy Assignment Operator
    *   ~ Move Assignment Operator
    */

    Array(void)             = default;
    Array(const Array &)    = default;
    Array(Array &&)         = default;
    ~Array(void)            = default;

    Array & operator = (const Array &)  = default;
    Array & operator = (Array &&)       = default;

    /*  Values Constructor
    *
    * move a variadic list of values into the raw data
    *
    * enabled when exactly n_ many values are provided
    *
    * @tparam Tps_    -> parameter pack of arguments to move assign
    *
    * ASSERT: Every Tps_ is move assignable
    */
    template<typename ... Tps_,
        typename = meta::EnableIf_t<
            meta::VariadicSize_v<Tps_ ... > == n_
        >>
    Array(Tps_ && ... values)
        : _data{ std::move(values) ... }
    {}


    //============================================
    //             ** Data Access **
    //============================================

    /*  Subcript Operator
    *
    * access an individual element in the array
    *
    * @param index  -> element in the array
    */
    Tp_ & operator[] (const Size & index) { return Array::_data[index]; }
    const Tp_ & operator[] (const Size & index) const { return Array::_data[index]; }

    /*  Iterators
    *
    * access [begin, end) pointers of the array
    */
    Tp_ * begin(void) { return Array::_data; }
    Tp_ * end(void) { return Array::_data + n_; }

    const Tp_ * begin(void) const { return Array::_data; }
    const Tp_ * end(void) const { return Array::_data + n_; }

    /*  First and Last
    *
    * access the first and last element in array
    */
    Tp_ & front(void) { return *Array::_data; }
    Tp_ & back(void) { return *(Array::_data + n_ - 1); }

    const Tp_ & front(void) const { return *Array::_data; }
    const Tp_ & back(void) const { return *(Array::_data + n_ - 1); }

    /*  Raw Data
    *
    * access a pointer to the raw data
    */
    Tp_ * data(void) { return Array::_data; }
    const Tp_ * data(void) const { return Array::_data; }


    //============================================
    //             ** Utilities **
    //============================================

    /*  Array size
    *
    * static size of array -> n_
    */
    static constexpr Size size(void) { return n_; }

    /*  Array Fill
    *
    * copy value to every element in array
    *
    * @param value  -> value to copy assign
    *
    * ASSERT: copy assignable from const Vp_ to Tp_
    */
    template<typename Vp_>
    void fill(const Vp_ & value)
    {
        static_assert(meta::CopyAssignable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::fill(const Vp_ &) ~ Invalid Vp_, requires copy assignability from const Vp_ to Tp_ \n");

        std::fill(Array::begin(), Array::end(), value);
    }


    /*  Array Copy
    * 
    * copy data from rhs array to lhs
    *
    * enable if c-styled copy available between types
    *
    * @param array  -> the rhs Array of any type
    */
    template<typename Vp_>
    meta::EnableIf_t<EnableCMem<const Vp_>::value>
    copy(const Array<Vp_, n_> & array)
    {
        memcpy(Array::_data, array.begin(), DataSize::value);
    }

    /*  Array Copy
    *
    * iterate copy from rhs array to lhs
    *
    * enable if c-styled copy is NOT available between types
    *
    * @param array  -> the rhs Array of any type
    *
    * ASSERT: copy assignable from const Vp_ to Tp_
    */
    template<typename Vp_>
    meta::EnableIf_t<!EnableCMem<const Vp_>::value>
    copy(const Array<Vp_, n_> & array)
    {
        static_assert(meta::CopyAssignable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::copy(const Array<Vp_, n_> &) ~ Invalid Vp_, requires copy assignability from const Vp_ to Tp_ \n");

        std::copy(array.begin(), array.end(), Array::begin());
    }

    /*  Array Move
    *
    * move data from rhs array to lhs
    *
    * enable if c-styled move available between types
    *
    * @param array  -> rhs Array of any type
    */
    template<typename Vp_>
    meta::EnableIf_t<EnableCMem<Vp_>::value>
    move(Array<Vp_, n_> && array)
    {
        memmove(Array::_data, array.begin(), DataSize::value);
    }

    /*  Array Move
    *
    * iterate move from rhs array to lhs
    *
    * enable if c-styled move NOT available between types
    *
    * @param array  -> rhs Array of any type
    *
    * ASSERT: move assignable from Vp_ to Tp_
    */
    template<typename Vp_>
    meta::EnableIf_t<!EnableCMem<Vp_>::value>
    move(Array<Vp_, n_> && array)
    {
        static_assert(meta::MoveAssignable_v<Tp_, Vp_>,
        "Array<Tp_, n_>::move(Array<Vp_, n_> &&) ~ Invalid Vp_, requires move assignability from Vp_ to Tp_ \n");

        std::move(array.begin(), array.end(), Array::begin());
    }

    /*  Array Swap
    *
    * swap data betwen rhs array and lhs array
    *
    * @param array  -> rhs Array to swap
    */
    template<typename Vp_>
    void swap(Array<Vp_, n_> & array)
    {
        auto array_temp = std::move(array);
        array = std::move(*this);
        *this = std::move(array_temp);
    }

    //============================================
    //             ** Operators **
    //============================================

    /*  Function Operator
    *
    * call `function` for every element
    *
    * @param function   -> function to call
    *
    * ASSERT: function is callable as 
    *           function(Tp_ &, const Size &)
    */
    void operator() (auto function)
    {
        static_assert(meta::Callable_v<decltype(function), Tp_ &, const Size &>,
        "Array<Tp_, n_>::operator()(auto function) ~ Invalid function, requires signature function(Tp_ &, const Size &) \n");

        for (Size i = 0; i != n_; ++i)
            function(Array::_data[i], i);
    }

    /*  Array Cast
    *
    * creates an implicit conversion from 
    * this Array<Tp_> to an Array<Vp_>
    *
    * copies data from this to lhs
    */
    template<typename Vp_>
    operator Array<Vp_, n_> (void)
    {
        Array<Vp_, n_> conversion;
        conversion.copy(*this);
        return conversion;
    }

    /*  Equality
    *
    * validate if every element in rhs array is
    * equal to every element in lhs array
    *
    * @param array  -> rhs Array of any type
    *
    * ASSERT: equatability between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator == (const Array<Vp_, n_> & array)
    {
        static_assert(meta::Equatable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator==(const Array<Vp_, n_> &) ~ Invalid Vp_, requires equatability between Tp_ and const Vp_ \n");

        return std::equal(array.begin(), array.end(), Array::begin());
    }

    /*  Scalar Equality
    *
    * validate if value is equal to
    * every element in lhs array
    *
    * @param value  -> value of any type
    *
    * ASSERT: equatability between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator == (const Vp_ & value)
    {
        static_assert(meta::Equatable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator==(const Vp_ &) ~ Invalid Vp_, requires equatability between Tp_ and const Vp_ \n");

        return std::equal(Array::begin(), Array::end(), value);
    }

    /*  Inequality
    *
    * return the inverse of equality
    */
    template<typename Vp_>
    auto operator != (const Array<Vp_, n_> & array)
    {
        return !(*this == array);
    }

    /*  Scalar Inequality
    *
    * return the inverse of scalar inequality
    */
    template<typename Vp_>
    auto operator != (const Vp_ & value)
    {
        return !(*this == value);
    }


    /*  Addition
    *
    * assign the addition of every element
    * in rhs array and every element in lhs
    * array to a new instance and return
    *
    * @param array  -> rhs Array of any type
    *
    * ASSERT: addability between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator + (const Array<Vp_, n_> & array)
    {
        static_assert(meta::Addable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator+(const Array<Vp_, n_> &) ~ Invalid Vp_, requires addability between Tp_ and const Vp_ \n");

        Array<meta::AddableTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] + array[index];
        });
        return result;
    }

    /*  Scalar Addition
    *
    * assign the addition of rhs value and 
    * every element in lhs array to 
    * a new instance and return
    *
    * @param value  -> rhs value of any type
    *
    * ASSERT: addability between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator + (const Vp_ & value)
    {
        static_assert(meta::Addable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator+(const Vp_ &) ~ Invalid Vp_, requires addability between Tp_ and const Vp_ \n");

        Array<meta::AddableTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] + value;
        });
        return result;
    }

    /*  Negation
    *
    * assign the negation of every element
    * in rhs array to new instance and return
    *
    * ASSERT: negatability of a const Tp_
    */
    auto operator - (void)
    {
        static_assert(meta::Negatable_v<Tp_>,
        "Array<Tp_, n_>::operator-(void) ~ Invalid Tp_, requires negatability on a Tp_ \n");

        Array<meta::NegatableTest<Tp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = -(this->_data[index]);
        });
        return result;
    }

    /*  Subtraction
    *
    * assign the subtraction of every element
    * in rhs array and every element in the lhs
    * array to a new instance and return
    *
    * @param array  -> rhs Array of any type
    *
    * ASSERT: subtractability between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator - (const Array<Vp_, n_> & array)
    {
        static_assert(meta::Subtractable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator-(const Array<Vp_, n_> &) ~ Invalid Vp_, requires subtractability between Tp_ and const Vp_ \n");

        Array<meta::SubtractableTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] - array[index];
        });
        return result;
    }

    /*  Scalar Subtraction
    *
    * assign the subtraction of rhs value and 
    * every element in lhs array to 
    * a new instance and return
    *
    * @param value  -> rhs value of any type
    *
    * ASSERT: subtractability between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator - (const Vp_ & value)
    {
        static_assert(meta::Subtractable_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator-(const Vp_ &) ~ Invalid Vp_, requires subtractability between Tp_ and const Vp_ \n");

        Array<meta::SubtractableTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] - value;
        });
        return result;
    }

    /*  Product
    *
    * assign the product of every element
    * in rhs array and every element in the lhs
    * array to a new instance and return
    *
    * @param array  -> rhs Array of any type
    *
    * ASSERT: multiplicativity between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator * (const Array<Vp_, n_> & array)
    {
        static_assert(meta::Multiplicative_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator*(const Array<Vp_, n_> &) ~ Invalid Vp_, requires multiplicativity between Tp_ and const Vp_ \n");

        Array<meta::MultiplicativeTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] * array[index];
        });
        return result;
    }

    /*  Scalar Product
    *
    * assign the product of rhs value and 
    * every element in lhs array to 
    * a new instance and return
    *
    * @param value  -> rhs value of any type
    *
    * ASSERT: multiplicativity between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator * (const Vp_ & value)
    {
        static_assert(meta::Multiplicative_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator*(const Vp_ &) ~ Invalid Vp_, requires multiplicativity between Tp_ and const Vp_ \n");

        Array<meta::MultiplicativeTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] * value;
        });
        return result;
    }

    /*  Quotient
    *
    * assign the quotient of every element
    * in rhs array and every element in the lhs
    * array to a new instance and return
    *
    * @param array  -> rhs Array of any type
    *
    * ASSERT: divisibility between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator / (const Array<Vp_, n_> & array)
    {
        static_assert(meta::Divisible_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator/(const Array<Vp_, n_> &) ~ Invalid Vp_, requires divisbility between Tp_ and const Vp_ \n");

        Array<meta::DivisibleTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] / array[index];
        });
        return result;
    }

    /*  Scalar Quotient
    *
    * assign the quotient of rhs value and 
    * every element in lhs array to 
    * a new instance and return
    *
    * @param value  -> rhs value of any type
    *
    * ASSERT: divisibility between Tp_ and const Vp_
    */
    template<typename Vp_>
    auto operator / (const Vp_ & value)
    {
        static_assert(meta::Divisible_v<Tp_, const Vp_>,
        "Array<Tp_, n_>::operator/(const Vp_ &) ~ Invalid Vp_, requires divisbility between Tp_ and const Vp_ \n");

        Array<meta::DivisibleTest<Tp_, const Vp_>, n_> result;
        result([&, this](auto & elem, const auto & index)
        {
            elem = this->_data[index] / value;
        });
        return result;
    }

};
