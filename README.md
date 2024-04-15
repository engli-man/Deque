# Deque

Необходимо реализовать шаблонный класс Deque<T, Allocator> аналог std::deque из stl.

Deque должен уметь в:
  * Базовую функциональность + итераторы
  * Поддержку аллокаторов
  * Поддержку move-семантики
  * Быть exception-safety

## Методы:

### Constructors: 
  * Deque()
  * Deque(const Allocator&)
  * Deque(const Deque&)
  * Deque(size_t count, const Allocator& alloc = Allocator())
  * Deque(size_t count, const T& value, const Allocator& alloc = Allocator())
  * Deque(Deque&& other)
  * Deque(std::initializer_list\<T\> init, const Allocator& alloc = Allocator())
### Destructor
### operator=:
  * Deque& operator=(const Deque& other)
  * Deque& operator=(Deque&& other)
### capacity:
  * size_t size()
  * bool empty()
### element access methods (работают за гарантированное O(1)):
  * operator[] (без проверок выхода за границу)
  * at() (с проверкой. Кидает std::out_of_range) 
### modifiers: (работают за амортизированное O(1))
  * push_back
  * emplace_back
  * pop_back (без проверок выхода за границу)
  * push_front
  * emplace_front
  * pop_front (без проверок выхода за границу)

## Iterator:
Внутренний тип iterator. Поддерживает:
  * Инкремент, декремент
  * Сложение с целым числом
  * Вычитание целого числа
  * Сравнения <,>,<=,>=,==,!=
  * Взятие разности от двух итераторов
  * operator*
  * operator->
  * Различные using'и: value_type, pointer, iterator_category, reference
Внутренний тип const_iterator. Отличие от обычного в том, что он не позволяет менять лежащий под ним элемент. Конверсия (неявная в том числе) из неконстантного в константный допустима. Обратно - нет.
Внутренний тип reverse_iterator
### modifiers:
  * begin, cbegin - возвращают итератор (константный итератор) на первый элемент дека
  * end, cend - возвращает итератор (константный итератор) на "элемент следующий за последним"
  * rbegin, rend, crbegin. crend - реверс итераторы на соответствующие элементы  
  * insert(iterator, const T&)
  * emplace(iterator, T&&)
  * erase(iterator)

