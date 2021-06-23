# Created by Mohamad ChamanMotlagh
# 06/04/2021

import math
# Install following libraries if not installed in venv
import numpy as np
import matplotlib.pyplot as plt


def calculate_toa(sf, pl, cr, bw):
    temp = ((8 * pl) - (4 * sf) + 28 + 16) / (4 * (sf - 2))
    n = 8 + max(math.ceil(temp * (cr + 4)), 0)
    tpl = n * ((2 ** sf) / bw)
    tpr = (4.25 + 8) * ((2 ** sf) / bw)
    toa = tpr + tpl
    return round(toa, 2)


def main():
    cr = 4 / 5  # default code-rate
    bw = 250  # default bandwidth

    sf_start, sf_end = 7, 12 + 1
    spread_factors = []
    for i in range(sf_start, sf_end):
        spread_factors.append(i)
    payload_sized = [16, 32, 51]

    for pl in payload_sized:
        toa = []
        for sf in spread_factors:
            toa.append(calculate_toa(sf, pl, cr, bw))
        plt.plot(np.array(spread_factors), np.array(toa))
        plt.title("Payload = " + str(pl))
        plt.xlabel("Spread factor")
        plt.ylabel("ToA (ms)")
        plt.grid()
        plt.show()


if __name__ == '__main__':
    main()
