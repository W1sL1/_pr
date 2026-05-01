# Функция для вычисления градиента методом центральных разностей
function compute_gradient(f, x, h=1e-6)
    n = length(x)
    grad = zeros(Float64, n)
    
    for i in 1:n
        # Создаем копии вектора для смещения
        x_plus = copy(x)
        x_minus = copy(x)
        
        # Смещаем только одну координату
        x_plus[i] += h
        x_minus[i] -= h
        
        # Вычисляем частную производную
        grad[i] = (f(x_plus) - f(x_minus)) / (2 * h)
    end
    
    return grad
end

# Основной блок программы
println("--- Вычисление градиента функции ---")

# 1. Определение функции пользователем
# Пример: f(x) = x[1]^2 + x[2]^3 + sin(x[3])
println("Введите выражение функции через x[1], x[2] и т.д. (на языке Julia).")
println("Пример: x[1]^2 + 3*x[2]")
print("f(x) = ")
input_str = readline()

# Превращаем строку в исполняемую функцию
try
    user_expr = Meta.parse("x -> " * input_str)
    f = eval(user_expr)

    # 2. Ввод начальной точки
    print("\nВведите координаты точки через пробел (например: 1.0 2.5 0): ")
    point_str = readline()
    x0 = [parse(Float64, s) for s in split(point_str)]

    # 3. Вычисление
    gradient = compute_gradient(f, x0)

    # 4. Вывод результата
    println("\nРезультаты:")
    println("Точка x: ", x0)
    println("Градиент ∇f(x): ", gradient)

catch e
    println("\nОшибка: Проверьте правильность ввода функции или координат.")
    println("Детали: ", e)

finally
        # ЭТА ЧАСТЬ НЕ ДАСТ ОКНУ ЗАКРЫТЬСЯ
        println("\nНажмите Enter, чтобы выйти...")
        readline()
end