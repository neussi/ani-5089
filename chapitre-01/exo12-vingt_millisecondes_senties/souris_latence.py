import time
import collections

# Programme de test de suivi de souris avec retard reglable (0 a 200 ms)

class LatencyTracker:
    def __init__(self, delay_ms=0):
        self.delay_s = delay_ms / 1000.0
        self.history = collections.deque() # (timestamp, x, y)

    def push_mouse_pos(self, x, y):
        now = time.perf_counter()
        self.history.append((now, x, y))
        return self.get_delayed_pos(now)

    def get_delayed_pos(self, now):
        target_time = now - self.delay_s
        while len(self.history) > 1 and self.history[1][0] <= target_time:
            self.history.popleft()
        return self.history[0][1], self.history[0][2]

if __name__ == "__main__":
    print("Simulateur de latence de curseur (0 a 200 ms)")
    tracker = LatencyTracker(delay_ms=60)
    print("Pret pour les tests de perception utilisateur.")
