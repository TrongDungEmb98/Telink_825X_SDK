import os
import json
import shutil
import argparse
import ntpath
import datetime

from zipfile import ZipFile

try:
    from tabulate import tabulate
except ModuleNotFoundError:
    print("tabulate is not installed!!!, Use comman: pip install tabulate")
    raise

dfu_dir = "../../dfu"
dfu_manifest = "manifest.json"


def dfu_crc16(crc, data, len):
    poly = [0, 0xa001]
    for i in range(len):
        ds = data[i]
        for j in range(8):
            crc = (crc >> 1) ^ poly[(crc ^ ds) & 1]
            ds = ds >> 1
    return crc


def creat_manifest(crc, length, dfu_path, fw_name, application_id, company_id, application_version, build):

    try:
        os.makedirs(dfu_path)
    except FileExistsError:
        print()

    try:
        path = dfu_path + dfu_manifest
        with open(path, "w") as manifest:

            data = {
                "manifest":
                {
                    "mcu": "tb03",
                    "bin_file": fw_name,
                    "init_packet_data": {
                        "application_version": application_version,
                        "firmware_length": length,
                        "build": build,
                        "application_id": application_id,
                        "company_id": company_id,
                        "crc": crc
                    }
                }
            }
            json.dump(data, manifest, indent=4, sort_keys=True)
            manifest.close()
    except FileNotFoundError:
        print("The 'docs' directory does not exist")


def main():
    dtime = datetime.datetime.now()
    print()
    print()
    parser = argparse.ArgumentParser()
    parser.add_argument('--fw', type=str)
    parser.add_argument('--app_id', type=int)
    parser.add_argument('--company_id', type=int)

    fw_company = parser.parse_args().company_id
    fw_app_id = parser.parse_args().app_id
    fw_app_ver = int('%d%02d%02d'%(dtime.year % 100, dtime.month, dtime.day))
    fw_app_build = int(dtime.timestamp())
    fw_path = parser.parse_args().fw
    fw_name = ntpath.basename(fw_path)

    # print()
    # print(tabulate([['fw_name', fw_name],
    #                 ['fw_path', fw_path],
    #                 ['fw_company', fw_company],
    #                 ['app_id', fw_app_id],
    #                 ['app_ver', fw_app_ver],
    #                 ['app_build', fw_app_build]],
    #                headers=['Name', 'Data']))
    # print()

    crc16 = 0xFFFF
    file_size = os.path.getsize(fw_path)

    f = open(fw_path, "rb")
    while True:
        data = f.read(256)
        if len(data) < 1:
            break
        crc16 = dfu_crc16(crc16, data, len(data))
    f.close()

    file_name = fw_name.replace('.bin', '') + \
        '_TB03_V' + str(fw_app_ver) + '_B' + str(fw_app_build)
    dfu_path = dfu_dir + "/" + file_name + "/"

    print(dfu_path)

    creat_manifest(crc16, file_size, dfu_path, file_name + '.bin',
                   fw_app_id, fw_company, fw_app_ver, fw_app_build)

    shutil.copy(fw_path, dfu_path + file_name + '.bin')
    # with ZipFile(dfu_dir + '/' + file_name + '.zip', 'w') as zipObj:
    #     zipObj.write(dfu_path + file_name + '.bin', file_name + '.bin')
    #     zipObj.write(dfu_path + dfu_manifest, dfu_manifest)
    #     zipObj.close()

    print(tabulate([['Path', fw_path],
                    ['Name', fw_name],
                    ['Size', file_size],
                    ['CRC', hex(crc16)]],
                   headers=['Name', 'Data']))
    print()


if __name__ == "__main__":
    main()
