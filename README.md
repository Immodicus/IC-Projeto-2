# Information and codification (IC) - Project 2

---

## Group members - G11

|  NMec | Name                | email                   |
| ----: | ------------------- | ----------------------- |
| 97147 | Jodionísio Muachifi | jodionsiomuachifi@ua.pt |
| 97688 | Rúben Castelhano    | rubencastelhano@ua.pt   |
| 93413 | Tiago Rodrigues     | rodriguestiago@ua.pt    |

## Building:

- git clone git@github.com:Immodicus/IC-Projeto-2.git
- cd IC-Projeto-2
- mkdir build && cd build
- cmake ..
- make

## Testing:
The following examples show all possible arguments being used although some of these are optional. You can run any program without arguments to see which arguments are optional and their default values.

### Ex1.
`img_cpy` original_img copy_img

#### Example:
- `img_cpy` 03.256.ppm 03cpy.ppm // copy 03.256.ppm to 03cpy.ppm

### Ex2.
`img_effects` input_img result_img 

#### Note: 
The program will ask you for which effect you want to apply

#### Example:
- `img_effects` 03.256.ppm effected.ppm // apply affects to 03.256.ppm and save them in effected.ppm

### Ex3.
- `golomb_test` 
- `golomb_efficiency` audioFile

#### Examples:
- `golomb_test` 
- `golomb_efficiency` sample01.wav

### Ex4.
`wav_golomb` -m [auto|value] -bs blockSize -range range div inputWav encoded

#### Note: 
The program will ask you for your predictor choice

#### Examples:
- `wav_golomb` sample01.wav encoded // encode sample01.wav to encoded (default settings)
- `wav_golomb` -d encoded decoded.wav // decode encoded to decoded.wav

### Ex5.
`wav_golomb` -m [auto|value] -l numBits -bs blockSize -range range div inputWav encoded

#### Note: 
The program will ask you for your predictor choice

#### Examples:
- `wav_golomb` -l 12 sample01.wav encoded // encode sample01.wav to encoded and quantize residuals to 12 bits
- `wav_golomb` -d encoded decoded.wav // decode encoded to decoded.wav

### Ex6.
`img_codec` -m [auto|value] -range range div input_img encoded

#### Examples:
- `img_codec` 23.256.ppm encoded // encode 23.256.ppm to encoded
- `img_codec` -d encoded decoded.ppm // decode encoded to decoded.ppm