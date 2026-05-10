using Plots
using LinearAlgebra
plotlyjs() # Интерактивный движок
# зайти в C:\_pr\matg

# 1. Функция и градиент
f(x, y) = -(x^2 + y^2) + 4
grad_f(x, y) = [-2x, -2y]

# 2. Оптимизация
x_curr, y_curr = -1.5, -1.5
η = 0.1
steps = 15
path = [(x_curr, y_curr, f(x_curr, y_curr))]

for i in 1:steps
    global x_curr, y_curr
    g = grad_f(x_curr, y_curr)
    x_curr += η * g[1]
    y_curr += η * g[2]
    push!(path, (x_curr, y_curr, f(x_curr, y_curr)))
end

# 3. Данные для визуализации
xs = range(-2, 2, length=40)
ys = range(-2, 2, length=40)
zs = [f(x, y) for y in ys, x in xs]

px = [p[1] for p in path]
py = [p[2] for p in path]
pz = [p[3] for p in path]

# 4. Построение
p = surface(xs, ys, zs, alpha=0.6, color=:viridis, title="3D Trajectory")
plot!(px, py, pz, lw=4, color=:red, label="Path", marker=:circle, markersize=3)

# 5. Сохранение 
savefig(p, "gradient_plot.html")
