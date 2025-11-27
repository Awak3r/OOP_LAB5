#include <gtest/gtest.h>
#include "../include/memory.h"
#include "../include/list.h"
#include <string>
#include <sstream>

// ==================== CustomMemoryResource Tests ====================

TEST(CustomMemoryResourceTest, AllocateNewBlock) {
    CustomMemoryResource mr;
    void* ptr = mr.allocate(64, alignof(int));
    ASSERT_NE(ptr, nullptr);
    mr.deallocate(ptr, 64, alignof(int));
}

TEST(CustomMemoryResourceTest, AllocateMultipleBlocks) {
    CustomMemoryResource mr;
    void* ptr1 = mr.allocate(32, alignof(int));
    void* ptr2 = mr.allocate(64, alignof(int));
    void* ptr3 = mr.allocate(128, alignof(int));
    
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr3, nullptr);
    ASSERT_NE(ptr1, ptr2);
    ASSERT_NE(ptr2, ptr3);
    
    mr.deallocate(ptr1, 32, alignof(int));
    mr.deallocate(ptr2, 64, alignof(int));
    mr.deallocate(ptr3, 128, alignof(int));
}

TEST(CustomMemoryResourceTest, ReuseFreedBlock) {
    CustomMemoryResource mr;
    
    // Выделяем и освобождаем блок
    void* ptr1 = mr.allocate(64, alignof(int));
    ASSERT_NE(ptr1, nullptr);
    mr.deallocate(ptr1, 64, alignof(int));
    
    // Выделяем блок того же размера - должен переиспользоваться
    void* ptr2 = mr.allocate(64, alignof(int));
    ASSERT_EQ(ptr1, ptr2); // Тот же адрес!
    
    mr.deallocate(ptr2, 64, alignof(int));
}

TEST(CustomMemoryResourceTest, ReuseAfterMultipleDeallocations) {
    CustomMemoryResource mr;
    
    void* ptr1 = mr.allocate(32, alignof(int));
    void* ptr2 = mr.allocate(64, alignof(int));
    void* ptr3 = mr.allocate(32, alignof(int));
    
    // Освобождаем первый блок
    mr.deallocate(ptr1, 32, alignof(int));
    
    // Выделяем новый блок - должен переиспользовать ptr1
    void* ptr4 = mr.allocate(32, alignof(int));
    ASSERT_EQ(ptr1, ptr4);
    
    mr.deallocate(ptr2, 64, alignof(int));
    mr.deallocate(ptr3, 32, alignof(int));
    mr.deallocate(ptr4, 32, alignof(int));
}

TEST(CustomMemoryResourceTest, DeallocateNonexistentBlock) {
    CustomMemoryResource mr;
    int dummy = 42;
    void* fake_ptr = &dummy;
    
    ASSERT_THROW(
        mr.deallocate(fake_ptr, 64, alignof(int)),
        std::logic_error
    );
}

TEST(CustomMemoryResourceTest, DeallocateWrongSize) {
    CustomMemoryResource mr;
    void* ptr = mr.allocate(64, alignof(int));
    
    // Пытаемся освободить с неправильным размером
    ASSERT_THROW(
        mr.deallocate(ptr, 128, alignof(int)),
        std::logic_error
    );
    
    // Правильное освобождение
    mr.deallocate(ptr, 64, alignof(int));
}

TEST(CustomMemoryResourceTest, IsEqualSameInstance) {
    CustomMemoryResource mr;
    ASSERT_TRUE(mr.is_equal(mr));
}

TEST(CustomMemoryResourceTest, IsEqualDifferentInstances) {
    CustomMemoryResource mr1;
    CustomMemoryResource mr2;
    ASSERT_FALSE(mr1.is_equal(mr2));
}

// ==================== Node Tests ====================

TEST(NodeTest, ConstructorWithValue) {
    int value = 42;
    Node<int> node(value);
    ASSERT_EQ(node.value, 42);
    ASSERT_EQ(node.next, nullptr);
    ASSERT_EQ(node.prev, nullptr);
}

TEST(NodeTest, ConstructorWithString) {
    std::string str = "Hello";
    Node<std::string> node(str);
    ASSERT_EQ(node.value, "Hello");
    ASSERT_EQ(node.next, nullptr);
    ASSERT_EQ(node.prev, nullptr);
}

