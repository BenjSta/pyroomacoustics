'''
In this example, we construct an L-shape 3D room. We use the same floor as in
the 2D example and extrude a 3D room from the floor with a given height.  This
is a simple way to create 3D rooms that fits most situations.  Then, we place
one source and two microphones in the room and compute the room impulse
responses.

In this example, we also compare the speed of the C extension module to
that of the pure python code.
'''
from __future__ import print_function

import numpy as np
import matplotlib.pyplot as plt
import time
import pyroomacoustics as pra

# Create the 2D L-shaped room from the floor polygon
pol = 10 * np.array([[0,0], [0,1], [2,1], [2,0.5], [1,0.5], [1,0]]).T
r_absor = 0.1
room = pra.Room.from_corners(pol, fs=16000, max_order=6, absorption=r_absor)

# Create the 3D room by extruding the 2D by 3 meters
room.extrude(3., absorption=r_absor)

# Add a source somewhere in the room
room.add_source([1.5, 1.2, 0.5])

R = np.array([[3., 1.],
              [2.25, 1.],
              [0.6, 1.]])

room.add_microphone_array(pra.MicrophoneArray(R, room.fs))

room.compute_rir(mode='hybrid', nb_thetas=100, nb_phis=100)



room.plot_rir()


plt.show()
