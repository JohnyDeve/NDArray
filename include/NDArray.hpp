#ifndef NDARRAY_HPP
#define NDARRAY_HPP

#include <type_traits>

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

template< typename T >
concept DimensionalContainerConcept =
	(std::is_integral_v< std::remove_cvref_t< decltype(*std::declval< T >().begin()) > >) && requires(T& t) {
		{ t.begin() };
		{ t.end() };
		{ t.size() } -> std::convertible_to< std::size_t >;
	};

template< typename ArrayType >
concept MultiArrayConcept = requires(const ArrayType& array) {
	{ array.data() };
	{ array.getDimensions() };
	{ array.total_count() };
	{ array.begin() };
	{ array.end() };
};

template< typename T, size_t N, bool IsConstClass >
class GeneralNDArrayView
{
  public:
	using value_type = T;
	using reference = std::conditional_t< IsConstClass, const T&, T& >;
	using pointer = std::conditional_t< IsConstClass, const T*, T* >;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::random_access_iterator_tag;

  private:
	pointer data_ptr_;
	std::array< size_type, N > view_dimensions_;
	size_type data_view_size_;

  public:
	template< typename Container >
		requires(MultiArrayConcept< Container >)
	GeneralNDArrayView(const Container& ndarray) noexcept :
		data_ptr_(const_cast< pointer >(ndarray.data())), view_dimensions_(ndarray.getDimensions()),
		data_view_size_(ndarray.total_count())
	{
	}

	GeneralNDArrayView(pointer data_ptr, const std::array< size_type, N >& view_dimensions) noexcept :
		data_ptr_(data_ptr), view_dimensions_(view_dimensions), data_view_size_(getSize(view_dimensions))
	{
	}

	template< typename Iterator >
		requires((std::contiguous_iterator< Iterator >) && (std::is_convertible_v< std::iter_value_t< Iterator >*, pointer >))
	GeneralNDArrayView(Iterator first, Iterator last, const std::array< size_t, N >& view_dimensions) noexcept :
		data_ptr_(std::to_address(first)), view_dimensions_(view_dimensions), data_view_size_(getSize(view_dimensions))
	{
	}

	GeneralNDArrayView() noexcept : data_ptr_(nullptr), data_view_size_(0) { view_dimensions_.fill(0); }

	GeneralNDArrayView(const GeneralNDArrayView& other) noexcept :
		data_ptr_(other.data_ptr_), view_dimensions_(other.view_dimensions_), data_view_size_(other.data_view_size_)
	{
	}

	GeneralNDArrayView(GeneralNDArrayView&& other) noexcept :
		data_ptr_(other.data_ptr_), view_dimensions_(std::move(other.view_dimensions_)), data_view_size_(other.data_view_size_)
	{
		other.data_ptr_ = nullptr;
		other.data_view_size_ = 0;
	}

	GeneralNDArrayView& operator=(const GeneralNDArrayView& other) noexcept
	{
		if (this != &other)
		{
			data_ptr_ = other.data_ptr_;
			view_dimensions_ = other.view_dimensions_;
			data_view_size_ = other.data_view_size_;
		}
		return *this;
	}

	GeneralNDArrayView& operator=(GeneralNDArrayView&& other) noexcept
	{
		if (this != &other)
		{
			data_ptr_ = other.data_ptr_;
			view_dimensions_ = std::move(other.view_dimensions_);
			data_view_size_ = other.data_view_size_;

			other.data_ptr_ = nullptr;
			other.data_view_size_ = 0;
		}
		return *this;
	}

	~GeneralNDArrayView()
	{
		data_ptr_ = nullptr;
		data_view_size_ = 0;
	}

	reference operator*() const &
		requires(N == 1)
	{
		return *data_ptr_;
	}

	GeneralNDArrayView< T, N - 1, IsConstClass > operator*() const &
		requires(N > 1)
	{
		return GeneralNDArrayView< T, N - 1, IsConstClass >{ data_ptr_, goDownDimensions(view_dimensions_) };
	}

	reference operator[](size_type index) const &
		requires(N == 1)
	{
		return data_ptr_[index];
	}

	GeneralNDArrayView< T, N - 1, IsConstClass > operator[](size_type index) const &
		requires(N > 1)
	{
		return GeneralNDArrayView< T, N - 1, IsConstClass >{ data_ptr_ + index * (total_count() / count()), goDownDimensions(view_dimensions_) };
	}

	GeneralNDArrayView& operator++() noexcept
	{
		data_ptr_ += (total_count() / count());
		return *this;
	}
	GeneralNDArrayView operator++(int) noexcept
	{
		GeneralNDArrayView tmp = *this;
		++(*this);
		return tmp;
	}