TEST(NodeTest, Linking) {
    int val1 = 10, val2 = 20;
    Node<int> node1(val1);
    Node<int> node2(val2);
    
    node1.next = &node2;
    node2.prev = &node1;
    
    ASSERT_EQ(node1.next, &node2);
    ASSERT_EQ(node2.prev, &node1);
    ASSERT_EQ(node1.next->value, 20);
    ASSERT_EQ(node2.prev->value, 10);
}

// ==================== Iterator Tests ====================

TEST(IteratorTest, Dereference) {
    int value = 100;
    Node<int> node(value);
    Iterator<int> it(&node);
    
    ASSERT_EQ(*it, 100);
}

TEST(IteratorTest, ArrowOperator) {
    struct TestStruct {
        int x;
        int y;
    };
    
    TestStruct ts{10, 20};
    Node<TestStruct> node(ts);
    Iterator<TestStruct> it(&node);
    
    ASSERT_EQ(it->x, 10);
    ASSERT_EQ(it->y, 20);
}

TEST(IteratorTest, PrefixIncrement) {
    int val1 = 1, val2 = 2;
    Node<int> node1(val1);
    Node<int> node2(val2);
    node1.next = &node2;
    
    Iterator<int> it(&node1);
    ASSERT_EQ(*it, 1);
    
    ++it;
    ASSERT_EQ(*it, 2);
}

TEST(IteratorTest, PostfixIncrement) {
    int val1 = 1, val2 = 2;
    Node<int> node1(val1);
    Node<int> node2(val2);
    node1.next = &node2;
    
    Iterator<int> it(&node1);
    Iterator<int> old_it = it++;
    
    ASSERT_EQ(*old_it, 1);
    ASSERT_EQ(*it, 2);
}

TEST(IteratorTest, Equality) {
    int value = 42;
    Node<int> node(value);
    
    Iterator<int> it1(&node);
    Iterator<int> it2(&node);
    Iterator<int> it3(nullptr);
    
    ASSERT_TRUE(it1 == it2);
    ASSERT_FALSE(it1 == it3);
}

TEST(IteratorTest, Inequality) {
    int val1 = 1, val2 = 2;
    Node<int> node1(val1);
    Node<int> node2(val2);
    
    Iterator<int> it1(&node1);
    Iterator<int> it2(&node2);
    
    ASSERT_TRUE(it1 != it2);
    ASSERT_FALSE(it1 != it1);
}

// ==================== DoubleLinkedList Basic Tests ====================

TEST(DoubleLinkedListTest, DefaultConstructor) {
    DoubleLinkedList<int> list;
    ASSERT_EQ(list.begin(), list.end());
}

TEST(DoubleLinkedListTest, ConstructorWithCustomMemoryResource) {
    CustomMemoryResource mr;
    DoubleLinkedList<int> list(&mr);
    ASSERT_EQ(list.begin(), list.end());
}

TEST(DoubleLinkedListTest, PushBackSingleElement) {
    DoubleLinkedList<int> list;
    list.push_back(42);
    
    auto it = list.begin();
    ASSERT_NE(it, list.end());
    ASSERT_EQ(*it, 42);
    
    ++it;
    ASSERT_EQ(it, list.end());
}

TEST(DoubleLinkedListTest, PushBackMultipleElements) {
    DoubleLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    auto it = list.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    ++it;
    ASSERT_EQ(*it, 3);
    ++it;
    ASSERT_EQ(it, list.end());
}

TEST(DoubleLinkedListTest, PushBackStrings) {
    DoubleLinkedList<std::string> list;
    list.push_back("Hello");
    list.push_back("World");
    list.push_back("!");
    
    auto it = list.begin();
    ASSERT_EQ(*it, "Hello");
    ++it;
    ASSERT_EQ(*it, "World");
    ++it;
    ASSERT_EQ(*it, "!");
}

TEST(DoubleLinkedListTest, PopBackSingleElement) {
    DoubleLinkedList<int> list;
    list.push_back(42);
    list.pop_back();
    
    ASSERT_EQ(list.begin(), list.end());
}

TEST(DoubleLinkedListTest, PopBackMultipleElements) {
    DoubleLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    list.pop_back();
    auto it = list.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    ++it;
    ASSERT_EQ(it, list.end());
    
    list.pop_back();
    it = list.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(it, list.end());
    
    list.pop_back();
    ASSERT_EQ(list.begin(), list.end());
}

