# Calculs des budgets de temps par image

latence_systeme = 8.0

for hz in [72, 90, 120]:
    duree_image = 1000.0 / hz
    reste = duree_image - latence_systeme
    print(f"{hz} Hz: duree = {duree_image:.1f} ms, reste = {reste:.1f} ms")
