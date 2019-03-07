import numpy as np
import matplotlib.pyplot as plt
import math

import sys

rs = float(sys.argv[1])
r = float(sys.argv[2])
phi = float(sys.argv[3]) * math.pi / 180.0
alpha = float(sys.argv[4]) * math.pi / 180.0
v = float(sys.argv[5])
time = float(sys.argv[6])
dt = float(sys.argv[7])
save = False
if (len(sys.argv) > 8):
    save = True
    name = sys.argv[8]

iter = int(time / dt)

x = np.empty(iter)
y = np.empty(iter)

max_radius = r
rdot = math.cos(alpha) * v
phidot = math.sin(alpha) * v / r

rrs = 1.0 - rs / r
actual_v = math.sqrt((rdot / rrs) ** 2 + (phidot * r / math.sqrt(rrs)) ** 2)
max_speed = 0
min_radius = r
assert(actual_v <= 1.0)
print(actual_v)

for i in range(0, iter):
    if (r > max_radius):
        max_radius = r

    if (r < min_radius):
        min_radius = r

    x[i] = r
    y[i] = phi

    rrs = 1.0 - rs / r

    ldot = math.sqrt(rdot ** 2 + (r * phidot) ** 2)
    if (ldot > max_speed):
        max_speed = ldot

    rddot = -rs * rrs / (2.0 * r * r) + (3.0 * rs * (rdot ** 2)) / (2.0 * r * r * rrs) + (r - rs) * (phidot ** 2)
    phiddot = -2.0 * rdot * phidot / r + rs * rdot * phidot / (r * r * rrs)

    new_rdot = rdot + rddot * dt
    new_phidot = phidot + phiddot * dt

    r = r + (rdot + new_rdot) * dt / 2.0
    phi = phi + (phidot + new_phidot) * dt / 2.0
    if phi > 2.0 * math.pi:
        phi = phi - 2.0 * math.pi
    elif phi < 0.0:
        phi = phi + 2.0 * math.pi

    rdot = new_rdot
    phidot = new_phidot

print(r)

ax = plt.subplot(111, projection='polar')
ax.plot(y, x)
ax.set_rmax(max_radius+1)
ax.set_rticks([rs])
ax.set_rlabel_position(-22.5)  # get radial labels away from plotted line
ax.grid(True)

ax.set_title("Closest approach = " + str(min_radius / rs) + "rs\nMax apparent speed = " + str(math.sqrt(max_speed)) + "c", va='bottom')
plt.show()
