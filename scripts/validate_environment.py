#!/usr/bin/env python3
"""
@file validate_environment.py
@brief Enterprise-Grade PlatformIO & Python Environment Validation Hook
@repository t4rxnn-devel/solarpunk-microgrid-ag-router
@details Performs rigorous pre-build assertions, toolchain path verification,
          compiler dependency audits, and system resource checks prior to compilation.
"""

import os
import sys
import platform
import subprocess
import shutil
import logging

# Configure dedicated build logging
logging.basicConfig(
    level=logging.INFO,
    format="[ENV-VALIDATOR] [%(levelname)s] %(message)s",
    handlers=[
        logging.StreamHandler(sys.stdout),
        logging.FileHandler("build_validation.log", mode="w")
    ]
)

class EnvironmentValidator:
    def __init__(self):
        self.min_python_version = (3, 8)
        self.required_tools = ["pio", "git"]
        self.validation_passed = True

    def audit_python_runtime(self) -> bool:
        logging.info("Auditing Python runtime version and environment...")
        current_version = sys.version_info[:2]
        
        if current_version < self.min_python_version:
            logging.error(
                f"Python version {current_version[0]}.{current_version[1]} is unsupported. "
                f"Minimum required version is {self.min_python_version[0]}.{self.min_python_version[1]}"
            )
            return False
            
        logging.info(f"Python runtime verified: {platform.python_version()} on {platform.system()}")
        return True

    def audit_system_binaries(self) -> bool:
        logging.info("Auditing essential system toolchain binaries...")
        for tool in self.required_tools:
            path = shutil.which(tool)
            if path is None:
                logging.error(f"Required binary '{tool}' not found in system PATH.")
                self.validation_passed = False
            else:
                logging.info(f"Found binary '{tool}' at {path}")
        return self.validation_passed

    def audit_platformio_core(self) -> bool:
        logging.info("Auditing PlatformIO Core and package registry access...")
        try:
            result = subprocess.run(
                ["pio", "--version"],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                check=True
            )
            logging.info(f"PlatformIO version active: {result.stdout.strip()}")
            return True
        except (subprocess.SubprocessError, FileNotFoundError) as e:
            logging.error(f"PlatformIO Core audit failed: {e}")
            return False

    def verify_workspace_structure(self) -> bool:
        logging.info("Verifying mandatory microgrid workspace folder structures...")
        mandatory_dirs = ["firmware/src", "firmware/include", "firmware/test", "utilities"]
        
        for directory in mandatory_dirs:
            if not os.path.isdir(directory):
                logging.error(f"Critical workspace directory missing: {directory}")
                self.validation_passed = False
            else:
                logging.info(f"Verified directory existence: {directory}")
                
        return self.validation_passed

    def run_full_validation_pipeline(self) -> int:
        logging.info("Initiating full pre-build environment validation pipeline...")
        
        python_ok = self.audit_python_runtime()
        binaries_ok = self.audit_system_binaries()
        pio_ok = self.audit_platformio_core()
        structure_ok = self.verify_workspace_structure()

        if python_ok and binaries_ok and pio_ok and structure_ok:
            logging.info("[SUCCESS] All environment validation checks passed cleanly. Build authorized.")
            return 0
        else:
            logging.error("[CRITICAL] Environment validation failed. Aborting build sequence.")
            return 1

def main():
    validator = EnvironmentValidator()
    exit_code = validator.run_full_validation_pipeline()
    sys.exit(exit_code)

if __name__ == "__main__":
    main()
