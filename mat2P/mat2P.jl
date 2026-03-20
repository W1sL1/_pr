using LinearAlgebra

function solve_conic()
    println("--- Анализ кривой второго порядка ---")
    println("Уравнение: Ax² + 2Bxy + Cy² + 2Dx + 2Ey + F = 0")
    
    #Ввод коэффициентов
    print("Введите A: "); A = parse(Float64, readline())
    print("Введите B (половина коэффициента при xy): "); B = parse(Float64, readline())
    print("Введите C: "); C = parse(Float64, readline())
    print("Введите D (половина коэффициента при x): "); D = parse(Float64, readline())
    print("Введите E (половина коэффициента при y): "); E = parse(Float64, readline())
    print("Введите F: "); F = parse(Float64, readline())

    #Матрицы
    Matrix_small = [A B; B C]
    Matrix_big = [A B D; B C E; D E F]

    #Инварианты
    I1 = tr(Matrix_small)
    I2 = det(Matrix_small)
    I3 = det(Matrix_big)

    println("\nИнварианты: I1 = $I1, I2 = $I2, I3 = $I3")

    #Тип кривой
    if I2 > 0
        type = "Эллипс"
    elseif I2 < 0
        type = "Гипербола"
    else
        type = "Парабола (не поддерживается данным расчетом)"
        println("Тип: $type")
        return
    end
    println("Тип кривой: $type")

    #Собственные числа и векторы
    vals, vecs = eigen(Matrix_small)
    println("Собственные числа: $(vals[1]) и $(vals[2])")
    println("Собственные векторы: v1 = $(vecs[:,1]), v2 = $(vecs[:,2])")

    #Центр (решение системы Ax + By + D = 0 и Bx + Cy + E = 0)
    center = Matrix_small \ [-D, -E]
    x0, y0 = center
    println("Центр: ($x0, $y0)")

    #Приведение к виду λ1*X² + λ2*Y² + I3/I2 = 0
    #Отсюда: λ1*X² + λ2*Y² = -I3/I2
    rhs = -I3 / I2
    a_sq = rhs / vals[1]
    b_sq = rhs / vals[2]

    #Полуоси (берем модули для гиперболы)
    a = sqrt(abs(a_sq))
    b = sqrt(abs(b_sq))
    
    #Сортировка для порядка (a - большая/действительная)
    semiaxes = sort([a, b], rev=true)
    
    println("Полуоси: a ≈ $(round(semiaxes[1], digits=4)), b ≈ $(round(semiaxes[2], digits=4))")

    #Эксцентриситет
    if type == "Эллипс"
        e = sqrt(1 - (min(a,b)^2 / max(a,b)^2))
    else #Гипербола
        #Для гиперболы: e = sqrt(1 + b²/a²) где a - действительная полуось
        #В каноническом виде: λ1*x² + λ2*y² = rhs. 
        #Если λ и rhs имеют разные знаки — это действительная ось.
        if sign(vals[1]) != sign(rhs)
            e = sqrt(1 + abs(vals[2]/vals[1]))
        else
            e = sqrt(1 + abs(vals[1]/vals[2]))
        end
    end
    println("Эксцентриситет: e ≈ $(round(e, digits=4))")

    println("\nНажмите Enter, чтобы выйти...")
    readline()
end
solve_conic()