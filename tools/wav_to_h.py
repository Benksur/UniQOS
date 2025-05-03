import scipy.io.wavfile as wavfile
import numpy

def to_16bit_wav(filepath):
    sr, data = wavfile.read(filepath)
    data = data[0:(4096*8)]
    length = data.shape[0]/sr
    print(length)
    with open("sm64_mario_boing.h", "a") as f:
        f.write(f"unsigned int sample_rate = {sr};\r\n")
        f.write(f"unsigned int samples = {data.shape[0]};\r\n")
        f.write("int16_t audio[] = {\r\n")
        # data = numpy.int16(data)
        bytestring = ",".join("{}".format(item) for item in data)
        
        f.write(f"{bytestring}\r\n")
        f.write("};")
    # wavfile.write(filepath, sr, numpy.int16(data))


if __name__ == "__main__":
  to_16bit_wav("/home/james/Desktop/sm64_mario_boing.wav")