TEST(DoubleLinkedListTest, PopBackEmptyList) {
    DoubleLinkedList<int> list;
    list.pop_back(); // Не должно вызывать ошибку
    ASSERT_EQ(list.begin(), list.end());
}

TEST(DoubleLinkedListTest, ClearEmptyList) {
    DoubleLinkedList<int> list;
    list.clear();
    ASSERT_EQ(list.begin(), list.end());
}

TEST(DoubleLinkedListTest, ClearNonEmptyList) {
    DoubleLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    list.clear();
    ASSERT_EQ(list.begin(), list.end());
}

TEST(DoubleLinkedListTest, PushBackAfterClear) {
    DoubleLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.clear();
    
    list.push_back(42);
    auto it = list.begin();
    ASSERT_EQ(*it, 42);
    ++it;
    ASSERT_EQ(it, list.end());
}

// ==================== Memory Reuse Tests ====================

TEST(MemoryReuseTest, AllocateAndDeallocate) {
    CustomMemoryResource mr;
    DoubleLinkedList<int> list(&mr);
    
    // Добавляем элементы
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    // Удаляем элементы (память помечается как свободная)
    list.pop_back();
    list.pop_back();
    
    // Добавляем новые элементы (должны переиспользовать память)
    list.push_back(10);
    list.push_back(20);
    
    auto it = list.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 10);
    ++it;
    ASSERT_EQ(*it, 20);
}

TEST(MemoryReuseTest, MultipleListsSharedMemoryResource) {
    CustomMemoryResource mr;
    
    {
        DoubleLinkedList<int> list1(&mr);
        list1.push_back(1);
        list1.push_back(2);
        // list1 уничтожается, память помечается как свободная
    }
    
    {
        DoubleLinkedList<int> list2(&mr);
        list2.push_back(10); // Должен переиспользовать память от list1
        list2.push_back(20);
        
        auto it = list2.begin();
        ASSERT_EQ(*it, 10);
        ++it;
        ASSERT_EQ(*it, 20);
    }
}

// ==================== Range-Based For Loop Tests ====================

TEST(RangeBasedForTest, SimpleIteration) {
    DoubleLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    int sum = 0;
    for (const auto& val : list) {
        sum += val;
    }
    
    ASSERT_EQ(sum, 6);
}

TEST(RangeBasedForTest, ModifyElements) {
    DoubleLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    for (auto& val : list) {
        val *= 2;
    }
    
    auto it = list.begin();
    ASSERT_EQ(*it, 2);
    ++it;
    ASSERT_EQ(*it, 4);
    ++it;
    ASSERT_EQ(*it, 6);
}

TEST(RangeBasedForTest, EmptyList) {
    DoubleLinkedList<int> list;
    
    int count = 0;
    for (const auto& val : list) {
        (void)val;
        count++;
    }
    
    ASSERT_EQ(count, 0);
}

// ==================== Complex Type Tests ====================

struct ComplexType {
    int id;
    std::string name;
    double value;
    
    ComplexType(int i, const std::string& n, double v) 
        : id(i), name(n), value(v) {}
    
    bool operator==(const ComplexType& other) const {
        return id == other.id && name == other.name && value == other.value;
    }
};

TEST(ComplexTypeTest, PushBackComplexType) {
    DoubleLinkedList<ComplexType> list;
    
    list.push_back(ComplexType(1, "First", 1.5));
    list.push_back(ComplexType(2, "Second", 2.5));
    list.push_back(ComplexType(3, "Third", 3.5));
    
    auto it = list.begin();
    ASSERT_EQ(it->id, 1);
    ASSERT_EQ(it->name, "First");
    ASSERT_NEAR(it->value, 1.5, 1e-6);
    
    ++it;
    ASSERT_EQ(it->id, 2);
    ++it;
    ASSERT_EQ(it->id, 3);
}

TEST(ComplexTypeTest, ClearComplexType) {
    DoubleLinkedList<ComplexType> list;
    
    for (int i = 0; i < 10; ++i) {
        list.push_back(ComplexType(i, "Item" + std::to_string(i), i * 1.5));
    }
    
    list.clear();
    ASSERT_EQ(list.begin(), list.end());
}

// ==================== Stress Tests ====================

