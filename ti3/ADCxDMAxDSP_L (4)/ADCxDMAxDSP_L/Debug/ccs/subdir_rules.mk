################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
ccs/%.obj: ../ccs/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/Program file/ti/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/ProgramData/program/ti3/ADCxDMAxDSP_L (4)/ADCxDMAxDSP_L" --include_path="D:/Program file/ti/simplelink_msp432e4_sdk_4_20_00_12/source/third_party/CMSIS/Include" --include_path="D:/Program file/ti/simplelink_msp432e4_sdk_4_20_00_12/source/third_party/CMSIS/DSP_Lib/Include" --include_path="D:/Program file/ti/simplelink_msp432e4_sdk_4_20_00_12/source/third_party/CMSIS/DSP_Lib/PrivateInclude" --include_path="D:/Program file/ti/simplelink_msp432e4_sdk_4_20_00_12/source" --include_path="D:/Program file/ti/simplelink_msp432e4_sdk_4_20_00_12/source/ti/net/bsd" --include_path="D:/Program file/ti/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --advice:power=none --define=ARM_MATH_CM4 --define=__FPU_PRESENT=1 --define=DeviceFamily_MSP432E4 --define=__MSP432E401Y__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="ccs/$(basename $(<F)).d_raw" --obj_directory="ccs" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


