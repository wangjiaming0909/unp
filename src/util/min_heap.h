#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <vector>
#include <queue>
#include <memory>

namespace util {

template <typename T>
class min_heap
{
public:
    using compare_type = std::greater<T>;
    using container_type = std::priority_queue<T, std::vector<T>, compare_type>;
    using size_type = typename container_type::size_type;
    explicit min_heap() : container_(){}

    static std::shared_ptr<min_heap> make_min_heap(const std::vector<T>& c)
    {
        std::shared_ptr<min_heap> target = std::make_shared<min_heap>();
        target->container_ = c;
        std::make_heap(target->container_.begin(), target->container_.end(), compare_type());
        return target;
    }
public:
    bool empty() const {return container_.empty();}
    size_type size() const {return container_.size();}
    size_type max_size() const {return container_.max_size();}
    void clear() {container_.clear();}
    const T& top() const {return container_.top();}
    void push(const T& t) { container_.push(t);}
    template<typename... Args >
    void emplace(Args&&... args)
    {
       container_.emplace(args...);
    }

    void pop(){container_.pop();}
    void reverse() {container_.reverse(container_.size());}
private:
    container_type container_;
};

} //namespace util
#endif // MIN_HEAP_H