TEST(StressTest, ManyPushBacks) {
    CustomMemoryResource mr;
    DoubleLinkedList<int> list(&mr);
    
    const int N = 1000;
    for (int i = 0; i < N; ++i) {
        list.push_back(i);
    }
    
    int count = 0;
    for (const auto& val : list) {
        ASSERT_EQ(val, count);
        count++;
    }
    
    ASSERT_EQ(count, N);
}

TEST(StressTest, ManyPushBacksAndPopBacks) {
    CustomMemoryResource mr;
    DoubleLinkedList<int> list(&mr);
    
    // Добавляем 500 элементов
    for (int i = 0; i < 500; ++i) {
        list.push_back(i);
    }
    
    // Удаляем 250 элементов
    for (int i = 0; i < 250; ++i) {
        list.pop_back();
    }
    
    // Добавляем ещё 250 элементов (должны переиспользовать память)
    for (int i = 500; i < 750; ++i) {
        list.push_back(i);
    }
    
    // Проверяем первые 250 элементов
    auto it = list.begin();
    for (int i = 0; i < 250; ++i) {
        ASSERT_EQ(*it, i);
        ++it;
    }
    
    // Проверяем последние 250 элементов
    for (int i = 500; i < 750; ++i) {
        ASSERT_EQ(*it, i);
        ++it;
    }
    
    ASSERT_EQ(it, list.end());
}

TEST(StressTest, AlternatingPushPopOperations) {
    DoubleLinkedList<int> list;
    
    for (int cycle = 0; cycle < 100; ++cycle) {
        // Добавляем 10 элементов
        for (int i = 0; i < 10; ++i) {
            list.push_back(cycle * 10 + i);
        }
        
        // Удаляем 5 элементов
        for (int i = 0; i < 5; ++i) {
            list.pop_back();
        }
    }
    
    // В итоге должно остаться 100 * 5 = 500 элементов
    int count = 0;
    for (const auto& val : list) {
        (void)val;
        count++;
    }
    
    ASSERT_EQ(count, 500);
}

// ==================== Edge Cases ====================

TEST(EdgeCaseTest, SingleElementOperations) {
    DoubleLinkedList<int> list;
    
    list.push_back(42);
    auto it = list.begin();
    ASSERT_EQ(*it, 42);
    
    list.pop_back();
    ASSERT_EQ(list.begin(), list.end());
    
    list.push_back(100);
    it = list.begin();
    ASSERT_EQ(*it, 100);
}

TEST(EdgeCaseTest, MultipleClears) {
    DoubleLinkedList<int> list;
    
    for (int cycle = 0; cycle < 5; ++cycle) {
        for (int i = 0; i < 10; ++i) {
            list.push_back(i);
        }
        list.clear();
        ASSERT_EQ(list.begin(), list.end());
    }
}

TEST(EdgeCaseTest, LargeValues) {
    DoubleLinkedList<long long> list;
    
    list.push_back(1000000000LL);
    list.push_back(2000000000LL);
    list.push_back(3000000000LL);
    
    auto it = list.begin();
    ASSERT_EQ(*it, 1000000000LL);
    ++it;
    ASSERT_EQ(*it, 2000000000LL);
    ++it;
    ASSERT_EQ(*it, 3000000000LL);
}

// ==================== Integration Tests ====================

TEST(IntegrationTest, MultipleListsDifferentTypes) {
    CustomMemoryResource mr;
    
    DoubleLinkedList<int> intList(&mr);
    DoubleLinkedList<std::string> strList(&mr);
    DoubleLinkedList<double> doubleList(&mr);
    
    intList.push_back(1);
    intList.push_back(2);
    
    strList.push_back("Hello");
    strList.push_back("World");
    
    doubleList.push_back(3.14);
    doubleList.push_back(2.71);
    
    // Проверяем int список
    auto it1 = intList.begin();
    ASSERT_EQ(*it1, 1);
    ++it1;
    ASSERT_EQ(*it1, 2);
    
    // Проверяем string список
    auto it2 = strList.begin();
    ASSERT_EQ(*it2, "Hello");
    ++it2;
    ASSERT_EQ(*it2, "World");
    
    // Проверяем double список
    auto it3 = doubleList.begin();
    ASSERT_NEAR(*it3, 3.14, 1e-6);
    ++it3;
    ASSERT_NEAR(*it3, 2.71, 1e-6);
}



// ==================== Main ====================

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}