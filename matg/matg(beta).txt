using ForwardDiff
using PlotlyJS

# 1. Определение функции и градиента
f(x, y) = sin(x) * cos(y)
f(v) = f(v[1], v[2]) # Вспомогательная функция для работы с вектором

# Градиент через автоматическое дифференцирование
∇f(v) = ForwardDiff.gradient(f, v)

# 2. Алгоритм градиентного подъема (дивергентный спуск к максимуму)
function gradient_ascent(start_point; lr=0.1, steps=50)
    trajectory = [start_point]
    current_p = start_point
    
    for i in 1:steps
        grad = ∇f(current_p)
        # Идем ПО направлению градиента для поиска максимума
        current_p = current_p + lr * grad
        push!(trajectory, current_p)
    end
    return trajectory
end

# Параметры поиска
start_p = [0.5, 0.5]
path = gradient_ascent(start_p)

# Подготовка данных для визуализации
x_vals = range(-3, 3, length=100)
y_vals = range(-3, 3, length=100)
z_vals = [f(x, y) for x in x_vals, y in y_vals]

path_x = [p[1] for p in path]
path_y = [p[2] for p in path]
path_z = [f(p...) for p in path]

# 3. Построение 3D графика
# Поверхность функции
surface_plot = surface(
    z=z_vals, x=x_vals, y=y_vals, 
    colorscale="Viridis", opacity=0.8
)

# Траектория спуска/подъема
path_plot = scatter3d(
    x=path_x, y=path_y, z=path_z,
    mode="lines+markers",
    line=attr(color="red", width=5),
    marker=attr(size=4, color="white"),
    name="Траектория"
)

config = Layout(
    title="Градиентный подъем: f(x,y) = sin(x)cos(y)",
    scene=attr(xaxis_title="X", yaxis_title="Y", zaxis_title="Z"),
    width=900, height=700
)

# Создание и сохранение графика
p = plot([surface_plot, path_plot], config)

# Сохранение в HTML
open("gradient_plot.html", "w") do io
    PlotlyJS.savefig(io, p, format="html")
end

println("Траектория рассчитана. График сохранен в файл 'gradient_plot.html'")
