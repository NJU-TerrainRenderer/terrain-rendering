#include <memory>
#include <cassert>

class A {
public:
    int dataA;
    A(int value) : dataA(value) {}
    virtual void someFunction() {}
};

class B : public A {
public:
    int dataB;
    B(int valueA, int valueB) : A(valueA), dataB(valueB) {}
};

int main() {
    // 初始化 B 类的智能指针
    std::shared_ptr<B> bPtr = std::make_shared<B>(1, 2);

    // 使用 dynamic_pointer_cast 进行转换
    std::shared_ptr<A> aPtr = std::dynamic_pointer_cast<A>(bPtr);

    // 确保转换成功后才使用
    assert(aPtr != nullptr && "Dynamic cast failed.");

    // 现在你可以使用 aPtr 来访问 A 类的成员

    return 0;
}