	GeneralNDArrayView& operator--() noexcept
	{
		data_ptr_ -= (total_count() / count());
		return *this;
	}

	GeneralNDArrayView operator--(int) noexcept
	{
		GeneralNDArrayView tmp = *this;
		--(*this);
		return tmp;
	}

	GeneralNDArrayView& operator+=(difference_type n) noexcept
	{
		data_ptr_ += (n * (total_count() / count()));
		return *this;
	}

	GeneralNDArrayView& operator-=(difference_type n) noexcept
	{
		data_ptr_ -= (n * (total_count() / count()));
		return *this;
	}

	GeneralNDArrayView operator+(difference_type n) const noexcept
	{
		GeneralNDArrayView tmp = *this;
		tmp += n;
		return tmp;
	}

	GeneralNDArrayView operator-(difference_type n) const noexcept
	{
		GeneralNDArrayView tmp = *this;
		tmp -= n;
		return tmp;
	}

	difference_type operator-(const GeneralNDArrayView& other) const noexcept { return data_ptr_ - other.data_ptr_; }

	bool operator==(const GeneralNDArrayView& other) const noexcept { return data_ptr_ == other.data_ptr_; }

	bool operator!=(const GeneralNDArrayView& other) const noexcept { return !(*this == other); }

	bool operator<(const GeneralNDArrayView& other) const noexcept { return data_ptr_ < other.data_ptr_; }

	bool operator>(const GeneralNDArrayView& other) const noexcept { return data_ptr_ > other.data_ptr_; }

	bool operator<=(const GeneralNDArrayView& other) const noexcept { return data_ptr_ <= other.data_ptr_; }

	bool operator>=(const GeneralNDArrayView& other) const noexcept { return data_ptr_ >= other.data_ptr_; }

	pointer operator->() const noexcept
		requires(N == 1)
	{
		return data_ptr_;
	}

	size_type count() const noexcept { return view_dimensions_[0]; }

	size_type total_count() const noexcept { return data_view_size_; }

	const std::array< size_type, N >& getDimensions() const noexcept { return view_dimensions_; }

	template< size_type K = N, typename InitIterator >
	auto at(InitIterator it) const noexcept
	{
		if constexpr (K == 1)
		{
			return (*this)[*it];
		}
		else
		{
			return (*this)[*it].template at< K - 1 >(it + 1);
		}
	}

  private:
	std::array< size_type, N - 1 > goDownDimensions(const std::array< size_type, N >& dims) const
	{
		std::array< size_type, N - 1 > result;
		std::copy(dims.begin() + 1, dims.end(), result.begin());
		return result;
	}

	size_type getSize(const std::array< size_type, N >& dims, size_type off = 0) const noexcept
	{
		size_type size = 1;
		for (size_type i = off; i < dims.size(); ++i)
		{
			size *= dims[i];
		}
		return size;
	}
};

template< typename T, size_t N >
using NDArrayView = GeneralNDArrayView< T, N, false >;

template< typename T, size_t N >
using NDArrayConstView = GeneralNDArrayView< T, N, true >;

template< typename T, size_t N >
class NDArray
{
  public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using iterator = NDArrayView< T, N >;
	using const_iterator = NDArrayConstView< T, N >;
	using difference_type = std::ptrdiff_t;
	using size_type = size_t;
	using pointer = T*;
	using const_pointer = const T*;
	using view = iterator;

  private:
	pointer data_;
	std::array< size_type, N > dimensions_;
	size_type data_size_;

  public:
	NDArray() noexcept : data_(nullptr), data_size_(0) { dimensions_.fill(0); }

