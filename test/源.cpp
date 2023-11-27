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
    // ��ʼ�� B �������ָ��
    std::shared_ptr<B> bPtr = std::make_shared<B>(1, 2);

    // ʹ�� dynamic_pointer_cast ����ת��
    std::shared_ptr<A> aPtr = std::dynamic_pointer_cast<A>(bPtr);

    // ȷ��ת���ɹ����ʹ��
    assert(aPtr != nullptr && "Dynamic cast failed.");

    // ���������ʹ�� aPtr ������ A ��ĳ�Ա

    return 0;
}
