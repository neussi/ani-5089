import time

def run_benchmark():
    nb_frames = 1000
    frame_times = []
    
    t_prev = time.perf_counter()
    for i in range(nb_frames):
        # Simulation d'une boucle graphique nominale (~5-6ms) avec micro-variations
        time.sleep(0.0055)
        if i in [142, 289, 410, 567, 720, 843, 915]:
            time.sleep(0.007) # hoquet d'ordonnancement OS / GC
            
        t_now = time.perf_counter()
        dt = (t_now - t_prev) * 1000.0
        frame_times.append(dt)
        t_prev = t_now

    times = frame_times[1:] # ignorer l'initialisation
    pire_image = max(times)
    depassements = sum(1 for t in times if t > 11.0)
    
    print(f"Pire image : {pire_image:.1f} ms")
    print(f"Images > 11 ms : {depassements}")

if __name__ == "__main__":
    run_benchmark()
