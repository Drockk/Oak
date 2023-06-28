import os
from pathlib import Path

import Utils


class VulkanConfiguration:
    required_vulkan_version = "1.3."
    install_vulkan_version = "1.3.246.1"
    vulkan_directory = "./Oak/Thirdparty/VulkanSDK"

    @classmethod
    def validate(cls):
        if not cls.check_vulkan_sdk():
            print("Vulkan SDK not installed correctly.")
            return

        if not cls.check_vulkan_sdk_debug_libs():
            print("\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            print("Debug configuration disabled.")

    @classmethod
    def check_vulkan_sdk(cls):
        vulkan_sdk = os.environ.get("VULKAN_SDK")
        if vulkan_sdk is None:
            print("\nYou don't have the Vulkan SDK installed!")
            cls.__install_vulkan_sdk()
            return False
        else:
            print(f"\nLocated Vulkan SDK at {vulkan_sdk}")

        if cls.required_vulkan_version not in vulkan_sdk:
            print(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.required_vulkan_version})")
            cls.__install_vulkan_sdk()
            return False

        print(f"Correct Vulkan SDK located at {vulkan_sdk}")
        return True

    @classmethod
    def __install_vulkan_sdk(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input(
                "Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(
                    cls.install_vulkan_version))).lower().strip()[
                    :1]
            if reply == 'n':
                return
            permission_granted = (reply == 'y')

        vulkan_install_url = f"https://sdk.lunarg.com/sdk/download/{cls.install_vulkan_version}/windows/VulkanSDK-{cls.install_vulkan_version}-Installer.exe"
        vulkan_path = f"{cls.vulkan_directory}/VulkanSDK-{cls.install_vulkan_version}-Installer.exe"
        print("Downloading {0:s} to {1:s}".format(vulkan_install_url, vulkan_path))
        Utils.download_file(vulkan_install_url, vulkan_path)
        print("Running Vulkan SDK installer...")
        os.startfile(os.path.abspath(vulkan_path))
        print("Re-run this script after installation!")
        quit()

    @classmethod
    def check_vulkan_sdk_debug_libs(cls):
        vulkan_sdk = os.environ.get("VULKAN_SDK")
        shadercd_lib = Path(f"{vulkan_sdk}/Lib/shaderc_sharedd.lib")

        return shadercd_lib.exists()


if __name__ == "__main__":
    VulkanConfiguration.validate()
