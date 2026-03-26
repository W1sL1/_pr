using LinearAlgebra

function analyze_conic()
    println("--- Анализ кривой второго порядка Ax² + 2Bxy + Cy² + 2Dx + 2Ey + F = 0 ---")
    
    # 1. Ввод коэффициентов
    print("Введите A: "); A = parse(Float64, readline())
    print("Введите B (коэфф. при xy, деленный на 2): "); B = parse(Float64, readline())
    print("Введите C: "); C = parse(Float64, readline())
    print("Введите D (коэфф. при x, деленный на 2): "); D = parse(Float64, readline())
    print("Введите E (коэфф. при y, деленный на 2): "); E = parse(Float64, readline())
    print("Введите F: "); F = parse(Float64, readline())

    # 1. Матрицы
    Q = [A B; B C]          # Матрица квадратичной формы
    QQ = [A B D; B C E; D E F] # Расширенная матрица

    # 2. Инварианты
    I1 = tr(Q)              # A + C
    I2 = det(Q)             # AC - B²
    I3 = det(QQ)            # Определитель расширенной матрицы

    println("\nИнварианты: I1 = $I1, I2 = $I2, I3 = $I3")

    # 3. Собственные числа и угол поворота
    eigen_decomp = eigen(Q)
    λ1, λ2 = eigen_decomp.values
    α = 0.5 * atan(2B, A - C) # Угол поворота в радианах
    
    println("Собственные числа: λ1 = $λ1, λ2 = $λ2")
    println("Угол поворота осей: $(rad2deg(α))°")

    # 4. Поиск центра (x0, y0)
    # Система: A*x + B*y + D = 0; B*x + C*y + E = 0
    local center = nothing
    if abs(I2) > 1e-9
        center = Q \ [-D, -E]
        println("Центр кривой: ($(center[1]), $(center[2]))")
    else
        println("Кривая не имеет единственного центра (параболический тип).")
    end

    # 5. Классификация
    print("Тип фигуры: ")
    if I2 > 0
        if I3 != 0
            (I1 * I3 < 0) ? println("Эллипс") : println("Мнимый эллипс")
        else
            println("Точка (пара мнимых пересекающихся прямых)")
        end
    elseif I2 < 0
        (I3 != 0) ? println("Гипербола") : println("Пара пересекающихся прямых")
    else # I2 == 0
        if I3 != 0
            println("Парабола")
        else
            # Анализ вырожденных параллельных прямых через доп. определитель
            # K = det([A D; D F]) + det([C E; E F])
            K = (A*F - D^2) + (C*F - E^2)
            if K < 0
                println("Пара параллельных прямых")
            elseif K > 0
                println("Пара мнимых параллельных прямых")
            else
                println("Прямая (пара совпадающих прямых)")
            end
        end
    end

    # 6. Каноническое уравнение
    println("\n--- Канонический вид ---")
    if I2 != 0
        # λ1*X² + λ2*Y² + I3/I2 = 0
        free_term = I3 / I2
        println("$(λ1)X² + $(λ2)Y² + ($free_term) = 0")
    else
        # Случай параболы: λ1*X² + 2*sqrt(-I3/I1)*Y = 0 (если λ1 != 0)
        # Упрощенная модель для демонстрации
        if I3 != 0
            p = sqrt(-I3 / I1^3)
            println("Y² = 2 * ($p) * X (или X² = 2py)")
        else
            println("Уравнение распадается на прямые.")
        end
    end
end

analyze_conic()