	NDArray(std::initializer_list< T > initializer)
		requires(N == 1)
		: data_size_(initializer.size())
	{
		dimensions_[0] = data_size_;
		start_allocate();
		size_type cnt_constructed = 0;
		try
		{
			for (const T& elem : initializer)
			{
				new (&data_[cnt_constructed++]) value_type(elem);
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}
	}

	NDArray(std::initializer_list< NDArray< T, N - 1 > > initializer)
		requires(N > 1)
	{
		if (initializer.size() == 0)
		{
			throw std::invalid_argument("[ERROR]: in NDArray::NDArray: initializer is empty");
		}

		std::array< size_type, N - 1 > temp_dims = (initializer.begin())->getDimensions();
		for (const NDArray< T, N - 1 >& lowerNdarray : initializer)
		{
			if (temp_dims != lowerNdarray.getDimensions())
			{
				throw std::invalid_argument("[ERROR]: in NDArray::NDArray: The dimensions do not match each other");
			}
		}

		dimensions_[0] = initializer.size();
		std::copy(temp_dims.begin(), temp_dims.end(), dimensions_.begin() + 1);

		data_size_ = getSize(dimensions_);

		start_allocate();
		size_type cnt_constructed = 0;
		try
		{
			for (; cnt_constructed < data_size_; cnt_constructed++)
			{
				new (&data_[cnt_constructed]) value_type();
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}

		pointer copy_ptr = data_;
		for (const NDArray< T, N - 1 >& lowerNdarray : initializer)
		{
			std::copy(lowerNdarray.data(), lowerNdarray.data() + lowerNdarray.total_count(), copy_ptr);
			copy_ptr += lowerNdarray.total_count();
		}
	}

	template< typename... Dims >
		requires((sizeof...(Dims) == N) && (std::conjunction_v< std::is_convertible< Dims, size_type >... >))
	explicit NDArray(Dims... dims) : data_(nullptr), data_size_(getSize(dimensions_))
	{
		dimensions_ = { static_cast< size_type >(dims)... };
		start_allocate();
		size_type cnt_constructed = 0;
		try
		{
			for (; cnt_constructed < data_size_; cnt_constructed++)
			{
				new (&data_[cnt_constructed]) value_type();
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}
	}

	template< typename... Args >
		requires((sizeof...(Args) == (N + 1)) && (std::is_convertible_v< std::tuple_element_t< N, std::tuple< Args... > >, value_type >) &&
				 (std::is_constructible_v< value_type, std::tuple_element_t< N, std::tuple< Args... > > >))
	explicit NDArray(Args... args) : data_(nullptr)
	{
		std::tuple< Args... > tuple_args(std::forward< Args >(args)...);
		std::array< size_type, N > dims;
		[&]< std::size_t... Is >(std::index_sequence< Is... >)
		{
			((dims[Is] = static_cast< size_type >(std::get< Is >(tuple_args))), ...);
		}(std::make_index_sequence< N >{});

		dimensions_ = dims;
		data_size_ = getSize(dimensions_);
		start_allocate();
		value_type value = static_cast< value_type >(std::get< N >(tuple_args));
		size_type cnt_constructed = 0;
		try
		{
			for (; cnt_constructed < data_size_; cnt_constructed++)
			{
				new (&data_[cnt_constructed]) value_type(value);
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}
	}

	template< typename VT, size_t VN, typename = std::enable_if_t< (std::is_same_v< VT, T >) && (VN == N) > >
	NDArray(const NDArrayView< VT, VN >& ndview) :
		dimensions_(ndview.getViewDimensions()), data_size_(getSize(dimensions_)), data_(nullptr)
	{
		start_allocate();
		size_type cnt_constructed = 0;
		try
		{
			for (; cnt_constructed < data_size_; cnt_constructed++)
			{
				new (&data_[cnt_constructed]) value_type(ndview[cnt_constructed]);
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}
	}

	template< typename Iterator >
		requires(std::input_iterator< Iterator > && std::sentinel_for< Iterator, Iterator > &&
				 std::convertible_to< std::iter_reference_t< Iterator >, value_type >)
	NDArray(Iterator first, Iterator last, std::array< size_type, N > dims = {}) :
		data_(nullptr), dimensions_{}, data_size_(0)
	{
		size_type cnt = static_cast< size_type >(std::distance(first, last));

		bool all_zero = std::all_of(dims.begin(), dims.end(), [](size_type dim) { return dim == 0; });

		if (all_zero)
		{
			dims.fill(1);
			dims[N - 1] = cnt;
		}

		if (getSize(dims) != cnt)
		{
			throw std::invalid_argument("[ERROR] in NDArray::NDArray: The count of elements error");
		}

		dimensions_ = dims;
		data_size_ = cnt;

		start_allocate();
		size_type cnt_constructed = 0;
		try
		{
			for (Iterator it = first; it != last; it++)
			{
				new (&data_[cnt_constructed++]) value_type(*it);
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}
	}

	NDArray(const std::array< size_type, N >& dimensions) :
		dimensions_(dimensions), data_size_(getSize(dimensions)), data_(nullptr)
	{
		start_allocate();
		size_type cnt_constructed = 0;
		try
		{
			for (; cnt_constructed < data_size_; cnt_constructed++)
			{
				new (&data_[cnt_constructed]) value_type();
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}
	}

	~NDArray() noexcept { start_cleaner(data_size_); }

	NDArray(const NDArray& other) : dimensions_(other.dimensions_), data_size_(other.data_size_), data_(nullptr)
	{
		start_allocate();
		size_type cnt_constructed = 0;
		try
		{
			for (; cnt_constructed < data_size_; cnt_constructed++)
			{
				new (&data_[cnt_constructed]) value_type(other.data_[cnt_constructed]);
			}
		} catch (...)
		{
			start_cleaner(cnt_constructed);
			throw std::runtime_error("BAD ALLOC");
		}
	}

	NDArray(NDArray&& other) noexcept :
		data_(other.data_), dimensions_(std::move(other.dimensions_)), data_size_(other.data_size_)
	{
		other.data_ = nullptr;
		other.data_size_ = 0;
	}

	NDArray& operator=(const NDArray& other)
	{
		if (this != &other)
		{
			NDArray temp(other);
			temp.swap(*this);
		}
		return *this;
	}

	NDArray& operator=(NDArray&& other) noexcept
	{
		if (this != &other)
		{
			NDArray temp(std::move(other));
			temp.swap(*this);
		}
		return *this;
	}

	reference operator[](size_type index) const &
		requires(N == 1)
	{
		return data_[index];
	}

	auto operator[](size_type index) noexcept
		requires(N > 1)
	{
		return *(iterator(*this) + index);
	}

	auto operator[](size_type index) const noexcept
		requires(N > 1)
	{
		return *(const_iterator(*this) + index);
	}

	size_type count() const noexcept { return dimensions_[0]; }

	size_type total_count() const noexcept { return data_size_; }

	size_type dim() const noexcept { return N; }
	void swap(NDArray& other)
	{
		if (dimensions_ != other.dimensions_)
		{
			throw std::invalid_argument("[ERROR] in NDArray::swap: The dimensions size do not match");
		}
		std::swap(data_, other.data_);
		std::swap(data_size_, other.data_size_);
		std::swap(dimensions_, other.dimensions_);
	}

	bool is_equal(const NDArray& other) const
		requires(std::equality_comparable< T >)
	{
		if (dimensions_ != other.dimensions_)
		{
			return false;
		}

		for (size_type i = 0; i < data_size_; i++)
		{
			if (!(data_[i] == other.data_[i]))
			{
				return false;
			}
		}
		return true;
	}

	auto at(std::initializer_list< size_type > indexes) const
	{
		if (indexes.size() > N || indexes.size() == 0)
		{
			throw std::invalid_argument("[ERROR] in NDArray::at: indexes size error");
		}
		size_type i = 0;
		for (size_type inx : indexes)
		{
			if (!(inx < dimensions_[i]))
			{
				throw std::invalid_argument("[ERROR] in NDArray::at: index out of bounds");
			}
			i++;
		}

		return iterator(*this).template at< N >(indexes.begin());
	}

	template< typename SizeList >
		requires(DimensionalContainerConcept< SizeList >)
	view reshape(const SizeList& dimsContainer)
	{
		if (dimsContainer.size() != N)
		{
			throw std::invalid_argument("[ERROR] in NDArray::reshape: dimension size error");
		}

		std::array< size_type, N > newDims;
		std::copy(dimsContainer.begin(), dimsContainer.end(), newDims.begin());

		size_type newTotal = getSize(newDims);
		if (newTotal != total_count())
		{
			throw std::invalid_argument("[ERROR] in NDArray::reshape: total element count error");
		}

		return iterator(data_, newDims);
	}

	iterator begin() noexcept { return iterator(*this); }

	iterator end() noexcept { return iterator(data_ + data_size_, dimensions_); }

	const_iterator begin() const noexcept { return const_iterator(*this); }

	const_iterator end() const noexcept { return const_iterator(data_ + data_size_, dimensions_); }

	const_iterator cbegin() const noexcept { return const_iterator(*this); }

	const_iterator cend() const noexcept { return const_iterator(data_ + data_size_, dimensions_); }

	pointer data() noexcept { return data_; }
	const_pointer data() const noexcept { return data_; }

	const std::array< size_type, N >& getDimensions() const noexcept { return dimensions_; }

  private:
	size_type getSize(const std::array< size_type, N >& view_dimensions, size_t off = 0) const noexcept
	{
		size_type res_size = 1;
		for (size_t i = off; i < view_dimensions.size(); i++)
		{
			res_size *= view_dimensions[i];
			if (!res_size)
			{
				return 0;
			}
		}
		return res_size;
	}

	void start_cleaner(const size_type cnt_constructed) noexcept
	{
		if (data_)
		{
			for (size_type i = cnt_constructed; i-- > 0;)
			{
				data_[i].~value_type();
			}
			::operator delete[](data_);
			data_ = nullptr;
		}
	}

	void start_allocate()
	{
		try
		{
			data_ = static_cast< pointer >(::operator new[](data_size_ * sizeof(value_type)));
		} catch (const std::bad_alloc&)
		{
			throw std::runtime_error("BAD ALLOC");
		}
	}
};

#endif
