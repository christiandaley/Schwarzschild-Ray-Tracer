import numpy as np
import matplotlib.pyplot as plt
import math

import sys

rs = float(sys.argv[1])
r = float(sys.argv[2])
phi = float(sys.argv[3]) * math.pi / 180.0
alpha = float(sys.argv[4]) * math.pi / 180.0
time = float(sys.argv[5])
dt = float(sys.argv[6])
save = False
if (len(sys.argv) > 7):
    save = True
    name = sys.argv[7]

iter = int(time / dt)

r_data = np.empty(0)
phi_data = np.empty(0)

x = r * math.cos(phi)
y = r * math.sin(phi)
max_radius = r
vx = math.cos(alpha) * math.cos(phi) - math.sin(alpha) * math.sin(phi)
vy = math.cos(alpha) * math.sin(phi) + math.sin(alpha) * math.cos(phi)

for i in range(0, iter):
    r = math.sqrt(x * x + y * y)
    #if (r <= 1.0):
        #break

    if (r > max_radius):
        max_radius = r

    r_data = np.append(r_data, r)
    phi_data = np.append(phi_data, math.atan2(y, x))

    cos2_alpha = ((x * vx + y * vy) / r) ** 2
    sin2_alpha = 1.0 - cos2_alpha

    if (sin2_alpha == 0.0):
        accel = 0.0
    else:
        cot2_alpha = cos2_alpha / sin2_alpha
        if (r >= rs):
            accel = -3.0 * rs / (2.0 * ((r - rs) / sin2_alpha + rs) ** 2)
        else:
            accel = -3.0 * rs / (2.0 * ((r - rs) / sin2_alpha + rs) ** 2)

    newVx = vx + (x / r) * accel * dt
    newVy = vy + (y / r) * accel * dt
    norm = math.sqrt(newVx ** 2 + newVy ** 2)

    newVx = newVx / norm
    newVy = newVy / norm

    effectiveVx = vx + newVx
    effectiveVy = vy + newVy
    norm = math.sqrt(effectiveVx ** 2 + effectiveVy ** 2)
    effectiveVx = effectiveVx / norm
    effectiveVy = effectiveVy / norm

    x = x + effectiveVx * dt
    y = y + effectiveVy * dt

    vx = newVx
    vy = newVy

ax = plt.subplot(111, projection='polar')
ax.plot(phi_data, r_data)
ax.set_rmax(max_radius * 1.1)
ax.set_rticks([rs])
ax.set_rlabel_position(-22.5)  # get radial labels away from plotted line
#ax.grid(True)

ax.set_title("Initial r = " + sys.argv[2] + ", alpha = " + sys.argv[4] + "\nrs = " + sys.argv[1] + ", travel distance = " + sys.argv[5], va='bottom')

if save:
    plt.savefig(name + ".png", bbox_inches = "tight", transparent = True)
else:
    plt.show()
