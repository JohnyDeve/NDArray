#include "NDArray.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <numeric>
#include <ostream>

TEST(NDArrayTest, Construction)
{
	int arr1p[] = { 1, 2, 3 };
	NDArray< int, 1 > arr1d{ 1, 2, 3 };
	EXPECT_EQ(arr1d.count(), 3);
	EXPECT_EQ(arr1d.data()[0], 1);
	EXPECT_EQ(arr1d.data()[1], 2);
	EXPECT_EQ(arr1d.data()[2], 3);

	int arr2p[] = { 1, 2, 3, 4 };
	NDArray< int, 2 > arr2d{ { 1, 2 }, { 3, 4 } };
	EXPECT_EQ(arr2d.count(), 2);
	for (size_t i = 0; i < sizeof(arr2p) / sizeof(*arr2p); ++i)
	{
		EXPECT_EQ(arr2d.data()[i], arr2p[i]);
	}

	int arr3p[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	NDArray< int, 3 > arr3d{ { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } };
	EXPECT_EQ(arr3d.count(), 2);
	for (size_t i = 0; i < sizeof(arr3p) / sizeof(*arr3p); ++i)
	{
		EXPECT_EQ(arr3d.data()[i], arr3p[i]);
	}
}

TEST(NDArrayTest, CopyAndMove)
{
	NDArray< int, 2 > original{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > copied(original);
	EXPECT_EQ(copied.count(), 2);
	EXPECT_EQ(copied.data()[0], 1);
	EXPECT_EQ(copied.data()[1], 2);
	EXPECT_EQ(copied.data()[2], 3);
	EXPECT_EQ(copied.data()[3], 4);

	NDArray< int, 2 > moved(std::move(original));
	EXPECT_EQ(moved.count(), 2);
	EXPECT_EQ(moved.data()[0], 1);
	EXPECT_EQ(moved.data()[1], 2);
	EXPECT_EQ(moved.data()[2], 3);
	EXPECT_EQ(moved.data()[3], 4);
	EXPECT_EQ(original.data(), nullptr);
}

TEST(NDArrayTest, CopyAndMoveTraits)
{
	EXPECT_TRUE((!std::is_trivially_copyable_v< NDArray< int, 2 > >))
		<< "Pointer-owning NDArray must not be trivially "
		   "copyable";

	EXPECT_TRUE((std::is_copy_constructible_v< NDArray< int, 2 > >)) << "Should be copy-constructible";

	EXPECT_TRUE((std::is_move_constructible_v< NDArray< int, 2 > >)) << "Should be move-constructible";

	EXPECT_TRUE((std::is_nothrow_move_constructible_v< NDArray< int, 2 > >))
		<< "Move-construct should be noexcept for "
		   "basic types";
}

TEST(NDArrayTest, DifferentTypes)
{
	NDArray< double, 2 > doubleArr{ { 1.5, 2.5 }, { 3.5, 4.5 } };
	EXPECT_EQ(doubleArr.count(), 2);
	EXPECT_DOUBLE_EQ(doubleArr.data()[0], 1.5);
	EXPECT_DOUBLE_EQ(doubleArr.data()[1], 2.5);
	EXPECT_DOUBLE_EQ(doubleArr.data()[2], 3.5);
	EXPECT_DOUBLE_EQ(doubleArr.data()[3], 4.5);
}

TEST(NDArrayTest, total_countMethod)
{
	NDArray< int, 1 > arr1d{ 1, 2, 3 };
	EXPECT_EQ(arr1d.total_count(), 3);

	NDArray< int, 2 > arr2d{ { 1, 2 }, { 3, 4 } };
	EXPECT_EQ(arr2d.total_count(), 4);

	NDArray< int, 3 > arr3d{ { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } };
	EXPECT_EQ(arr3d.total_count(), 8);

	NDArray< int, 4 > arr4d{ { { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } }, { { { 9, 10 }, { 11, 12 } }, { { 13, 14 }, { 15, 16 } } } };
	EXPECT_EQ(arr4d.total_count(), 16);
}

TEST(NDArrayTest, EmptyArray)
{
	NDArray< int, 1 > empty1d{};
	EXPECT_EQ(empty1d.count(), 0);
	EXPECT_EQ(empty1d.total_count(), 0);
	EXPECT_EQ(empty1d.data(), nullptr);

	NDArray< int, 2 > empty2d{ {} };
	EXPECT_EQ(empty2d.count(), 1);
	EXPECT_EQ(empty2d.total_count(), 0);
	EXPECT_NE(empty2d.data(), nullptr);
}

TEST(NDArrayMoveAssignment, BasicMoveAssignmentStealsStorage)
{
	NDArray< int, 2 > src{ { 1, 2 }, { 3, 4 } };
	int* oldPtr = src.data();

	NDArray< int, 2 > dst{ { 1, 1 }, { 1, 1 } };
	dst = std::move(src);

	EXPECT_EQ(dst.data()[0], 1);
	EXPECT_EQ(dst.data()[3], 4);
	EXPECT_EQ(src.data(), nullptr);
	EXPECT_EQ(dst.data(), oldPtr);
}

TEST(NDArrayMoveAssignment, SelfMoveNoCrashNoChange)
{
	NDArray< int, 1 > arr{ 10, 20, 30 };
	int* before = arr.data();
	size_t len = arr.total_count();

	arr = std::move(arr);
	EXPECT_EQ(arr.data(), before);
	EXPECT_EQ(arr.total_count(), len);
}

struct LifeCounter
{
	static inline int live = 0;
	int v{};

	explicit LifeCounter(int v = 0) : v(v) { ++live; }
	LifeCounter(const LifeCounter& other) : v(other.v) { ++live; }
	LifeCounter(LifeCounter&& other) noexcept : v(other.v)
	{
		++live;
		other.v = -1;
	}

	LifeCounter& operator=(const LifeCounter& o)
	{
		v = o.v;
		return *this;
	}

	LifeCounter& operator=(LifeCounter&& o) noexcept
	{
		v = o.v;
		o.v = -1;
		return *this;
	}

	~LifeCounter() { --live; }
	bool operator==(const LifeCounter& o) const { return v == o.v; }
};

TEST(NDArrayLifetime, ConstructionsEqualDestructions)
{
	{
		NDArray< LifeCounter, 3 > arr{ { { LifeCounter(1), LifeCounter(2) } }, { { LifeCounter(3), LifeCounter(4) } } };
		EXPECT_EQ(LifeCounter::live, 4);
	}
	EXPECT_EQ(LifeCounter::live, 0);
}

TEST(NDArrayConstAccess, DataAndIndexingAreConstCorrect)
{
	const NDArray< std::string, 2 > names{ { std::string("A"), std::string("B") }, { std::string("C"), std::string("D") } };
	EXPECT_EQ(names[1][0], std::string("C"));
}

TEST(NDArrayConstAccess, DataAndIndexingAreConstCorrectTraits)
{
	const NDArray< std::string, 2 > names{ { std::string("A"), std::string("B") }, { std::string("C"), std::string("D") } };

	EXPECT_TRUE((std::is_same_v< decltype(names.data()), const std::string* >))
		<< "data() const should be "
		   "const-qualified";

	EXPECT_TRUE((std::is_same_v< decltype(names[1][0]), std::string const & >)) << "[] const should be const-qualified";

	NDArray< bool, 1 > arr{ true, false, true };
	EXPECT_TRUE((std::is_same_v< decltype(arr.data()), bool* >)) << "data() should not transform bool";
}

TEST(NDArrayComparisonEdge, DifferentShapesNeverEqual)
{
	NDArray< int, 1 > a{ 1, 2, 3 };
	NDArray< int, 1 > b{ 1, 2, 3, 4 };
	EXPECT_FALSE(a.is_equal(b));
}

TEST(NDArrayCopyIndependence, MutateCopyDoesNotAffectOriginal)
{
	NDArray< int, 2 > org{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > cpy(org);

	cpy[0][0] = 99;
	EXPECT_EQ(org[0][0], 1);
	EXPECT_EQ(cpy[0][0], 99);
}

TEST(NDArrayHighNension, SevenNOnes)
{
	NDArray< int, 7 > ones{ { { { { { { 1 } } } } } } };
	EXPECT_EQ(ones.total_count(), 1);
	ones[0][0][0][0][0][0][0] = 42;
	EXPECT_EQ(ones[0][0][0][0][0][0][0], 42);
}

TEST(NDArrayZeroNensional, CopyAndMoveOps)
{
	NDArray< int, 0 > v(7);
	NDArray< int, 0 > cpy = v;
	EXPECT_EQ(*cpy.data(), 7);

	NDArray< int, 0 > mv = std::move(cpy);
	EXPECT_EQ(*mv.data(), 7);
	EXPECT_EQ(cpy.data(), nullptr);
}

TEST(NDArrayBool, BasicOperations)
{
	NDArray< bool, 1 > arr{ true, false, true };

	EXPECT_EQ(arr.count(), 3u);
	EXPECT_TRUE(arr[0]);
	EXPECT_FALSE(arr[1]);
	EXPECT_TRUE(arr[2]);

	arr[1] = true;
	EXPECT_TRUE(arr[1]);
}

struct ThrowOnCopy
{
	inline static int copies = 0;
	inline static int max_copies = 2;
	int id;

	explicit ThrowOnCopy(int id = 0) : id(id) {}

	ThrowOnCopy(const ThrowOnCopy& other) : id(other.id)
	{
		if (++copies > max_copies)
			throw std::runtime_error("copy failed");
	}

	ThrowOnCopy(ThrowOnCopy&&) noexcept = default;
	ThrowOnCopy& operator=(const ThrowOnCopy&) = default;
	ThrowOnCopy& operator=(ThrowOnCopy&&) noexcept = default;

	operator int() const { return id; }

	static void reset() { copies = 0; }

	static void set(int value) { max_copies = value; }

	bool operator==(const ThrowOnCopy& other) const { return id == other.id; }
};

TEST(NDArrayExceptionSafety, CopyCtorLeavesSourceIntactOnThrow)
{
	ThrowOnCopy::reset();
	NDArray< ThrowOnCopy, 1 > good{ ThrowOnCopy(1), ThrowOnCopy(2) };

	EXPECT_THROW(
		(
			[&]
			{
				NDArray< ThrowOnCopy, 1 > doomed(good);
				(void)doomed;
			}()),
		std::runtime_error);

	EXPECT_EQ(good.total_count(), 2u);
	EXPECT_EQ(good[0], ThrowOnCopy(1));
	EXPECT_EQ(good[1], ThrowOnCopy(2));
}

TEST(NDArraySwap, StdSwapsEverything)
{
	NDArray< int, 2 > a{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > b{ { 9, 8 }, { 7, 6 } };

	int* aPtr = a.data();
	int* bPtr = b.data();

	std::swap(a, b);

	EXPECT_EQ(a.data(), bPtr);
	EXPECT_EQ(b.data(), aPtr);
	EXPECT_EQ(a[0][0], 9);
	EXPECT_EQ(b[1][1], 4);
}

TEST(NDArraySwap, SwapsEverything)
{
	NDArray< int, 2 > a{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > b{ { 9, 8 }, { 7, 6 } };

	int* aPtr = a.data();
	int* bPtr = b.data();

	a.swap(b);

	EXPECT_EQ(a.data(), bPtr);
	EXPECT_EQ(b.data(), aPtr);
	EXPECT_EQ(a[0][0], 9);
	EXPECT_EQ(b[1][1], 4);
}
TEST(NDArrayIterator, ForwardAccumulate)
{
	NDArray< int, 1 > arr{ 1, 2, 3, 4 };
	int sum = std::accumulate(arr.begin(), arr.end(), 0);
	EXPECT_EQ(sum, 10);
}

TEST(NDArrayIterator, MutateViaIterator)
{
	NDArray< int, 1 > arr{ 1, 2, 3, 4, 5 };
	for (auto it = arr.begin(); it != arr.end(); ++it)
		*it *= 2;

	EXPECT_EQ(arr[0], 2);
	EXPECT_EQ(arr[4], 10);
}

TEST(NDArrayIterator, ConstAndReverseIterators)
{
	const NDArray< int, 1 > arr{ 1, 2, 3 };
	std::vector< int > fwd(arr.cbegin(), arr.cend());
	EXPECT_EQ(fwd, (std::vector< int >{ 1, 2, 3 }));
}

TEST(NDArrayIterator, ConstAndReverseIteratorsTraits)
{
	const NDArray< int, 1 > arr{ 1, 2, 3 };
	EXPECT_TRUE((std::same_as< decltype(arr.cbegin()), decltype(arr.begin()) >));
}

TEST(NDArraySubview, RowViewIsLive)
{
	NDArray< int, 2 > arr{ { 10, 11 }, { 20, 21 } };
	auto row = arr[1];
	row[0] = 77;
	EXPECT_EQ(arr[1][0], 77);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
