# Created by Mohamad ChamanMotlagh
# 06/04/2021
import math


def calculate_bitrate(bw, sf, cr):
    bitrate = bw / (2 ** sf)
    bitrate *= sf * cr
    return round(bitrate, 2)


def calculate_sensitivity(bw, sf):
    snr = 10 * math.log10(31 / (2 ** sf))
    sensitivity = -174 + 6 + snr
    sensitivity += 10 * math.log10(bw * 1000)
    return round(sensitivity, 2)


def calculate_toa(bw, sf, cr, pl):
    temp = ((8 * pl) - (4 * sf) + 28 + 16) / (4 * (sf - 2))
    n = 8 + max(math.ceil(temp * (cr + 4)), 0)
    tpl = n * ((2 ** sf) / bw)
    tpr = (4.25 + 8) * ((2 ** sf) / bw)
    toa = tpr + tpl
    return round(toa, 2)


def print_line():
    for i in range(105):
        print("_", end='')
    print()


def main():
    cr = 4 / 5
    pl = 10
    print_line()
    print("\t\t\t\t125kHz\t\t\t\t\t|\t\t\t150kHz\t\t\t\t|\t\t\t500kHz")
    print_line()
    print("\t    Bitrate\t\tSensitivity\tToA \tBitrate\t\tSensitivity\tToA\t\tBitrate\t\tSensitivity\tToA")
    for sf in [7, 8, 9, 10, 11, 12]:
        print("SF = " + str(sf) + ": ", end='')
        for bw in [125, 250, 500]:  # bw in kHz
            print(str(calculate_bitrate(bw, sf, cr)) + "\t\t" + str(calculate_sensitivity(bw, sf)) + "\t\t" + str(
                calculate_toa(bw, sf, cr, pl)) + "\t|", end='')
        print()


if __name__ == '__main__':
    main()
