using LinearAlgebra

println("Анализ кривой: Ax² + 2Bxy + Cy² + 2Dx + 2Ey + F = 0")
print("Введите коэффициенты A, B, C, D, E, F через пробел: ")
input = split(readline())
A, B, C, D, E, F = parse.(Float64, input)

# 1. Матрицы
Q = [A B; B C]
QQ = [A B D; B C E; D E F]

# 2. Инварианты
I1 = tr(Q)
I2 = det(Q)
I3 = det(QQ)

# 3. Собственные числа и угол
eigen_vals = eigvals(Q)
λ1, λ2 = eigen_vals
θ = 0.5 * atan(2B, A - C)

# 4. Центр (если I2 != 0)
center = I2 != 0 ? Q \ [-D, -E] : nothing

# 5 & 6. Классификация и Каноническое уравнение
println("\n--- Результаты анализа ---")
println("Инварианты: I1=$I1, I2=$I2, I3=$I3")

if I2 > 0
    if I3 * I1 < 0
        a, b = sqrt(-I3/(λ1*I2)), sqrt(-I3/(λ2*I2))
        println("Тип: Эллипс. Уравнение: x²/($a)² + y²/($b)² = 1")
    elseif I3 * I1 > 0
        a, b = sqrt(I3/(λ1*I2)), sqrt(I3/(λ2*I2))
        println("Тип: Мнимый эллипс. Уравнение: x²/($a)² + y²/($b)² = -1")
    else
        println("Тип: Точка (Мнимые пересекающиеся прямые). Уравнение: x²/a² + y²/b² = 0")
    end
elseif I2 < 0
    if I3 != 0
        a, b = sqrt(abs(I3/(λ1*I2))), sqrt(abs(I3/(λ2*I2)))
        sign_val = -I3/I2 > 0 ? "1" : "-1"
        println("Тип: Гипербола. Уравнение: x²/($a)² - y²/($b)² = $sign_val")
    else
        println("Тип: Пара пересекающихся прямых. Уравнение: x²/a² - y²/b² = 0")
    end
else # I2 == 0
    if I3 != 0
        p = sqrt(-I3 / I1^3)
        println("Тип: Парабола. Уравнение: y² = 2($p)x")
    else
        # Упрощенная проверка для параллельных прямых
        K = det([A D; D F]) + det([C E; E F])
        if K < 0
            println("Тип: Пара параллельных прямых. Уравнение: x² - a² = 0")
        elseif K > 0
            println("Тип: Пара мнимых параллельных прямых. Уравнение: x² + a² = 0")
        else
            println("Тип: Прямая (совпадающие прямые). Уравнение: x² = 0")
        end
    end
end

if center !== nothing
    println("Центр кривой: (x0=$(center[1]), y0=$(center[2]))")
end
println("Угол поворота осей: $(rad2deg(θ))°")
