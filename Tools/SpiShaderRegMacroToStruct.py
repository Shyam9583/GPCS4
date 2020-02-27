import os

def CountSetBits(n):
    count = 0
    while n:
        count += n & 1
        n >>= 1
    return count

def MacroToStruct(src, dst):
    lines = src.readlines()
    reg_name = ''
    struct_start = False
    struct_valid = False
    bit_count_sum = 0
    for line in lines:
        if line[:2] == '//':
            reg_name = line[2:-1]

            if struct_valid:
                if bit_count_sum < 32:
                    bit_reserved = 32 - bit_count_sum
                    dst.write('\tuint32_t {}\t:\t{};\n'.format('reserved', bit_reserved))
                dst.write('};\n\n')
                struct_start = False
                struct_valid = False
                bit_count_sum = 0
            continue

        # filter some registers which we currently don't use.
        # change the filter at your needs for future development.
        if not reg_name.startswith('SPI_SHADER_PGM') and not reg_name.startswith('COMPUTE_PGM'):
            continue

        print(reg_name)
        if not struct_start:
            struct_start = True
            dst.write('struct ' + reg_name + '\n{\n')

        line = line[8:-1]
        parts = line.split(' ')
        pair = [x for x in parts if x != '']
        field_name = pair[0]
        field_value = pair[1]

        if not field_name.endswith('_MASK'):
            continue

        field_value = field_value[:-1]  # rip 'L'
        value = int(field_value, 16)
        bit_count = CountSetBits(value)

        field = field_name[len(reg_name) + 2 : -5].lower()

        if bit_count == 32:
            dst.write('\tuint32_t {};\n'.format(field))
        else:
            dst.write('\tuint32_t {}\t:\t{};\n'.format(field, bit_count))

        struct_valid = True
        bit_count_sum += bit_count


def main():
    src = open('gc_9_2_1_sh_mask.h', 'r')
    dst = open('PsslShaderRegField.h', 'w')

    dst.write('// Note:' + '\n')
    dst.write('// ' + 'The code is auto-generated using {}'.format(os.path.basename(__file__)) + '\n')
    dst.write('// ' + 'Do not edit this file manually,' + '\n')
    dst.write('// ' + 'if there are something wrong, fix the script file instead.' + '\n')
    dst.write('\n\n')

    MacroToStruct(src, dst)
    src.close()
    dst.close()


if __name__ == '__main__':
    main()