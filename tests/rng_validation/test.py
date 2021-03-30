import numpy as np
import matplotlib.pyplot as plt


def test(filename):
    i = 0
    arr = np.zeros(256)
    with open(filename, 'r') as f:
        for line in f:
            arr[int(line)] += 1
            i += 1
    print(filename + ": ")
    print(str(i) + " amount of samples")
    print(arr)
    a = plt.figure()
    axes = a.add_axes([0.1, 0.1, 0.8, 0.8])
    axes.plot(arr, color='red', label=filename)
    axes.set_ylim([0, 5000])
    a.savefig(filename + ".png")
    print()
    print()


def main():
    test('esp8266_rng.txt')


if __name__ == '__main__':
    main()
