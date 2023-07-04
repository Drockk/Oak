import os
from pathlib import Path

import Utils


class PremakeConfiguration:
    premake_version = "5.0.0-beta2"
    premake_zip_urls = f"https://github.com/premake/premake-core/releases/download/v{premake_version}/premake-{premake_version}-windows.zip"
    premake_license_url = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premake_directory = "./Thridparty/Premake/bin"

    @classmethod
    def validate(cls):
        if not cls.check_if_premake_installed():
            print("Premake is not installed.")
            return False

        print(f"Correct Premake located at {os.path.abspath(cls.premake_directory)}")
        return True

    @classmethod
    def check_if_premake_installed(cls):
        premake_exe = Path(f"{cls.premake_directory}/premake5.exe")
        if not premake_exe.exists():
            return cls.install_premake()

        return True

    @classmethod
    def install_premake(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Premake not found. Would you like to download Premake {0:s}? [Y/N]: ".format(
                cls.premake_version))).lower().strip()[:1]
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')

        premake_path = f"{cls.premake_directory}/premake-{cls.premake_version}-windows.zip"
        print("Downloading {0:s} to {1:s}".format(cls.premake_zip_urls, premake_path))
        Utils.download_file(cls.premake_zip_urls, premake_path)
        print("Extracting", premake_path)
        Utils.unzip_file(premake_path, delete_zip_file=True)
        print(f"Premake {cls.premake_version} has been downloaded to '{cls.premake_directory}'")

        premake_license_path = f"{cls.premake_directory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premake_license_url, premake_license_path))
        Utils.download_file(cls.premake_license_url, premake_license_path)
        print(f"Premake License file has been downloaded to '{cls.premake_directory}'")

        return True
