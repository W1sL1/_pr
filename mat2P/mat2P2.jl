# Подключение модуля для линейной алгебры (матрицы, определители, собственные значения)
using LinearAlgebra

# Определение функции для анализа кривой второго порядка
function solve_conic()
    # Вывод заголовка и описания формата уравнения
    println("--- Анализ кривой второго порядка ---")
    println("Уравнение: Ax² + 2Bxy + Cy² + 2Dx + 2Ey + F = 0")
    
    # === Ввод коэффициентов ===
    print("Введите A: "); A = parse(Float64, readline())
    print("Введите B (половина коэффициента при xy): "); B = parse(Float64, readline())
    print("Введите C: "); C = parse(Float64, readline())
    print("Введите D (половина коэффициента при x): "); D = parse(Float64, readline())
    print("Введите E (половина коэффициента при y): "); E = parse(Float64, readline())
    print("Введите F: "); F = parse(Float64, readline())

    # === Формирование матриц ===
    Matrix_small = [A B; B C]      # Матрица квадратичной формы 2×2
    Matrix_big = [A B D; B C E; D E F]  # Полная матрица кривой 3×3

    # === Вычисление инвариантов ===
    I1 = tr(Matrix_small)      # Первый инвариант
    I2 = det(Matrix_small)     # Второй инвариант (определитель)
    I3 = det(Matrix_big)       # Третий инвариант (определитель)
    
    # Вычисление дополнительных инвариантов для вырожденных случаев
    K = det(Matrix_small) * I3  # Инвариант для определения типа вырожденных кривых

    # Вывод вычисленных инвариантов
    println("\nИнварианты: I1 = $I1, I2 = $I2, I3 = $I3")

    # === Определение типа кривой ===
    type = ""
    canonical_eq = ""
    
    # Проверка на вырожденные случаи (I3 = 0)
    if abs(I3) < 1e-10  # I3 = 0 - вырожденная кривая
        if I2 > 0
            if I1 * I3 < 0 || (abs(I1) < 1e-10 && I3 == 0)
                type = "Точка"
                canonical_eq = "X²/a² + Y²/b² = 0 (точка)"
            else
                type = "Мнимый эллипс"
                canonical_eq = "X²/a² + Y²/b² = -1 (мнимый эллипс)"
            end
        elseif I2 < 0
            type = "Пара пересекающихся прямых"
            # Находим уравнения прямых
            canonical_eq = "X²/a² - Y²/b² = 0 (две пересекающиеся прямые)"
        elseif I2 == 0
            if I1 * I3 < 0
                type = "Пара параллельных прямых"
                canonical_eq = "X²/a² = 1 (две параллельные прямые)"
            elseif I1 * I3 > 0
                type = "Пара мнимых параллельных прямых"
                canonical_eq = "X²/a² = -1 (две мнимые параллельные прямые)"
            else
                type = "Прямая (пара совпадающих прямых)"
                canonical_eq = "X² = 0 (одна прямая)"
            end
        end
    else  # I3 ≠ 0 - невырожденная кривая
        if I2 > 0
            if I1 * I3 < 0
                type = "Эллипс"
                # Для эллипса каноническое уравнение: X²/a² + Y²/b² = 1
                # Находим собственные значения
                vals, vecs = eigen(Matrix_small)
                rhs = -I3 / I2
                a_sq = rhs / vals[1]
                b_sq = rhs / vals[2]
                if a_sq < 0 || b_sq < 0
                    # Мнимый эллипс
                    type = "Мнимый эллипс"
                    a = sqrt(abs(a_sq))
                    b = sqrt(abs(b_sq))
                    a, b = sort([a, b], rev=true)
                    canonical_eq = "X²/$(round(a, digits=4))² + Y²/$(round(b, digits=4))² = -1"
                else
                    a = sqrt(a_sq)
                    b = sqrt(b_sq)
                    a, b = sort([a, b], rev=true)
                    canonical_eq = "X²/$(round(a, digits=4))² + Y²/$(round(b, digits=4))² = 1"
                end
            else
                type = "Мнимый эллипс"
                vals, vecs = eigen(Matrix_small)
                rhs = -I3 / I2
                a_sq = rhs / vals[1]
                b_sq = rhs / vals[2]
                a = sqrt(abs(a_sq))
                b = sqrt(abs(b_sq))
                a, b = sort([a, b], rev=true)
                canonical_eq = "X²/$(round(a, digits=4))² + Y²/$(round(b, digits=4))² = -1"
            end
        elseif I2 < 0
            type = "Гипербола"
            vals, vecs = eigen(Matrix_small)
            rhs = -I3 / I2
            # Определяем знаки для канонического уравнения
            if vals[1] * rhs > 0
                a = sqrt(abs(rhs / vals[1]))
                b = sqrt(abs(rhs / vals[2]))
                canonical_eq = "X²/$(round(a, digits=4))² - Y²/$(round(b, digits=4))² = 1"
            else
                a = sqrt(abs(rhs / vals[2]))
                b = sqrt(abs(rhs / vals[1]))
                canonical_eq = "Y²/$(round(a, digits=4))² - X²/$(round(b, digits=4))² = 1"
            end
        else  # I2 == 0
            type = "Парабола"
            # Для параболы каноническое уравнение: Y² = 2pX или X² = 2pY
            # Находим собственные значения
            vals, vecs = eigen(Matrix_small)
            # Вычисляем инвариант для параболы
            Δ = det(Matrix_big)
            # Определяем направление параболы
            if abs(vals[1]) > abs(vals[2])
                p = -Δ / (vals[1]^2 * (vals[1] + vals[2]))
                canonical_eq = "Y² = 2*$(round(p, digits=4))*X"
            else
                p = -Δ / (vals[2]^2 * (vals[1] + vals[2]))
                canonical_eq = "X² = 2*$(round(p, digits=4))*Y"
            end
        end
    end
    
    println("Тип кривой: $type")
    println("Каноническое уравнение: $canonical_eq")

    # === Нахождение центра кривой ===
    if abs(I2) > 1e-10  # Для невырожденных кривых с центром
        # Решение системы уравнений:
        # A*x + B*y + D = 0
        # B*x + C*y + E = 0
        center = Matrix_small \ [-D, -E]
        x0, y0 = center
        println("Центр: ($(round(x0, digits=4)), $(round(y0, digits=4)))")
        
        # Проверка правильности центра
        check1 = A*x0 + B*y0 + D
        check2 = B*x0 + C*y0 + E
        if abs(check1) < 1e-10 && abs(check2) < 1e-10
            println("✓ Центр определен верно (погрешность: $(round(max(abs(check1), abs(check2)), digits=10)))")
        else
            println("⚠ Центр определен с погрешностью: $(round(max(abs(check1), abs(check2)), digits=10))")
        end
    elseif type == "Парабола"
        println("Центр: не определен (парабола не имеет центра)")
    elseif type in ["Пара параллельных прямых", "Пара мнимых параллельных прямых", "Прямая (пара совпадающих прямых)"]
        # Для параллельных прямых центр не определен, но есть ось симметрии
        println("Центр: не определен (кривая не имеет центра симметрии)")
        # Находим ось симметрии
        if abs(A) > 1e-10
            println("Ось симметрии: вертикальная прямая")
        elseif abs(C) > 1e-10
            println("Ось симметрии: горизонтальная прямая")
        end
    elseif type in ["Пара пересекающихся прямых", "Точка"]
        center = Matrix_small \ [-D, -E]
        x0, y0 = center
        println("Центр (точка пересечения): ($(round(x0, digits=4)), $(round(y0, digits=4)))")
        
        # Проверка правильности центра
        check1 = A*x0 + B*y0 + D
        check2 = B*x0 + C*y0 + E
        if abs(check1) < 1e-10 && abs(check2) < 1e-10
            println("✓ Центр определен верно (погрешность: $(round(max(abs(check1), abs(check2)), digits=10)))")
        end
    end

    # === Нахождение собственных чисел и векторов для невырожденных кривых ===
    if abs(I2) > 1e-10  # Для кривых с ненулевым I2
        vals, vecs = eigen(Matrix_small)
        println("\nСобственные числа: $(round(vals[1], digits=4)) и $(round(vals[2], digits=4))")
        println("Собственные векторы: v1 = ($(round(vecs[1,1], digits=4)), $(round(vecs[2,1], digits=4))), v2 = ($(round(vecs[1,2], digits=4)), $(round(vecs[2,2], digits=4)))")
    end

    # === Дополнительная информация для разных типов кривых ===
    if type == "Эллипс" && abs(I3) > 1e-10
        # Вычисление полуосей и эксцентриситета для эллипса
        vals, vecs = eigen(Matrix_small)
        rhs = -I3 / I2
        a_sq = abs(rhs / vals[1])
        b_sq = abs(rhs / vals[2])
        a, b = sort([sqrt(a_sq), sqrt(b_sq)], rev=true)
        e = sqrt(1 - (b^2 / a^2))
        println("\nПараметры эллипса:")
        println("  Большая полуось a = $(round(a, digits=4))")
        println("  Малая полуось b = $(round(b, digits=4))")
        println("  Эксцентриситет e = $(round(e, digits=4))")
        println("  Фокусное расстояние c = $(round(sqrt(a^2 - b^2), digits=4))")
    elseif type == "Гипербола"
        # Вычисление параметров гиперболы
        vals, vecs = eigen(Matrix_small)
        rhs = -I3 / I2
        # Определяем действительную и мнимую полуоси
        if vals[1] * rhs > 0
            a = sqrt(abs(rhs / vals[1]))
            b = sqrt(abs(rhs / vals[2]))
        else
            a = sqrt(abs(rhs / vals[2]))
            b = sqrt(abs(rhs / vals[1]))
        end
        e = sqrt(1 + (b^2 / a^2))
        println("\nПараметры гиперболы:")
        println("  Действительная полуось a = $(round(a, digits=4))")
        println("  Мнимая полуось b = $(round(b, digits=4))")
        println("  Эксцентриситет e = $(round(e, digits=4))")
        println("  Фокусное расстояние c = $(round(sqrt(a^2 + b^2), digits=4))")
    end

    # === Ожидание нажатия Enter перед завершением ===
    println("\nНажмите Enter, чтобы выйти...")
    readline()
end

# === Вызов функции для выполнения программы ===
solve_conic()