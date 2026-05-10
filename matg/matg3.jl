using Plots
# Мы не пишем using PlotlyJS, чтобы избежать конфликта имен. 
# Просто подключаем бэкенд:
plotlyjs() 

# 1. Функция и градиент
f(x, y) = -(x^2 + y^2) + 10
grad_f(x, y) = [-2x, -2y]

# 2. Параметры и расчет траектории
η = 0.1
epochs = 20
curr_p = [4.5, 4.5]
path = [curr_p]

for i in 1:epochs
    global curr_p
    g = grad_f(curr_p...)
    curr_p = curr_p + η * g
    push!(path, curr_p)
end

# Подготовка координат для отрисовки
path_x = [p[1] for p in path]
path_y = [p[2] for p in path]
path_z = [f(p...) for p in path]

# 3. Построение графика с явным указанием модуля Plots
x_range = range(-5, 5, length=50)
y_range = range(-5, 5, length=50)

# Используем Plots.surface вместо просто surface
fig = Plots.surface(x_range, y_range, (x,y) -> f(x,y), 
    alpha=0.6, 
    title="Траектория к максимуму",
    xlabel="X", ylabel="Y", zlabel="Z")

# Добавляем траекторию
Plots.plot!(path_x, path_y, path_z, 
    lw=4, lc=:red, label="Траектория", marker=(:circle, 3))

# Начальная точка
Plots.scatter!([path_x[1]], [path_y[1]], [path_z[1]], 
    color=:yellow, markersize=6, label="Старт")

# 4. Сохранение
Plots.savefig(fig, "gradient_ascent_3d.html")
display(fig)
