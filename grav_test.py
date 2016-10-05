import numpy as np
import matplotlib.pyplot as plt

m_earth = 5.972e24 #kg
m_moon = 7.35e22 #kg
m_sun = 1.989e30 #kg

d_earth_sun = 1.496e8 #km
d_earth_moon = 3.844e5 #km

s_earth_sun = 30 #km/sec
s_earth_moon = 1.022 #km/s

G = 6.67408e-20 #6.67408e-11 m3 kg-1 s-2

year_sec = 365 * 24 * 60 * 60



dt = 60
t=np.arange(0,year_sec,dt)
num_points = len(t)
x_earth = np.zeros(num_points)
y_earth = np.zeros(num_points)
xd_earth = np.zeros(num_points)
yd_earth = np.zeros(num_points)
x_moon = np.zeros(num_points)
y_moon = np.zeros(num_points)
xd_moon = np.zeros(num_points)
yd_moon = np.zeros(num_points)

x_earth[0] = 0
y_earth[0] = d_earth_sun
xd_earth[0] = s_earth_sun
yd_earth[0] = 0

x_moon[0] = 0
y_moon[0] = d_earth_sun + d_earth_moon
xd_moon[0] = s_earth_sun + s_earth_moon
yd_moon[0] = 0

f_const_sun = G * m_sun
f_const_earth = G * m_earth

moon_far = True
crosses = []

for i in range(1,num_points):
  x_earth[i] = x_earth[i - 1] + dt * xd_earth[i-1]
  y_earth[i] = y_earth[i - 1] + dt * yd_earth[i - 1]
  r_3rd_earth_sun = (x_earth[i-1] ** 2 + y_earth[i-1] ** 2) ** 1.5
  xd_earth[i] = xd_earth[i - 1] - dt * f_const_sun * x_earth[i - 1] / r_3rd_earth_sun
  yd_earth[i] = yd_earth[i - 1] - dt * f_const_sun * y_earth[i - 1] / r_3rd_earth_sun

  x_moon[i] = x_moon[i - 1] + dt * xd_moon[i - 1]
  y_moon[i] = y_moon[i - 1] + dt * yd_moon[i - 1]

  r_3rd_moon_sun = (x_moon[i - 1] ** 2 + y_moon[i - 1] ** 2) ** 1.5
  xd_moon[i] = xd_moon[i - 1] - dt * f_const_sun * x_moon[i - 1] / r_3rd_moon_sun
  yd_moon[i] = yd_moon[i - 1] - dt * f_const_sun * y_moon[i - 1] / r_3rd_moon_sun

  if not moon_far == (r_3rd_moon_sun > r_3rd_earth_sun):
    crosses.append(i - 1)
    moon_far = not moon_far

  x_m_e = x_moon[i - 1] - x_earth[i - 1]
  y_m_e = y_moon[i - 1] - y_earth[i - 1]
  r_3rd_moon_earth = (x_m_e ** 2 + y_m_e ** 2) ** 1.5
  xd_moon[i] = xd_moon[i] - dt * f_const_earth * x_m_e / r_3rd_moon_earth
  yd_moon[i] = yd_moon[i] - dt * f_const_earth * y_m_e / r_3rd_moon_earth




plt.plot(x_earth, y_earth,'bx')
plt.plot(x_moon, y_moon,'gx')
plt.plot(x_earth[crosses], y_earth[crosses],'ro')

plt.figure()
x_m_e = x_moon-x_earth
y_m_e = y_moon - y_earth
plt.plot(x_m_e, y_m_e,'gx')
plt.plot(x_m_e[crosses], y_m_e[crosses],'ro')

plt.show()




